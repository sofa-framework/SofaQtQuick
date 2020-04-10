#include "ProfilerTreeView.h"

namespace sofaqtquick {

// quick method to convert freq time into ms
double convertInMs(ctime_t t, int nbIter=1)
{
    static double timer_freqd = double(CTime::getTicksPerSec());
    return 1000.0 * double(t) / double (timer_freqd * nbIter);
}

AnimationStepData::AnimationStepData(int level, std::string name, ctime_t start)
    : m_level(level)
    , m_name(name)
    , m_nbrCall(1)
    , m_start(start)
    , m_parent(nullptr)
{
}

AnimationStepData::AnimationStepData(int step, const std::string& idString)
    : m_stepIteration(step),
      m_level(0),
      m_name(idString),
      m_parent(nullptr)
{
    sofa::helper::vector<Record> _records = sofa::helper::AdvancedTimer::getRecords(idString);
    std::stack<AnimationStepData*> processStack;
    int level = 0;
    ctime_t t0 = 0;
    ctime_t tEnd = CTime::getTime();
    ctime_t tCurr;
    for (unsigned int ri = 0; ri < _records.size(); ++ri)
    {
        const Record& rec = _records[ri];

        if (level == 0) // main step
        {
            t0 = rec.time;
            level++;
            continue;
        }

        tCurr = rec.time - t0;

        if (rec.type == Record::RBEGIN || rec.type == Record::RSTEP_BEGIN || rec.type == Record::RSTEP)
        {
            AnimationStepData* currentSubStep = new AnimationStepData(level, rec.label, tCurr);
            if (rec.obj)
                currentSubStep->m_tag = std::string(AdvancedTimer::IdObj(rec.obj));

            if (level == 1) // Add top level step
            {
                currentSubStep->m_parent = this;
                m_substeps.push_back(currentSubStep);
            }
            else
            {
                if (processStack.empty())
                {
                    msg_error("SofaWindowProfiler") << "No parent found to add step: " << currentSubStep->m_name;
                    delete currentSubStep;
                    break;
                }
                else if (processStack.top()->m_level + 1 != currentSubStep->m_level)
                {
                    msg_warning("SofaWindowProfiler") << "Problem of level coherence between step: " << currentSubStep->m_name << " with level: " << currentSubStep->m_level
                                                      << " and parent step: " << processStack.top()->m_name << " with level: " << processStack.top()->m_level;
                }

                // add next step to the hierarchy
                currentSubStep->m_parent = processStack.top();
                processStack.top()->m_substeps.push_back(currentSubStep);
            }

            // add step into the stack for parent/child order
            processStack.push(currentSubStep);
            ++level;
        }

        if (rec.type == Record::REND || rec.type == Record::RSTEP_END)
        {
            --level;

            if (processStack.empty())
            {
                msg_error("SofaWindowProfiler") << "End step with no step in the stack for: " << rec.label;
                break;
            }
            else if (rec.label != processStack.top()->m_name)
            {
                msg_error("SofaWindowProfiler") << "Not the same name to end step between logs: " << rec.label << " and top stack: " << processStack.top()->m_name;
                break;
            }

            AnimationStepData* currentSubStep = processStack.top();
            processStack.pop();

            currentSubStep->m_end = tCurr;
        }
    }
    // compute total MS step:
    m_totalMs = convertInMs(tEnd - t0);

    // update percentage
    SReal invTotalMs = 100 / m_totalMs;
    for (unsigned int i = 0 ; i < m_substeps.size() ; i++)
    {
        m_substeps[i]->computeTimeAndPercentage(invTotalMs);
    }
}

AnimationStepData::~AnimationStepData()
{
    for (unsigned int i=0; i<m_substeps.size(); ++i)
        delete m_substeps[i];

    m_substeps.clear();
}

void AnimationStepData::computeTimeAndPercentage(SReal invTotalMs)
{
    if (!m_substeps.empty()) // compute from leaf to trunk
    {
        SReal totalChildrenMs = 0.0;
        for (unsigned int i=0; i<m_substeps.size(); i++)
        {
            m_substeps[i]->computeTimeAndPercentage(invTotalMs);
            totalChildrenMs += m_substeps[i]->m_totalMs;
        }

        // now that all children are update, compute ms and %
        m_totalMs = convertInMs(m_end - m_start);
        m_selfMs = m_totalMs - totalChildrenMs;

        m_selfPercent = m_selfMs * invTotalMs;
        m_totalPercent = m_totalMs * invTotalMs;
    }
    else // leaf
    {
        // compute ms:
        m_totalMs = convertInMs(m_end - m_start);
        if (m_nbrCall != 1)
            m_selfMs = SReal(m_totalMs / m_nbrCall);
        else
            m_selfMs = m_totalMs;

        // compute %
        m_selfPercent = m_selfMs * invTotalMs;
        m_totalPercent = m_totalMs * invTotalMs;
    }
}

}  // namespace sofaqtquick
