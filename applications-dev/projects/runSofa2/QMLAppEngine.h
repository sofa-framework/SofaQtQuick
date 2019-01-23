#include <QtGui>
#include <QtQuick>

// Use QmlLive headers
#include "livenodeengine.h"
#include "remotereceiver.h"

class MyQmlApplicationEngine : public QQmlApplicationEngine
{
    Q_OBJECT

public:
    MyQmlApplicationEngine(const QString &mainQml); // Perform some setup here

    QString mainQml() const;
    QQuickWindow *mainWindow();
    QList<QQmlError> warnings() const;

    // ...
};
