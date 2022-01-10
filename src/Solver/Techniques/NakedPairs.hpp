#ifndef __NAKEDPAIRS_H__
#define __NAKEDPAIRS_H__

#include "Solver/Technique.hpp"

class NakedPairs : public Technique
{
  protected:
    bool analyze() override;
    bool perform() override;

  public:
    NakedPairs(Solver& solver)
        : Technique(solver)
    {}
    ~NakedPairs() {}
};

#endif // __NAKEDPAIRS_H__