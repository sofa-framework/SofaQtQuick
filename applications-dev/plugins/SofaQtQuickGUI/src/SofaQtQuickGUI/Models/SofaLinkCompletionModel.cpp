#include "SofaLinkCompletionModel.h"
#include <sofa/core/objectmodel/Base.h>
#include <SofaSimulationGraph/DAGNode.h>
#include <sofa/core/ObjectFactory.h>

namespace sofaqtquick {

using namespace sofa::core::objectmodel;

void SofaLinkCompletionModel::setSofaData(sofaqtquick::bindings::_sofadata_::SofaData *newSofaData)
{
    m_sofaData = newSofaData;
    sofa::core::objectmodel::Base* base = newSofaData->rawData()->getOwner();
    m_relativeRoot = base->toBaseNode();
    if (!m_relativeRoot) {
        sofa::core::objectmodel::BaseObject* baseobject = base->toBaseObject();
        if (baseobject) {
            m_relativeRoot = baseobject->getContext()->toBaseNode();
        } else {
            msg_error("SofaLinkCompletionModel") << "Can't fetch links: sofaData " + m_sofaData->getName().toStdString() + " has no context.";
        }
    }
    m_absoluteRoot = m_relativeRoot->getRoot();
    updateModel();
    emit sofaDataChanged(m_sofaData);
}

void SofaLinkCompletionModel::setLinkPath(QString newlinkPath)
{
    std::cout << "c++: " << newlinkPath.toStdString() << std::endl;
    m_linkPath = newlinkPath;
    updateModel();
    emit linkPathChanged(newlinkPath);
}


Base* SofaLinkCompletionModel::getLastValidObject(QString& lastValidLinkPath)
{
    lastValidLinkPath = "";
    if (!m_linkPath.startsWith("@"))
        return nullptr;

    QString linkpath = m_linkPath.remove(0, 1);
    QStringList strlist = m_linkPath.split("/");
    BaseNode* root = m_relativeRoot->toBaseNode();
    int i = -1;
    for (auto s : strlist)
    {
        i++;

        if (i == 0 && s == "") // If First character is root
        {
            lastValidLinkPath.push_back(s);
            lastValidLinkPath.push_back("/");
            root = m_absoluteRoot->toBaseNode();
        }
        else if (s == "" && i != strlist.size() - 1) // if a path is empty (double slash //)
        {
            return nullptr;
        }
        else if (s == "..")
        {
            lastValidLinkPath.push_back(s);
            lastValidLinkPath.push_back("/");
            if (root->getFirstParent() != nullptr)
                root = root->getFirstParent();
        }
        else if (s == ".")
        {
            lastValidLinkPath.push_back(s);
            lastValidLinkPath.push_back("/");
            continue;
        }
        else
        {
            if (i == strlist.size() - 1) {
                if (s.contains(".")) {
                    s = s.split(".")[0];
                }
            }
            auto n = static_cast<sofa::simulation::Node*>(root);
            if (!n) return nullptr;
            if (n->getChild(s.toStdString()) != nullptr)
            {
                lastValidLinkPath.push_back(s);
                lastValidLinkPath.push_back("/");
                root = n->getChild(s.toStdString());
            }
            else if (n->getObject(s.toStdString()) != nullptr) {
                lastValidLinkPath.push_back(s);
                return n->getObject(s.toStdString());
            }
            else {
                msg_error("") << "no item named " << s.toStdString() << " in " << n->getPathName();
                return root;
            }

        }
    }
    return root;
}

void SofaLinkCompletionModel::updateModel()
{
    beginResetModel();
    QString lastValidLinkPath = "";
    std::string path_separator = "/";
    std::string data_separator = ".";
    Base* lastValid = this->getLastValidObject(lastValidLinkPath);
    if (lastValidLinkPath[lastValidLinkPath.size() -1] == "/")
        path_separator = "";
    if (lastValidLinkPath[lastValidLinkPath.size() -1] == ".")
        data_separator = "";
    m_modelText.clear();
    m_modelName.clear();
    m_modelHelp.clear();

    if (!lastValid) {
        msg_error("SofaLinkCompletionModel") << "invalid object";
        return;
    }
    msg_error("SofaLinkCompletionModel") << "valid object found: " << lastValid->getName();
    if (lastValid->toBaseNode())
    {
        BaseNode* node = lastValid->toBaseNode();
        for (auto child : node->getChildren())
        {
            m_modelText.push_back(lastValidLinkPath+QString::fromStdString(path_separator+child->getName()));
            m_modelName.push_back(QString::fromStdString(child->getName()));
            m_modelHelp.push_back(QString::fromStdString(child->getTypeName()));
        }
        sofa::simulation::graph::DAGNode* n = static_cast<sofa::simulation::graph::DAGNode*>(node);
        for (auto object : n->object)
        {
            m_modelText.push_back(lastValidLinkPath+QString::fromStdString(path_separator+object->getName()));
            m_modelName.push_back(QString::fromStdString(object->getName()));
            m_modelHelp.push_back(QString::fromStdString(
                                      sofa::core::ObjectFactory::getInstance()
                                      ->getEntry(object->getClassName()).description));
        }
    }

    for (auto data : lastValid->getDataFields())
    {
        m_modelText.push_back(lastValidLinkPath+QString::fromStdString(data_separator+data->getName()));
        m_modelName.push_back(QString::fromStdString(data->getName()));
        m_modelHelp.push_back(QString::fromStdString(data->getHelp()));
    }
    m_modelText.erase(std::remove_if(m_modelText.begin(), m_modelText.end(),
                     [this](const QString &s) { return !s.contains(m_linkPath); }), m_modelText.end());
    msg_error("SofaLinkCompletionModel") << "all completions:" << m_modelText.join("\n").toStdString();
    endResetModel();
}

int	SofaLinkCompletionModel::rowCount(const QModelIndex & parent) const
{
    if (parent.isValid())
        return 0;

    return m_modelText.size();
}

QVariant SofaLinkCompletionModel::data(const QModelIndex &index, int role) const
{
    QString v = "";
    switch (role)
    {
    case CompletionRole:
        v = m_modelText[index.row()];
        v = v.remove(0, m_linkPath.size());
        break;
    case NameRole:
        v = m_modelName[index.row()];
        break;
    case HelpRole:
        v = m_modelHelp[index.row()];
        break;
    default:
        break;
    }
    return QVariant::fromValue<QString>(v);
}

QHash<int, QByteArray> SofaLinkCompletionModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[CompletionRole] = "completion";
    roles[NameRole] = "name";
    roles[HelpRole] = "help";
    return roles;
}

}
