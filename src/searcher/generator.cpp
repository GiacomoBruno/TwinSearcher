#include "searcher/generator.h"

namespace searcher
{
using optimization = config::OPTIMIZATION_LEVELS;
using namespace globals;

extern configuration conf;

template <>
void twin_generator::generate_forward<optimization::NO_OPTIMIZATION>(x_type &x) noexcept
{
    integer xp1 = *x + 1;
    auto   &S   = gb::instance().S;
    for (auto iter = std::next(x); iter != S.end(); ++iter)
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
void twin_generator::generate_backward<optimization::NO_OPTIMIZATION>(x_type &x) noexcept
{
    auto &S = gb::instance().S;
    for (auto iter = std::prev(x); iter != S.begin(); --iter)
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