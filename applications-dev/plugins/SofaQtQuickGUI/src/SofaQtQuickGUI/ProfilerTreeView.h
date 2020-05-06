#ifndef PROFILERTREEVIEW_H
#define PROFILERTREEVIEW_H

#include <sofa/helper/system/thread/CTime.h>
#include <sofa/helper/AdvancedTimer.h>

#include <QTreeView>
#include <QTreeWidgetItem>
#include <queue>

using namespace sofa::helper::system::thread;
using namespace sofa::helper;

namespace sofaqtquick {


/**
 * @brief The AnimationSubStepData Internal class to store data for each step of the animation. Correspond to one AdvanceTimer::begin/end
 * Data stored/computed will be step name, its time in ms and the corresponding % inside the whole step.
 * the total ms and percentage it represent if this step has substeps.
 * Buffer of AnimationSubStepData corresponding to its children substeps
 */
class AnimationStepData
{
public:
    AnimationStepData(int level, std::string name, ctime_t start);
    AnimationStepData(int step, const std::string& idString);
    virtual ~AnimationStepData();

    int m_stepIteration;
    int m_level;
    std::string m_name;
    int m_nbrCall;
    ctime_t m_start;
    ctime_t m_end;

    std::string m_tag;
    double m_totalMs;
    double m_totalPercent;
    double m_selfMs;
    double m_selfPercent;
    AnimationStepData* m_parent;
    void computeTimeAndPercentage(double invTotalMs);

    sofa::helper::vector<AnimationStepData*> m_substeps;
};

}  // namespace sofaqtquick

#endif // PROFILERTREEVIEW_H
