#pragma once
#include "../config.h"

#include <cstddef>
#include <variant>

struct no_settings
{
};

struct constant_range_settings
{
    size_t range;
};

struct global_k_settings
{
    double starting_k;
    double optimized_k;
    double current_k;
};

using configuration = std::variant<no_settings, constant_range_settings, global_k_settings>;

template <config::OPTIMIZATION_LEVELS O>
configuration load_configuration(size_t size)
{
    if constexpr (O == config::OPTIMIZATION_LEVELS::NO_OPTIMIZATION)
    {
        return no_settings{};
    }
    else if constexpr (O == config::OPTIMIZATION_LEVELS::CONSTANT_RANGE)
    {
        return constant_range_settings{ static_cast<size_t>(CONFIG.RangeCurrent) };
    }
    else if constexpr (O == config::OPTIMIZATION_LEVELS::GLOBAL_K)
    {
        if (size > 100000)
        {
            return global_k_settings{ 2.0, CONFIG.KCurrent, CONFIG.KCurrent };
        }
        else
        {
            return global_k_settings{ 2.0, CONFIG.KCurrent, 2.0 };
        }
    }
    else
    {
        return {};
    }
}
