#pragma once


#include <gmpxx.h>
#include <set>
#include <vector>
#include <memory>
//#include "tlx/container/btree_set.hpp"
#include "config.h"
#include "output.h"
#include "allocator.h"

#define CONTAINS(_s_, _x_) (_s_.find(_x_) != _s_.end())

namespace globals
{
    using integer = mpz_class;
    using floating = mpf_class;
    std::string GetOptimizationLevel(config::OPTIMIZATION_LEVELS level)
    {
        using O = config::OPTIMIZATION_LEVELS;
        switch(level)
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
        static const bool self_verify = false;
        static const bool debug = false;
        static const int leaf_slots = 12800 / (sizeof(Value));
        static const int inner_slots = 12800 / (sizeof(Key) + sizeof(void*));
        static const size_t binsearch_threshold = 256;
    };
    
    //using set = tlx::btree_set<number>;
    using set = std::set<integer, std::less<integer>, Allocator<integer, 1024>>;
    using wset = std::vector<typename set::const_iterator>;
}