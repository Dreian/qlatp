// resolution.cpp
// Implementation of the given clause algorithm and the most important helper
// methods. Also present are implementations of basic heuristics.

#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include <utility>
#include <vector>
#include "resolution.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#define debug_write(obj) \
    if (DEBUG) { \
        std::cout << obj; }

// A debugging method for pretty-printing a clause
void print_clause(clause_t clause)
{
    if (DEBUG) {
        debug_write("{ ");
        for (literal_t lit : clause) {
            if (!std::get<1>(lit)) {
                debug_write("-");
            }
            debug_write(std::get<0>(lit));
            debug_write(" ");
        }
        debug_write("} ");
    }
}

// generate random number
double gen_rand(double low, double high)
{
    double r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    return low + r * (high - low);
}

qfun_est(state_feature_cnt + action_feature_cnt, hidden_neurons_cnt, 1,
         learn_rate, learn_iter_cnt);

// Q-learning constructor
// adds nothing
res_qlearn::res_qlearn(clause_set_t& clauses, int steps, double lambda_choose,
                       double reward_proof)
    : resolution_algorithm(clauses)
{
    debug_write("qlearn used\n");
    steps_limit = steps;
    lambda = lambda_choose;
    reward = reward_proof;
    previously_took = false;
}

// qlearn method of choosing the clause
clause_t res_qlearn::choose_clause(void)
{
    double p_total = 0.0;
    double qfun_max = 0.0;
    clause_set_t::iterator pr_it = (*get_processed()).begin();
    clause_set_t::iterator unpr_it = (*get_unprocessed()).begin();
    clause_t::iterator clause_it;
    std::vector<double> inputs(state_feature_cnt + action_feature_cnt, 0.0);
    std::vector<double> p_result((*get_unprocessed()).size(), 0.0);
    // TODO: unprocessed set features
    // F0: average processed clause length
    // F1: proportion of unit clauses
    double avg_length = 0.0;
    double unit_prop = 0.0;
    for (; pr_it != (*get_processed()).end(); pr_it++) {
        avg_length += (*pr_it).size();
        unit_prop += ((*pr_it).size() == 1);
    }
    avg_length /= (*get_processed()).size();
    inputs[0] = avg_length;
    unit_prop /= (*get_processed()).size();
    inputs[1] = unit_prop;
    for (int i = 0; unpr_it != (*get_unprocessed()).end(); unpr_it++, i++) {
        // TODO: clause features
        // clause length
        inputs[state_feature_cnt + 0] = (*unpr_it).size();
/*        // ...?
        inputs[state_feature_cnt + 1] = 0.0;
        // ...?
        inputs[state_feature_cnt + 2] = 0.0;*/
        // distribution
        double qfun_res = qfun_est.feed_forward(inputs)[0];
        if (qfun_max < qfun_res) {
            qfun_max = qfun_res;
        }
        p_result[i] = pow(lambda, qfun_res);
        p_total += p_result[i];
    }
    if (previously_took) {
        out_batch.back() += ql_learn_rate * discount_factor * qfun_max;
    }
    double r = gen_rand(0.0, p_total);
    double p_sofar = 0.0;
    int cl_idx = 0;
    for (int i = 0; cl_idx < (*get_unprocessed()).end(); cl_idx++, i++) {
        p_sofar += p_result[i];
        if (p_sofar >= r) {
            break;
        }
    }
    steps_taken++;
    // possibly add sample to batch 
    if (gen_rand(0.0, 1.0) < prob_take) {
        previously_took = true;
        in_batch.push_back(inputs);
        if ((*get_unprocessed())[cl_idx].empty()) {
            out_batch.push_back(
                (1.0 - ql_learn_rate) * qfun_est.feed_forward(inputs)[0] +
                ql_learn_rate * reward);
        } else {
            out_batch.push_back(
                (1.0 - ql_learn_rate) * qfun_est.feed_forward(inputs)[0]);
        }
    } else {
        previously_took = false;
    }
    return (*get_unprocessed())[cl_idx];
}

// qlearn method of rejecting a set of clauses
// only if set of unprocessed clauses is empty or too many steps taken
bool res_qlearn::should_reject(void)
{
    return (*get_unprocessed()).empty() || steps_taken == steps_limit;
}

