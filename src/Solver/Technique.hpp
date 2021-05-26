#ifndef __TECHNIQUE_H__
#define __TECHNIQUE_H__

class Solver;

class Technique
{
protected:
    Solver* m_solver;

    virtual bool analyze() = 0;
    virtual bool perform() = 0;
public:
    Technique(Solver* solver)
    : m_solver(solver) {}
    Technique() = default;
    virtual ~Technique() = default;

    // TODO: improve return type
    bool run()
    {
        return analyze() ? perform() : false;
    }

};

#endif // __TECHNIQUE_H__