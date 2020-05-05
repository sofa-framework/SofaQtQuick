#pragma once

#include "Manipulator.h"

namespace sofaqtquick
{

class Snapping_Manipulator : public Manipulator
{
public:
    Snapping_Manipulator(QObject* parent = nullptr);
    virtual void internalDraw(const SofaViewer& viewer, int pickIndex, bool isPicking = false) override;
    virtual int getIndices() const override;
    virtual QString getDisplayText() const override;

};

}  // namespace sofaqtquick
