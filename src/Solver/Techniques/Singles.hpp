#ifndef __SINGLES_H__
#define __SINGLES_H__

#include "Solver/Technique.hpp"

class Singles : public Technique
{
protected:
    /* data */
    bool analyze() override;
    bool perform() override;
public:
    Singles(Solver* solver): Technique(solver) {}
    Singles() = default;
    ~Singles() = default;
};

#endif // __SINGLES_H__