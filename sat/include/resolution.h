// resolution.h
// Strategy pattern implemented for the main resolution algorithm

#include <iostream>
#include "clauses.h"

// generic resolution algorithm structure, abstract class, Strategy pattern
class resolution_algorithm
{
    private:
        // sets of processes and unprocessed clauses used in the underlying
        // given clause algorithm
        clause_set_t processed;
        clause_set_t unprocessed;
        // helper method, implements propositional resolution
        clause_t resolve(clause_t&, clause_t&, literal_t&);
    public:
        // constructor, takes initial set of unprocessed clauses
        resolution_algorithm(clause_set_t&);
        // destructor
        virtual ~resolution_algorithm();
        // main proof method, same for every algorithm
        int prove(void);
        // generating a set of new clauses from the set of processed clauses
        // and a selected given clause, same for every algorithm
        void generate(clause_t&);
        // abstract method for given clause selection
        virtual clause_t choose_clause(void) = 0;
        // abstract method for clause set rejection
        virtual bool should_reject(void) = 0;
        // accessors of the pointers to the clause sets
        clause_set_t* get_processed(void) { return &processed; }
        clause_set_t* get_unprocessed(void) { return &unprocessed; }
};

// heuristic H1: always choose first clause, never reject
class res_h1 : public resolution_algorithm
{
    public:
        res_h1(clause_set_t&);
        virtual clause_t choose_clause(void);
        virtual bool should_reject(void);
};

// heuristic H2: choose clause at random, reject after too many steps
class res_h2 : public resolution_algorithm
{
    private:
        int steps_taken;
        int steps_limit;
    public:
        res_h2(clause_set_t&, int);
        virtual clause_t choose_clause(void);
        virtual bool should_reject(void);
};

// heuristic H3: choose shortest clause, break ties at random, reject after
// too many steps
class res_h3 : public resolution_algorithm
{
    private:
        int steps_taken;
        int steps_limit;
    public:
        res_h3(clause_set_t&, int);
        virtual clause_t choose_clause(void);
        virtual bool should_reject(void);
};
