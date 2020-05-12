#include "SofaCameraListModel.h"
#include "SofaQtQuickGUI/Bindings/SofaCamera.h"

namespace sofaqtquick
{

using sofaqtquick::binding::SofaCamera;

SofaCameraListModel::SofaCameraListModel(QObject* parent)
    : QAbstractListModel (parent),
      m_sofaScene(nullptr)
{
    connect(this, &SofaCameraListModel::sofaSceneChanged,
            this, &SofaCameraListModel::forceUpdate);
}

SofaCameraListModel::~SofaCameraListModel()
{

}

SofaBaseScene *SofaCameraListModel::sofaScene() const
{
    return m_sofaScene;
}

void SofaCameraListModel::setSofaScene(SofaBaseScene* newSofaScene)
{
    m_sofaScene = newSofaScene;
    emit sofaSceneChanged(m_sofaScene);
}

void SofaCameraListModel::update(bool reset)
{
    if(m_sofaScene==nullptr)
        return;

    auto& node = m_sofaScene->sofaRootNode();
    if(node==nullptr)
        return;

    auto cams = node->getTreeObjects<BaseCamera>();
    if (cams.size() != m_cameras.size() || reset) {
        emit beginResetModel();
        m_cameras = cams;
        emit endResetModel();
    }
}

void SofaCameraListModel::forceUpdate()
{
    update(true);
}

int SofaCameraListModel::rowCount(const QModelIndex &parent) const
{
    SOFA_UNUSED(parent);
    return int(m_cameras.size());
}

QVariant SofaCameraListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
    {
        msg_error("SofaQtQuickGUI") << "SofaCameraListModel::data  Invalid index";
        return QVariant("");
    }

    size_t idx = size_t(index.row());
    if(idx >= m_cameras.size())
        return QVariant("");

    auto cam = m_cameras[idx];

    switch(role)
    {
    case NameRole:
        return QVariant::fromValue(QString::number(idx) + ". " +  cam->getName().c_str());
    case ValueRole:
        return QVariant::fromValue(new SofaBaseObject(BaseCamera::SPtr(cam)));
    default:
        msg_error("SofaQtQuickGUI") << "SofaDataListModel::data  Role unknown:" << role;
        break;
    }

    return QVariant("");
}

QHash<int, QByteArray> SofaCameraListModel::roleNames() const
{
    QHash<int,QByteArray> roles;

    roles[NameRole] = "name";
    roles[ValueRole]  = "value";

    return roles;
}

}  // namespace sofaqtquick
