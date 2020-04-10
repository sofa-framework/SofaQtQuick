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

void ProfilerTreeViewModel::seek(int stepNumber)
{
    beginResetModel();
    m_stepNumber = stepNumber;
    endResetModel();
}

QVariant ProfilerTreeViewModel::recordStep(int step, const QString& idString)
{
    m_steps.push_back(new AnimationStepData(step, idString.toStdString()));
    if (m_steps.size() > m_bufferSize)
        m_steps.pop_front();
    return QVariant::fromValue(m_steps.back()->m_totalMs);
}

void ProfilerTreeViewModel::clearBuffer()
{
    beginResetModel();
    m_steps.clear();
    endResetModel();
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

    AnimationStepData* stepData;
    if (!parent.isValid())
        stepData = m_steps[size_t(m_stepNumber)];
    else
        stepData = static_cast<AnimationStepData*>(parent.internalPointer());

    stepData = stepData->m_substeps[size_t(row)];
    if (!stepData)
        return QModelIndex();
    return createIndex(row, column, stepData);
}

// Returns the index of the parent step
QModelIndex ProfilerTreeViewModel::parent(const QModelIndex &index) const
{
    if(!index.isValid()) {
        return QModelIndex();
    }

    AnimationStepData* stepData = static_cast<AnimationStepData*>(index.internalPointer());
    AnimationStepData* parent = stepData->m_parent;
    if (parent == m_steps[size_t(m_stepNumber)])
        return QModelIndex();

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
    if (!index.isValid()) {
        return 1;
    }
    if (index.internalPointer())
    {
        AnimationStepData* stepData = static_cast<AnimationStepData*>(index.internalPointer());
        return int(stepData->m_substeps.size());
    }
    return 1;
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
    case Roles::Step:
        return QVariant(stepData->m_level > 0 ? QString("") : QString::number(stepData->m_stepIteration));
    case Roles::Label:
        return QVariant(QString::fromStdString(stepData->m_name));
    case Roles::Level:
        return QVariant(stepData->m_level);
    case Roles::Icon:
        return QVariant(QString::fromStdString(stepData->m_name));
    case Roles::SelfMs:
        return (stepData->m_level != 1 ? QVariant(QString::number(stepData->m_selfMs) + " ms") : "");
    case Roles::TotalMs:
        return (stepData->m_level == 1 ? QVariant(QString::number(stepData->m_totalMs) + " ms") : "");
    case Roles::SelfPerc:
        return (stepData->m_level != 1 ? QVariant(QString::number(stepData->m_selfPercent) + " %") : "100");
    case Roles::TotalPerc:
        return (stepData->m_level != 1 ? QVariant(QString::number(stepData->m_totalPercent) + " %") : "100");
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
            { int(Roles::Step),      "step" },
            { int(Roles::Label),     "label" },
            { int(Roles::Level),     "level" },
            { int(Roles::Icon),      "icon" },
            { int(Roles::TotalPerc), "totalPerc" },
            { int(Roles::TotalMs),   "totalMs" },
            { int(Roles::SelfPerc),  "selfPerx" },
            { int(Roles::SelfMs),    "selfMs" }
    }};
    return mapping;
}

}  // namespace sofaqtquick::_profilertreeviewmodel_
