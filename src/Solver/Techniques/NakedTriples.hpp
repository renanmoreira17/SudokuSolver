#ifndef __NAKEDTRIPLES_H__
#define __NAKEDTRIPLES_H__

#include "Solver/Technique.hpp"

class NakedTriples : public Technique
{
  protected:
    bool analyze() override;
    bool perform() override;

  public:
    NakedTriples(Solver& solver)
        : Technique(solver)
    {}
    ~NakedTriples() {}
};

#endif // __NAKEDTRIPLES_H__