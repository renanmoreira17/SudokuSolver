#ifndef __TECHNIQUE_H__
#define __TECHNIQUE_H__

#include <string>

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
    bool run()
    {
        const bool shouldProceed = analyze();
        if (!shouldProceed)
        {
            return false;
        }
        const bool performed = perform();
        if (!performed)
        {
            return false;
        }

        ++m_ranCount;
        return true;
    }

    virtual std::string getTechniqueName() const = 0;

    unsigned int getRanCount() const { return m_ranCount; }

  private:
    unsigned int m_ranCount{0};
};

#define NEW_TECHNIQUE(name)                                                                                  \
    class name : public Technique                                                                            \
    {                                                                                                        \
      protected:                                                                                             \
        bool analyze() override;                                                                             \
        bool perform() override;                                                                             \
                                                                                                             \
      public:                                                                                                \
        name(Solver& solver)                                                                                 \
            : Technique(solver)                                                                              \
        {}                                                                                                   \
        ~name() = default;                                                                                   \
                                                                                                             \
        std::string getTechniqueName() const override { return #name; }                                      \
    }

#endif // __TECHNIQUE_H__