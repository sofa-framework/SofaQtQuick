#pragma once

#include <QObject>
#include <QPointF>
#include <sofa/helper/system/config.h>

namespace sofaqtquick
{

class SofaViewer;

class Manipulator : public QObject
{
    Q_OBJECT
public:
    Manipulator(QObject* parent = nullptr);


    /// The name of the manipulator (e.g. Translate_Manipulator)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int index READ getIndex WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(int particleIndex READ getParticleIndex WRITE setParticleIndex NOTIFY particleIndexChanged)
    Q_PROPERTY(QString displayText READ getDisplayText NOTIFY displayTextChanged)
    Q_PROPERTY(bool isEditMode READ isEditMode WRITE toggleEditMode NOTIFY isEditModeChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool persistent READ persistent WRITE setPersistent NOTIFY persistentChanged)


    const QString& getName();
    void setName(const QString&);

    void setIndex(int);
    int getIndex();
    void setParticleIndex(int);
    int getParticleIndex();
    bool isEditMode();
    void toggleEditMode(bool);

    bool enabled() { return m_enabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; emit enabledChanged(m_enabled); }

    bool persistent() { return m_persistent; }
    void setPersistent(bool persistent) { m_persistent = persistent; emit persistentChanged(m_persistent); }

 signals:
    void nameChanged(const QString&);
    void indexChanged(int);
    void particleIndexChanged(int);
    void displayTextChanged(const QString&);
    void isEditModeChanged(bool);
    void enabledChanged(bool);
    void persistentChanged(bool);


public:
    void draw(const SofaViewer& viewer);
    void pick(const SofaViewer& viewer, int& pickIndex);

    virtual void internalDraw(const SofaViewer& viewer, int pickIndex, bool isPicking = false) = 0;


    Q_INVOKABLE virtual void mouseMoved(const QPointF& mouse, SofaViewer* viewer) { SOFA_UNUSED(mouse);SOFA_UNUSED(viewer); }
    Q_INVOKABLE virtual void mousePressed(const QPointF& mouse, SofaViewer* viewer) { SOFA_UNUSED(mouse);SOFA_UNUSED(viewer); }
    Q_INVOKABLE virtual void mouseReleased(const QPointF& mouse, SofaViewer* viewer) { SOFA_UNUSED(mouse);SOFA_UNUSED(viewer); }
    Q_INVOKABLE virtual void mouseClicked(const QPointF& mouse, SofaViewer* viewer) { SOFA_UNUSED(mouse);SOFA_UNUSED(viewer); }

    Q_INVOKABLE virtual void keyPressed(int key, int modifiers, const QString& text) { SOFA_UNUSED(key);SOFA_UNUSED(modifiers);SOFA_UNUSED(text); }
    Q_INVOKABLE virtual void keyReleased(int key, int modifiers, const QString& text) { SOFA_UNUSED(key);SOFA_UNUSED(modifiers);SOFA_UNUSED(text); }

    virtual int getIndices() const = 0;

    virtual QString getDisplayText() const = 0;

protected:
    QString m_name;
    int m_index {0};
    int m_particleIndex {0};
    bool m_isEditMode {false};
    bool m_persistent {false};
    bool m_enabled {false};
};

}  // namespace sofaqtquick
