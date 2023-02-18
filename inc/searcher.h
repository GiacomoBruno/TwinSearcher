#pragma once

#include "benchmark.h"
#include "globals.h"
#include "searcher_configuration.h"
#include "splitfile.h"

#include <atomic>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <optional>
#include <thread>
#include <variant>

using namespace globals;

static constexpr size_t CHUNK_SIZE = 500;

set           S{};
configuration conf{};

#define FORWARD_CALCS(x, y)                                                                                                                          \
    do                                                                                                                                               \
    {                                                                                                                                                \
        m1    = xp1 * (y);                                                                                                                           \
        delta = (y) - (x);                                                                                                                           \
        d     = m1 % delta;                                                                                                                          \
    }                                                                                                                                                \
    while (false)

#define BACKWARD_CALCS(x, z)                                                                                                                         \
    do                                                                                                                                               \
    {                                                                                                                                                \
        m1 = (z) + 1;                                                                                                                                \
        m1 *= (x);                                                                                                                                   \
        delta = (x) - (z);                                                                                                                           \
        d     = m1 % delta;                                                                                                                          \
    }                                                                                                                                                \
    while (false)

struct generator_object
{
    generator_object() = default;

    template <config::OPTIMIZATION_LEVELS O>
    void generate_twins();

    template <config::OPTIMIZATION_LEVELS O>
    void generate_twins_forward_only();

    bool has_work() const
    {
        return elements_to_work_on > 0;
    }

    std::set<integer> results{};
    integer           d{};
    integer           delta{};
    integer           m1{};
    work_set_iterator current_element{};
    size_t            elements_to_work_on{};
};

// NO OPTIMIZATION

template <>
void generator_object::generate_twins<config::OPTIMIZATION_LEVELS::NO_OPTIMIZATION>()
{
#ifdef STANDARD_SET
    auto x = *current_element;
#else
    auto x = S.find(*current_element);
#endif

    integer xp1 = *x + 1;

    for (auto iter = std::next(x); iter != S.end(); ++iter)
    {
        auto y = *iter;
        FORWARD_CALCS(*x, y);

        if (d == 0)
        {
            integer potential_twin = (m1 / delta) - 1;
            if (!CONTAINS(S, potential_twin))
            {
                results.insert(potential_twin);
            }
        }
    }

    if (x == S.begin())
        goto end;

    BACKWARD_CALCS(*x, *S.begin());

    if (d == 0)
    {
        integer potential_twin = (m1 / delta) - 1;
        if (!CONTAINS(S, potential_twin))
        {
            results.insert(potential_twin);
        }
    }

    for (auto iter = std::prev(x); iter != S.begin(); --iter)
    {
        BACKWARD_CALCS(*x, *iter);

        if (d == 0)
        {
            integer potential_twin = (m1 / delta) - 1;
            if (!CONTAINS(S, potential_twin))
            {
                results.insert(potential_twin);
            }
        }
    }
end:
    current_element++;
    elements_to_work_on--;
}

template <>
void generator_object::generate_twins_forward_only<config::OPTIMIZATION_LEVELS::NO_OPTIMIZATION>()
{
#ifdef STANDARD_SET
    auto x = *current_element;
#else
    auto x = S.find(*current_element);
#endif
    globals::integer xp1 = *x + 1;
    for (auto iter = std::next(x); iter != S.end(); ++iter)
    {
        auto y = *iter;
        FORWARD_CALCS(*x, y);

        if (d == 0)
        {
            globals::integer potential_twin = (m1 / delta) - 1;
            if (!CONTAINS(S, potential_twin))
            {
                results.insert(potential_twin);
            }
        }
    }

    current_element++;
    elements_to_work_on--;
}

