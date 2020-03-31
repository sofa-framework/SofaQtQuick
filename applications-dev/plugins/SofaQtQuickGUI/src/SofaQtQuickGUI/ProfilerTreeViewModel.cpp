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
    m_stepNumber = stepNumber;
    std::cout << ("reseting model") << std::endl;
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
    std::cout << "index()" << std::endl;
    std::cout << "index r: " << row <<" c: " << column << " p: " << parent.internalPointer() << std::endl;
    if (m_steps.empty()) return QModelIndex();

    // root items
    if (!parent.isValid())
    {
        if (!row && !column) {
            std::cout << "returning root" << std::endl;
            return createIndex(0,0, m_steps[size_t(m_stepNumber)]);
        }
        std::cout << "[OUPS] invalid parent, row col not 0" << std::endl;
        return QModelIndex();
    }
    AnimationStepData* stepData = static_cast<AnimationStepData*>(parent.internalPointer());
    if (!stepData) {
        std::cout << "[OUPS] NO stepData for parent" << std::endl;
        return QModelIndex();
    }
    if (stepData->m_substeps.size() < size_t(row)) {
        std::cout << "[OUPS] requested row number outside of substeps range" << std::endl;
        return QModelIndex();
    }
    std::cout << "returning a substep yay" << std::endl;
    return createIndex(row, 0, stepData->m_substeps[size_t(row)]);
}

// Returns the index of the parent step
QModelIndex ProfilerTreeViewModel::parent(const QModelIndex &index) const
{
    std::cout << "looking for parent of [" << index.row() << ";" << index.column() << " : " << index.internalPointer() << "]" << std::endl;
    if(!index.isValid()) {
        std::cout << "[OUPS] invalid index" << std::endl;
        return QModelIndex();
    }

    AnimationStepData* stepData = static_cast<AnimationStepData*>(index.internalPointer());
    if (!stepData) {
        std::cout << "[OUPS] NO stepData for index" << std::endl;
        return QModelIndex();
    }

    if  (!stepData->m_parent) {
        std::cout << "no parent node for index [" << index.row() << ";" << index.column() << " : " << index.internalPointer() << "]" << std::endl;
        return QModelIndex();
    }

    auto parent = stepData->m_parent;

    if  (!parent) {
        std::cout << "parent of index is root step, returning createIndex(0, 0, parent)" << std::endl;
        return createIndex(0, 0, parent);
    }

    auto gdParent = parent->m_parent;
    size_t row = size_t(std::distance(gdParent->m_substeps.begin(),
                                    std::find(gdParent->m_substeps.begin(), gdParent->m_substeps.end(), parent)));
    std::cout << "found index of parent. returning" << std::endl;
    return createIndex(int(row), 0, parent);
}

// Whether or not this step has substeps
bool ProfilerTreeViewModel::hasChildren(const QModelIndex &index) const
{
    std::cout << "index  has children ? [" << index.row() << ";" << index.column() << " : " << index.internalPointer() << "]: " << (rowCount(index) > 0) << std::endl;
    return rowCount(index) > 0;
}

// Number of substeps within a step
int ProfilerTreeViewModel::rowCount(const QModelIndex &index) const
{
    std::cout << "rowCount of index [" << index.row() << ";" << index.column() << " : " << index.internalPointer() << "]: ";
    if (!index.isValid()) {
        std::cout << 1 << std::endl;
        return 1;
    }
    if (index.internalPointer())
    {
        AnimationStepData* stepData = static_cast<AnimationStepData*>(index.internalPointer());
        std::cout << int(stepData->m_substeps.size()) << std::endl;
        return int(stepData->m_substeps.size());
    }
    std::cout << "[Oups] no internal pointer on valid index" << std::endl;
    return 1;
}

// Always 1 in our case
int ProfilerTreeViewModel::columnCount(const QModelIndex &index) const
{
    SOFA_UNUSED(index);
    std::cout << "Column count 1" << std::endl;
    return 1;
}

QVariant ProfilerTreeViewModel::data(const QModelIndex &index, int role) const
{
    std::cout << "data [" << index.row() << ";" << index.column() << ":" << index.internalPointer() <<  "]" << std::endl;
    if (!index.isValid())
        return false;
    auto stepData = static_cast<AnimationStepData*>(index.internalPointer());
    switch(static_cast<Roles>(role))
    {
    case Roles::Name:
        std::cout << "Name" << std::endl;
        return QVariant(QString::fromStdString(stepData->m_name));
    case Roles::Label:
        std::cout << "Label" << std::endl;
        return QVariant(QString::fromStdString(stepData->m_name));
    case Roles::Icon:
        std::cout << "Icon" << std::endl;
        return QVariant(QString::fromStdString(stepData->m_name));
    case Roles::SelfMs:
        std::cout << "SelfMS" << std::endl;
        return QVariant(stepData->m_selfMs);
    case Roles::TotalMs:
        std::cout << "TotalMs" << std::endl;
        return QVariant(stepData->m_totalMs);
    case Roles::SelfPerc:
        std::cout << "SelfPerc" << std::endl;
        return QVariant(stepData->m_selfPercent);
    case Roles::TotalPerc:
        std::cout << "TotalPerc" << std::endl;
        return QVariant(stepData->m_totalPercent);
    }
    std::cout << "INVALID ROLE" << std::endl;
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
