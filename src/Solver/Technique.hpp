#ifndef __TECHNIQUE_H__
#define __TECHNIQUE_H__

class Solver;

class Technique
{
  protected:
    Solver& m_solver;

    virtual bool analyze() = 0;
    virtual bool perform() = 0;

  public:
    Technique(Solver& solver)
        : m_solver(solver)
    {}
    virtual ~Technique() = default;

    // TODO: improve return type
    bool run() { return analyze() ? perform() : false; }
};

#define NEW_TECHNIQUE(name)                                                                        \
    class name : public Technique                                                                  \
    {                                                                                              \
      protected:                                                                                   \
        bool analyze() override;                                                                   \
        bool perform() override;                                                                   \
                                                                                                   \
      public:                                                                                      \
        name(Solver& solver)                                                                       \
            : Technique(solver)                                                                    \
        {}                                                                                         \
        ~name() = default;                                                                         \
    }

#endif // __TECHNIQUE_H__