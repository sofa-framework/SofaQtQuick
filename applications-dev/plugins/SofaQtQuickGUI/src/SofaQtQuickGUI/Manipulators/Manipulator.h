#pragma once

#include <QObject>
#include <QPointF>
#include <SofaQtQuickGUI/SofaViewer.h>

namespace sofaqtquick
{

class Manipulator : public QObject
{
    Q_OBJECT
public:
    Manipulator(QObject* parent = nullptr);


    /// The name of the manipulator (e.g. Translate_Manipulator)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int index READ getIndex WRITE setIndex NOTIFY indexChanged)


    const QString& getName();
    void setName(const QString&);

    int getIndex();
    void setIndex(int);
signals:
    void nameChanged(const QString&);
    void indexChanged(int);

public:
    void draw(const SofaViewer& viewer);
    void pick(const SofaViewer& viewer, int& pickIndex);

    virtual void internalDraw(const SofaViewer& viewer, int pickIndex, bool isPicking = false) = 0;
    Q_INVOKABLE virtual void manipulate(const QPointF& mouse, SofaViewer* viewer) const = 0;
    virtual int getIndices() const = 0;

protected:
    QString m_name;
    int m_index;
};

}  // namespace sofaqtquick
