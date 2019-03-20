#ifndef SOFAPROJECT_H
#define SOFAPROJECT_H

#include <SofaQtQuickGUI/config.h>
#include <QObject>
#include <QUrl>

namespace sofa {
namespace qtquick {

/**
 *  \brief Holds the current project's properties
 *
 *   A SOFA project is defined by a directory, containing scene files, assets, plugins etc, and respecting a specific directory tree
 */

class SOFA_SOFAQTQUICKGUI_API SofaProject : public QObject
{
    Q_OBJECT
public:
    SofaProject();
    ~SofaProject();

    Q_PROPERTY(QUrl rootDir READ getRootDir WRITE setRootDir NOTIFY rootDirChanged)
    const QUrl& getRootDir();
    void setRootDir(const QUrl& rootDir);
    Q_SIGNAL void rootDirChanged(QUrl& rootDir);

private:
    QUrl m_rootDir;

};

}  // namespace qtquick
}  // namespace sofa

#endif // SOFAPROJECT_H
