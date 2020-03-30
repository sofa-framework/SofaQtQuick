#include "ProfilerTreeViewModel.h"

namespace sofaqtquick::_profilertreeviewmodel_ {

ProfilerTreeViewModel::ProfilerTreeViewModel(QObject* parent)
    : QAbstractItemModel(parent)
{

}





ProfilerTreeViewModel::~ProfilerTreeViewModel() {
    delete m_steps.front();
    m_steps.pop_front();
}

void ProfilerTreeViewModel::activateTimer(bool activate, const QString& idString)
{
    sofa::helper::AdvancedTimer::setEnabled(idString.toStdString(), activate);
    sofa::helper::AdvancedTimer::setInterval(idString.toStdString(), 1);
    sofa::helper::AdvancedTimer::setOutputType(idString.toStdString(), "gui");
}

void ProfilerTreeViewModel::setProfilerTo(int stepNumber)
{
    m_stepNumber = stepNumber;
    beginResetModel();
    endResetModel();

}

QVariant ProfilerTreeViewModel::recordStep(int step, const QString& idString)
{
    m_steps.push_back(new AnimationStepData(step, idString.toStdString()));
    if (m_steps.size() > m_bufferSize)
        m_steps.pop_front();
    return QVariant::fromValue(m_steps.back()->m_totalMs);
}

















QModelIndex ProfilerTreeViewModel::index(AnimationStepData* stepData)
{
    if (!stepData)
        return QModelIndex();

    if (!stepData->m_parent)
        return createIndex(0,0, stepData);
    auto parent = stepData->m_parent;
    size_t row = size_t(std::distance(parent->m_substeps.begin(),
                                      std::find(parent->m_substeps.begin(), parent->m_substeps.end(), stepData)));
    return createIndex(int(row), 0, stepData);
}

// Row will indicate the index of the idString within its parent, parent is the parent step, column is unused
QModelIndex ProfilerTreeViewModel::index(int row, int column, const QModelIndex &parent) const
{
    if (m_steps.empty()) return QModelIndex();

    // root items
    if (!parent.isValid())
    {
        if (!row && !column)
            return createIndex(0,0, m_steps[size_t(m_stepNumber)]);
        else return QModelIndex();
    }
    AnimationStepData* stepData = static_cast<AnimationStepData*>(parent.internalPointer());
    if (!stepData)
        return QModelIndex();
    if (stepData->m_substeps.size() < size_t(row))
        return QModelIndex();
    return createIndex(row, 0, stepData->m_substeps[size_t(row)]);
}

// Returns the index of the parent step
QModelIndex ProfilerTreeViewModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return QModelIndex();

    AnimationStepData* stepData = static_cast<AnimationStepData*>(index.internalPointer());
    if (!stepData)
        return QModelIndex();

    if  (!stepData->m_parent)
        return QModelIndex();

    auto parent = stepData->m_parent;

    if  (!parent)
        return createIndex(0, 0, parent);

    auto gdParent = parent->m_parent;
    size_t row = size_t(std::distance(gdParent->m_substeps.begin(),
                                    std::find(gdParent->m_substeps.begin(), gdParent->m_substeps.end(), parent)));
    return createIndex(int(row), 0, parent);
}

// Whether or not this step has substeps
bool ProfilerTreeViewModel::hasChildren(const QModelIndex &index) const
{
    return rowCount(index) > 0;
}

// Number of substeps within a step
int ProfilerTreeViewModel::rowCount(const QModelIndex &index) const
{
    if (!index.isValid())
        return 1;
    if (index.internalPointer())
    {
        AnimationStepData* stepData = static_cast<AnimationStepData*>(index.internalPointer());
        return int(stepData->m_substeps.size());
    }
}

// Always 1 in our case
int ProfilerTreeViewModel::columnCount(const QModelIndex &index) const
{
    SOFA_UNUSED(index);
    return 1;
}

QVariant ProfilerTreeViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return false;
    auto stepData = static_cast<AnimationStepData*>(index.internalPointer());
    switch(static_cast<Roles>(role))
    {
    case Roles::Name:
        return QVariant(QString::fromStdString(stepData->m_name));
    case Roles::Label:
        return QVariant(QString::fromStdString(stepData->m_name));
    case Roles::Icon:
        return QVariant(QString::fromStdString(stepData->m_name));
    case Roles::SelfMs:
        return QVariant(stepData->m_selfMs);
    case Roles::TotalMs:
        return QVariant(stepData->m_totalMs);
    case Roles::SelfPerc:
        return QVariant(stepData->m_selfPercent);
    case Roles::TotalPerc:
        return QVariant(stepData->m_totalPercent);
    }
    return QVariant("INVALID ROLE");
}

QHash<int, QByteArray> ProfilerTreeViewModel::roleNames() const
{
    /// Create a static mapping to link the "role" number as Qt is using and the corresponding
    /// textual represresentation; The Roles are encoded using an 'enum class' so they don't
    /// automatically convert to int value in the constructor initializer_list.
    static QHash<int, QByteArray> mapping
    {{
            { int(Roles::Name),      "name" },
            { int(Roles::Label),     "label" },
            { int(Roles::Icon),      "icon" },
            { int(Roles::TotalPerc), "totalPerc" },
            { int(Roles::TotalMs),   "totalMs" },
            { int(Roles::SelfPerc),  "selfPerx" },
            { int(Roles::SelfMs),    "selfMs" }
    }};
    return mapping;
}

}  // namespace sofaqtquick::_profilertreeviewmodel_
