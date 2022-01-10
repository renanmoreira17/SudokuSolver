#ifndef __SINGLESREGION_H__
#define __SINGLESREGION_H__

#include "Solver/Technique.hpp"

class SinglesRegion : public Technique
{
  protected:
    bool analyze() override;
    bool perform() override;

  public:
    SinglesRegion(Solver& solver)
        : Technique(solver)
    {}
    ~SinglesRegion() = default;
};

#endif // __SINGLESREGION_H__