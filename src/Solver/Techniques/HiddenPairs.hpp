#ifndef __HIDDENPAIRS_H__
#define __HIDDENPAIRS_H__

#include "Solver/Technique.hpp"

class HiddenPairs : public Technique
{
  protected:
    bool analyze() override;
    bool perform() override;

  public:
    HiddenPairs(Solver& solver)
        : Technique(solver)
    {}
    ~HiddenPairs() {}
};

#endif // __HIDDENPAIRS_H__