#pragma once

#include "../config.h"
#include "../output.h"
#include "allocator.h"

#include <btree/fc_btree.h>
#include <gmpxx.h>
#include <memory>
#include <set>
#include <tlx/btree_set.hpp>
#include <vector>

#define CONTAINS(_s_, _x_) (_s_.find(_x_) != _s_.end())

namespace globals
{
using integer  = mpz_class;
using floating = mpf_class;

inline std::string GetOptimizationLevel(config::OPTIMIZATION_LEVELS level)
{
    using O = config::OPTIMIZATION_LEVELS;
    switch (level)
    {
    case O::NO_OPTIMIZATION:
        return "No optimizations";
    case O::CONSTANT_RANGE_OPTIMIZATION:
        return std::string("Constant range: ") + std::to_string(CONFIG.RangeCurrent);
    case O::GLOBAL_K_OPTIMIZATION:
        return std::string("Global K: ") + std::to_string(CONFIG.KCurrent);
    case O::VARIABLE_RANGE_OPTIMIZATION:
        return "Variable range";
    }
    return "";
}

template <typename Key, typename Value>
struct optimized_btree_traits
{
    static const bool   self_verify         = false;
    static const bool   debug               = false;
    static const int    leaf_slots          = 12800 / (sizeof(Value));
    static const int    inner_slots         = 12800 / (sizeof(Key) + sizeof(void *));
    static const size_t binsearch_threshold = 256;
};

struct compare
{
    int operator()(const integer &l, const integer &r) const
    {
        return mpz_cmp(l.get_mpz_t(), r.get_mpz_t());
    }
};

#define STL_TREE 0
#define TLX_TREE 1
#define FC_TREE  2

#define TREE_STANDARD 0
#define TREE_TLX      1
#define TREE_FC       2

#define TREE_TYPE TREE_TLX

#if TREE_TYPE == STL_TREE
using set    = std::set<integer>;
using wset   = std::vector<typename set::const_iterator>;
using x_type = set::const_iterator;
#    define GET_X(ce) ce

#elif TREE_TYPE == TLX_TREE

using set    = tlx::btree_set<integer, compare, optimized_btree_traits<integer, integer> >;
using wset   = std::vector<integer>;
using x_type = typename globals::set::const_iterator;
#    define GET_X(ce) gb::instance().S.find(*ce)
#elif TREE_TYPE == FC_TREE

using set    = frozenca::BTreeSet<integer>;
using wset   = std::vector<integer>;
using x_type = typename globals::set::const_iterator;
#    define GET_X(ce) gb::instance().S.find(ce)

#endif

using iterator          = typename globals::set::const_iterator;
using work_set_iterator = typename globals::wset::const_iterator;

} // namespace globals