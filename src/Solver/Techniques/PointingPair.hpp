#ifndef __POINTINGPAIR_H__
#define __POINTINGPAIR_H__

#include "Solver/Technique.hpp"

class PointingPair : public Technique
{
  protected:
    bool analyze() override;
    bool perform() override;

  public:
    PointingPair(Solver& solver)
        : Technique(solver)
    {}
    ~PointingPair() = default;
};

#endif // __POINTINGPAIR_H__