// CONSTANT RANGE OPTIMIZATION
template <>
void generator_object::generate_twins<config::OPTIMIZATION_LEVELS::CONSTANT_RANGE_OPTIMIZATION>()
{
#ifdef STANDARD_SET
    auto x = *current_element;
#else
    auto x = S.find(*current_element);
#endif
    auto            range = std::get<constant_range_settings>(conf).range;
    decltype(range) i     = 0;
    integer         xp1   = *x + 1;

    for (auto iter = std::next(x); i < range && iter != S.end(); ++iter, ++i)
    {
        auto y = *iter;
        FORWARD_CALCS(*x, y);

        if (d == 0)
        {
            integer potential_twin = (m1 / delta) - 1;
            if (!CONTAINS(S, potential_twin))
            {
                results.insert(potential_twin);
            }
        }
    }

    if (x == S.begin())
        goto end;

    BACKWARD_CALCS(*x, *S.begin());

    if (d == 0)
    {
        integer potential_twin = (m1 / delta) - 1;
        if (!CONTAINS(S, potential_twin))
        {
            results.insert(potential_twin);
        }
    }
    i = 0;
    for (auto iter = std::prev(x); i < range && iter != S.begin(); --iter, ++i)
    {
        BACKWARD_CALCS(*x, *iter);

        if (d == 0)
        {
            integer potential_twin = (m1 / delta) - 1;
            if (!CONTAINS(S, potential_twin))
            {
                results.insert(potential_twin);
            }
        }
    }
end:
    current_element++;
    elements_to_work_on--;
}

template <>
void generator_object::generate_twins_forward_only<config::OPTIMIZATION_LEVELS::CONSTANT_RANGE_OPTIMIZATION>()
{
#ifdef STANDARD_SET
    auto x = *current_element;
#else
    auto x = S.find(*current_element);
#endif
    auto             range = std::get<constant_range_settings>(conf).range;
    decltype(range)  i     = 0;
    globals::integer xp1   = *x + 1;
    for (auto iter = std::next(x); i < range && iter != S.end(); ++iter, ++i)
    {
        auto y = *iter;
        FORWARD_CALCS(*x, y);

        if (d == 0)
        {
            globals::integer potential_twin = (m1 / delta) - 1;
            if (!CONTAINS(S, potential_twin))
            {
                results.insert(potential_twin);
            }
        }
    }

    current_element++;
    elements_to_work_on--;
}

// GENERATE FUNCTION
template <config::OPTIMIZATION_LEVELS O>
void generate()
{
    wset work_set{};

    std::cout << "SMOOTHNESS: " << CONFIG.Smoothness << std::endl;
    std::cout << "THREAD COUNT: " << CONFIG.ThreadPool.get_thread_count() << std::endl;
    std::cout << "\nEXECUTING: " << GetOptimizationLevel(O) << std::endl;

    table_header_string();

    benchmark btot{};
    btot.start_bench();

    // INSERT INITIAL ELEMENTS
    for (int i = 0; i < CONFIG.Smoothness; i++)
    {
        auto inserted = S.insert(integer(i + 1));
#ifdef STANDARD_SET
        work_set.push_back(inserted.first);
#else
        work_set.push_back(i + 1);
#endif
    }

    // ACTUAL ALGORITHM
    while (!work_set.empty())
    {
        benchmark b;
        b.start_bench();

        conf                 = load_configuration<O>(S.size());
        size_t current_chunk = 0;

        auto              total_chunks = (work_set.size() / CHUNK_SIZE) + ((work_set.size() % CHUNK_SIZE) != 0);
        generator_object *generators   = new generator_object[total_chunks]{};

        for (decltype(total_chunks) i = 0; i < total_chunks; i++)
        {
            CONFIG.ThreadPool.push_task(
                [total_chunks, &generators, &work_set](decltype(i) index)
                {
                    generators[index].current_element     = std::next(work_set.begin(), index * CHUNK_SIZE);
                    generators[index].elements_to_work_on = index != (total_chunks - 1) ? CHUNK_SIZE : work_set.size() % CHUNK_SIZE;
                },
                i);
        }
        CONFIG.ThreadPool.wait_for_tasks();

        for (size_t i = 0; i < total_chunks; i++)
        {
            CONFIG.ThreadPool.push_task(
                [&generators](size_t i)
                {
                    while (generators[i].has_work())
                        generators[i].generate_twins<O>();
                },
                i);
        }

        CONFIG.ThreadPool.wait_for_tasks();
        work_set.clear();
        size_t count_newly_found = 0;
        for (int i = 0; i < total_chunks; i++)
        {
            for (auto &x : generators[i].results)
            {
                if (auto insertion = S.insert(x); insertion.second)
                {
                    count_newly_found++;
#ifdef STANDARD_SET
                    work_set.push_back(insertion.first);
#else
                    work_set.push_back(x);
#endif
                }
            }
        }

        delete[] generators;

        b.conclude_bench();

        table_line_string(count_newly_found, b.seconds_passed(), S.size());
    }

    btot.conclude_bench();
    std::cout << "FOUND: " << S.size() << std::endl;
    std::cout << "Total time taken: " << btot.seconds_passed() << " seconds" << std::endl;
}