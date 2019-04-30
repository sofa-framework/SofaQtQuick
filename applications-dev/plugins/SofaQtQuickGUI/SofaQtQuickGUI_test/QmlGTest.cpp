#include <QCoreApplication>

#include <SofaQtQuickGUI/Bindings/SofaCoreBindingContext.h>
using sofaqtquick::bindings::SofaCoreBindingContext;

#include <SofaQtQuickGUI/SofaQtQuickQmlModule.h>
using sofaqtquick::SofaQtQuickQmlModule;

#include "QmlGTest.h"

/// This function is used by gtest to print the content of the struct in a meaninfull way
void PrintTo(const QmlTestData& d, ::std::ostream *os)
{
    (*os) << d.filepath  ;
    (*os) << " with args {" ;
    for(auto& v : d.arguments)
    {
        (*os) << v << ", " ;
    }
    (*os) << "}";
}


///////////////////////// QmlTestData Definition  ///////////////////////////////////////////////
QmlTestData::QmlTestData(const std::string& filepath, const std::string &testgroup,
                         const std::vector<std::string>& arguments ) :
    filepath(filepath), arguments(arguments), testgroup{testgroup} {}


std::vector<QmlTestData> QmlTestList::getList()
{
    if(list.size() == 0)
        doFillList();
    return list;
}

/// add a Python_test_data with given path
void QmlTestList::addGroupOfTests( const std::string& filename,
                                   const std::string& path,
                                   const std::string& testgroup,
                                   const std::vector<std::string>& arguments)
{
    /// Add an test with an empty method name so there is a at least one test...that consist
    /// in loading the qml file.
    list.push_back( QmlTestData( filepath(path,filename), testgroup, {""} ) );

    QQmlEngine* m_engine = new QQmlEngine();
    m_engine->addImportPath("qrc:/");
    m_engine->addImportPath(QCoreApplication::applicationDirPath() + "/../lib/qml/");
    QQmlContext* m_objectContext = new QQmlContext(m_engine->rootContext());

    SofaQtQuickQmlModule::RegisterTypes(m_engine);
    dmsg_info("QmlGTest") << "Registering '" << filename << "'";

    QQmlComponent component(m_engine,
                            QUrl(QString::fromStdString("file://"+path+filename)),
                            QQmlComponent::CompilationMode::PreferSynchronous);


    if(!component.isReady())
    {
        return;
    }
    QObject *object = component.create(m_objectContext);
    QVariant returnedValue;
    QStringList functions;
    const QMetaObject* metaObject = object->metaObject();

    for (int n = 0; n < metaObject->methodCount(); n++)
    {
        QString methodName = QString::fromLocal8Bit(metaObject->method(n).name());
        if(methodName.startsWith("tst_"))
        {
            std::vector<std::string> cargs;
            cargs.push_back(methodName.toStdString());
            cargs.insert(cargs.end(), arguments.begin(), arguments.end());
            list.push_back( QmlTestData( filepath(path,filename), testgroup, cargs ) );
        }
    }

    delete m_objectContext;
    delete m_engine;
}

void QmlTestList::addTestDir(const std::string& dir, const std::string& testgroup, const std::string& prefix)
{
    std::vector<std::string> files;
    sofa::helper::system::FileSystem::listDirectory(dir, files);

    for(const std::string& file : files)
    {
        if( sofa::helper::starts_with(prefix, file) && (sofa::helper::ends_with(".qml", file)))
        {
            addGroupOfTests(file, dir, testgroup);
        }
    }
}

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // construct a msg
    QString sFunction = context.function;

    if(type==QtMsgType::QtSystemMsg && msg.startsWith("FAILURE"))
    {
        return GTEST_MESSAGE_AT_(context.file, context.line, msg.toStdString().c_str(), ::testing::TestPartResult::kFatalFailure);
    }
    if(type==QtMsgType::QtInfoMsg){
        msg_info("QmlInfo") << qUtf8Printable(msg);
        return;
    }
    if(type==QtMsgType::QtWarningMsg){
        return GTEST_MESSAGE_AT_(context.file, context.line, msg.toStdString().c_str(), ::testing::TestPartResult::kFatalFailure);
    }
    if(type==QtMsgType::QtDebugMsg){
        msg_info("QmlDebug") << qUtf8Printable(msg);
        return;
    }
    if(type==QtMsgType::QtSystemMsg){
        msg_info("QmlLog") << qUtf8Printable(msg);
        return;
    }

    msg_error("Qml") << qUtf8Printable(msg);
}

QmlGTest::QmlGTest()
{

}

void QmlGTest::run( const QmlTestData& data )
{

    qInstallMessageHandler(myMessageHandler);
    msg_info("QmlTest") << "running " << data.filepath;

    QQmlEngine* m_engine = new QQmlEngine();
    m_engine->addImportPath("qrc:/");
    m_engine->addImportPath(QCoreApplication::applicationDirPath() + "/../lib/qml/");
    QQmlContext* m_objectContext = new QQmlContext(m_engine->rootContext());

    SofaQtQuickQmlModule::RegisterTypes(m_engine);
    QQmlComponent component(m_engine,
                            QUrl(QString::fromStdString("file://"+data.filepath)),
                            QQmlComponent::CompilationMode::PreferSynchronous);

    if(!component.isReady())
    {
        FAIL() << "Unable to load test file: " << data.filepath << "\n" << component.errorString().toStdString();
    }
    QObject *object = component.create(m_objectContext);
    QVariant returnedValue;
    QStringList functions;

    for(auto& method : data.arguments)
    {
        QMetaObject::invokeMethod(object,
                                  method.c_str(),
                                  Q_RETURN_ARG(QVariant, returnedValue));
    }
}


