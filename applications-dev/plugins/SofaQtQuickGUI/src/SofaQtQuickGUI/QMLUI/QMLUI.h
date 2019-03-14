/*********************************************************************
Copyright 2019, Inria, CNRS, University of Lille

This file is part of runSofa2

runSofa2 is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

runSofa2 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
/********************************************************************
 Contributors:
    - damien.marchal@univ-lille.fr
********************************************************************/
#pragma once
#include <QObject>
#include <QQuickItem>
#include <QQmlContext>

namespace sofaqtquick::qmlui
{
    typedef QList<QUrl> QUrlList;

    class QmlUIManager : public QObject
    {
        Q_OBJECT

    public:
        Q_INVOKABLE void loadCanvasFrom(const QUrl& filename);

      signals:
        void notifyLoadCanvasFrom(const QUrl& filename);
    };

    class QmlUIManagerSingleton : public QObject
    {
    public:
        static QmlUIManager* GetInstance();
        static void LoadCanvasFrom(const QUrl& filename);
    };

    /// Load UI element interfaces into a QML view.
    ///
    /// The QmlUILoader should be associated to a QML widget
    /// and will inject into the widget's tree a set of QML component
    /// loaded from files.
    ///
    /// To work, the QmlUILoader need to be attached to valid
    /// qml context.
    class QmlUILoader : public QQuickItem
    {
        Q_OBJECT

    public:
        QmlUILoader(QObject* parent=nullptr);
        ~QmlUILoader() override;

    public slots:
        void resetAndLoadAll(const QUrlList& list);
        void load(const QUrl& filename);

    private:
        QList<QQuickItem*> m_loadedItems;
    };
}
