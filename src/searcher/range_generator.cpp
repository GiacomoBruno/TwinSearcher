#include "searcher/generator.h"

namespace searcher
{
using optimization = config::OPTIMIZATION_LEVELS;
using namespace globals;

template <>
void twin_generator::generate_forward<optimization::CONSTANT_RANGE>(x_type &x) noexcept
{
    auto            range = std::get<constant_range_settings>(gb::instance().conf).range;
    decltype(range) i     = 0;
    integer         xp1   = *x + 1;
    auto           &S     = gb::instance().S;
    for (auto iter = std::next(x); i < range && iter != S.end(); ++iter, ++i)
    {
        auto y = *iter;
        m1     = xp1 * (y);
        delta  = (y) - (*x);
        d      = m1 % delta;

        if (d == 0)
        {
            integer potential_twin = (m1 / delta) - 1;
            if (S.find(potential_twin) == S.end())
            {
                results.insert(potential_twin);
            }
        }
    }
}

template <>
void twin_generator::generate_backward<optimization::CONSTANT_RANGE>(x_type &x) noexcept
{
    auto            range = std::get<constant_range_settings>(gb::instance().conf).range;
    decltype(range) i     = 0;
    auto           &S     = gb::instance().S;
    for (auto iter = std::prev(x); i < range && iter != S.begin(); --iter, ++i)
    {
        m1 = (*iter) + 1;
        m1 *= (*x);
        delta = (*x) - (*iter);
        d     = m1 % delta;
        if (d == 0)
        {
            integer potential_twin = (m1 / delta) - 1;
            if (S.find(potential_twin) == S.end())
            {
                results.insert(potential_twin);
            }
        }
    }
}

} // namespace searcher
