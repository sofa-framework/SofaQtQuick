#pragma once

#include <SofaQtQuickGUI/config.h>
#include <SofaQtQuickGUI/ProfilerTreeView.h>

#include <sofa/helper/AdvancedTimer.h>
#include <sofa/helper/system/thread/CTime.h>

#include <QAbstractItemModel>

#include <deque>

namespace sofaqtquick {
namespace _profilertreeviewmodel_ {

class ProfilerTreeViewModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    ProfilerTreeViewModel(QObject* parent = nullptr);
    ~ProfilerTreeViewModel() override;


    Q_PROPERTY(int bufferSize READ getBufferSize WRITE setBufferSize NOTIFY bufferSizeChanged)

    Q_INVOKABLE void seek(int stepNumber);
    Q_INVOKABLE void clearBuffer();
    Q_INVOKABLE QVariant recordStep(int step, const QString& idString = "Animate");
    Q_INVOKABLE void activateTimer(bool activate, const QString& idString = "Animate");

    int  getBufferSize() { return int(m_bufferSize); }
    void setBufferSize(int bufSize) { m_bufferSize = size_t(bufSize); }

signals:
    void bufferSizeChanged(int);

private:
    size_t m_bufferSize;
    std::deque<AnimationStepData*> m_steps;
    int m_stepNumber;
    ///////////////////////////// AbstractItemModel ///////////////////////////////////
public:
    QModelIndex index(AnimationStepData* stepData);

    /// The following methods are inherited from QAbstractItemModel.
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const override ;
    virtual QModelIndex parent(const QModelIndex &index) const override ;
    virtual bool hasChildren(const QModelIndex &index) const override;
    virtual int rowCount(const QModelIndex &parent) const override ;
    virtual int columnCount(const QModelIndex &parent) const override ;
    virtual QVariant data(const QModelIndex &index, int role) const override ;
    virtual QHash<int, QByteArray> roleNames() const override;

    enum class Roles
    {
        Name = Qt::UserRole + 1,
        Step, // AdvancedTimer idString
        Label, // AdvancedTimer idString
        Level, // substep's level
        Icon,  // maybe one day a matching pictogram for the idString
        TotalPerc,
        TotalMs,
        SelfPerc,
        SelfMs
    };
    ///////////////////////////////////////////////////////////////////////////////////
};

}  // namespace _profilertreeviewmodel_
    using _profilertreeviewmodel_::ProfilerTreeViewModel;
}  // namespace sofaqtquick

