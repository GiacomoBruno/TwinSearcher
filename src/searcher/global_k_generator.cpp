#include "searcher/generator.h"

namespace searcher
{
using optimization = config::OPTIMIZATION_LEVELS;
using namespace globals;

#define GET_MAX_BY_K()                                                                                                                               \
    do                                                                                                                                               \
    {                                                                                                                                                \
        if (!chunk.empty())                                                                                                                          \
        {                                                                                                                                            \
            {                                                                                                                                        \
                mpf_class f{ *chunk[0] };                                                                                                            \
                f *= GLOBALS.KCurrent;                                                                                                               \
                max_y = f;                                                                                                                           \
            }                                                                                                                                        \
            {                                                                                                                                        \
                mpf_class f{ *chunk[0] };                                                                                                            \
                f *= (2 - GLOBALS.KCurrent);                                                                                                         \
                max_z = f;                                                                                                                           \
            }                                                                                                                                        \
        }                                                                                                                                            \
        else                                                                                                                                         \
            return;                                                                                                                                  \
    }                                                                                                                                                \
    while (false)

template <>
void twin_generator::generate_forward<optimization::GLOBAL_K>(x_type &x) noexcept
{
    mpz_class max = std::get<global_k_settings>(gb::instance().conf).current_k * *x;
    integer         xp1 = *x + 1;
    auto           &S   = gb::instance().S;
    auto max_iter = S.lower_bound(max);

    for (auto iter = std::next(x); iter != max_iter && iter != S.end(); ++iter)
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
void twin_generator::generate_backward<optimization::GLOBAL_K>(x_type &x) noexcept
{
    mpz_class max = (2.0 - std::get<global_k_settings>(gb::instance().conf).current_k) * *x;
    auto           &S     = gb::instance().S;
    auto max_iter = S.upper_bound(max);
    for (auto iter = std::prev(x); iter != max_iter && iter != S.begin(); --iter)
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
