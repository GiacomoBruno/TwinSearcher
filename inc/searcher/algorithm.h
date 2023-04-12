#pragma once
#include "../stopwatch.h"
#include "generator.h"
#include "global_variables.h"

namespace searcher
{

template <optimization O>
void algorithm()
{
    auto &gb = global_variables::instance();

    wset work_set{};
    std::cout << "SMOOTHNESS: " << CONFIG.Smoothness << std::endl;
    std::cout << "THREAD COUNT: " << CONFIG.ThreadPool.get_thread_count() << std::endl;
    std::cout << "\nEXECUTING: " << GetOptimizationLevel(O) << std::endl;
    static constexpr size_t CHUNK_SIZE = 500;
    table_header_string();

    stopwatch btot{};
    btot.start();

    for (int x = 0; x < CONFIG.Smoothness; x++)
    {
        auto inserted = gb.S.insert(integer(x + 1));
        INSERT_NEW_TWIN();
    }

    while (!work_set.empty())
    {
        stopwatch b;
        b.start();

        gb.conf = load_configuration<O>(gb.S.size());

        unsigned long   total_chunks = (work_set.size() / CHUNK_SIZE) + ((work_set.size() % CHUNK_SIZE) != 0);
        twin_generator *generators   = new twin_generator[total_chunks]{};

        for (unsigned long i = 0; i < total_chunks; i++)
        {
            CONFIG.ThreadPool.push_task(
                [total_chunks, &generators, &work_set](decltype(i) i) noexcept
                {
                    generators[i].current_element     = std::next(work_set.begin(), i * CHUNK_SIZE);
                    generators[i].elements_to_work_on = i != (total_chunks - 1) ? CHUNK_SIZE : work_set.size() % CHUNK_SIZE;
                    while (generators[i].has_work())
                        generators[i].generate<O>();
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
                if (auto insertion = gb.S.insert(x); insertion.second)
                {
                    count_newly_found++;
                    INSERT_NEW_TWIN();
                }
            }
        }

        delete[] generators;
        b.pause();
        table_line_string(count_newly_found, b.elapsed<std::chrono::seconds>(), gb.S.size());
    }

    btot.pause();
    std::cout << "FOUND: " << gb.S.size() << std::endl;
    std::cout << "Total time taken: " << btot.elapsed<std::chrono::seconds>() << " seconds" << std::endl;
}

} // namespace searcher
