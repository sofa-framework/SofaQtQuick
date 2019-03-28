#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include <QObject>

class ProjectView : public QObject
{
    Q_OBJECT
public:
    explicit ProjectView(QObject *parent = nullptr);

signals:

public slots:
};

#endif // PROJECTVIEW_H