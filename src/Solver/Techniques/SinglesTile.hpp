#ifndef __SINGLESTILE_H__
#define __SINGLESTILE_H__

#include "Solver/Technique.hpp"

class SinglesTile : public Technique
{
  protected:
    bool analyze() override;
    bool perform() override;

  public:
    SinglesTile(Solver& solver)
        : Technique(solver)
    {}
    ~SinglesTile() = default;
};

#endif // __SINGLESTILE_H__