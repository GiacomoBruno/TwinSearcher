#pragma once

#include <optional>
#include <mutex>
#include <iostream>
#include <filesystem>
#include <thread>
#include <atomic>
#include <variant>
#include "globals.h"
#include "benchmark.h"
#include "splitfile.h"
using iterator = typename globals::set::const_iterator;

#define FORWARD_CALCS(x, y)                                                                         \
	do                                                                                              \
	{                                                                                               \
		m1 = xp1 * (y);                                                                             \
		delta = (y) - (x);                                                                          \
		d = m1 % delta;                                                                             \
	} while(false)

#define BACKWARD_CALCS(x, z)                                                                        \
	do                                                                                              \
	{                                                                                               \
		m1 = (z) + 1;                                                                               \
		m1 *= (x);                                                                                  \
		delta = (x) - (z);                                                                          \
		d = m1 % delta;                                                                             \
	} while(false)


//    NO_OPTIMIZATION = 0,
//    CONSTANT_RANGE_OPTIMIZATION = 1,
//    GLOBAL_K_OPTIMIZATION = 2,
//    VARIABLE_RANGE_OPTIMIZATION = 3
struct no_settings {};
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

struct arguments
{
    iterator        x;
    globals::set&   S;
    globals::set&   R;
    std::mutex&     M;
    configuration&  C;

    arguments(iterator _x, globals::set& s, globals::set& r, std::mutex& m, configuration& c) : x{_x}, S{s}, R{r}, M{m}, C{c} {} 
};
template<config::OPTIMIZATION_LEVELS O>
void generate_twins(arguments& args);

template<config::OPTIMIZATION_LEVELS O>
void generate_twins_forward_only(arguments& args);

template<>
void generate_twins<config::OPTIMIZATION_LEVELS::NO_OPTIMIZATION>(arguments& args)
{
    globals::integer d, delta, m1;
    auto [x, S, R, M, C] = args;
    globals::integer xp1 = *x + 1;

    for(auto iter = std::next(x); iter != S.end(); ++iter)
    {
        auto y = *iter;
        FORWARD_CALCS(*x, y);

        if(d == 0) [[unlikely]]
        {
            globals::integer potential_twin = (m1 / delta) - 1;
            if(!CONTAINS(S, potential_twin))
            {
                std::unique_lock l{M};
                R.insert(potential_twin);
            }
        }
    }

    if(x == S.begin()) return;

    BACKWARD_CALCS(*x, *S.begin());
    if(d == 0) [[unlikely]]
    {
        globals::integer potential_twin = (m1 / delta) - 1;
        if(!CONTAINS(S, potential_twin))
        {
            std::unique_lock l{M};
            R.insert(potential_twin);
        }
    }

    for(auto iter = std::prev(x); iter != S.begin(); --iter)
    {
        BACKWARD_CALCS(*x, *iter);

        if(d == 0) [[unlikely]]
        {
            globals::integer potential_twin = (m1 / delta) - 1;
            if(!CONTAINS(S, potential_twin))
            {
                std::unique_lock l{M};
                R.insert(potential_twin);
            }
        }
    }
    

        
    
}

template<>
void generate_twins_forward_only<config::OPTIMIZATION_LEVELS::NO_OPTIMIZATION>(arguments& args)
{
    globals::integer d, delta, m1;
    auto [x, S, R, M, C] = args;
    globals::integer xp1 = *x + 1;

    for(auto iter = std::next(x); iter != S.end(); ++iter)
    {
        auto y = *iter;
        FORWARD_CALCS(*x, y);

        if(d == 0) [[unlikely]]
        {
            globals::integer potential_twin = (m1 / delta) - 1;
            if(!CONTAINS(S, potential_twin))
            {
                std::unique_lock l{M};
                R.insert(potential_twin);
            }
        }
    }
}

template<>
void generate_twins<config::OPTIMIZATION_LEVELS::CONSTANT_RANGE_OPTIMIZATION>(arguments& args)
{
    globals::integer d, delta, m1;
    size_t counter = 0;
    auto [x, S, R, M, C] = args;
    size_t range = std::get<constant_range_settings>(C).range;

    globals::integer xp1 = *x + 1;
    for(auto iter = std::next(x); iter != S.end() && counter < range; ++iter)
    {
        counter++;
        auto y = *iter;
        FORWARD_CALCS(*x, y);

        if(d == 0) [[unlikely]]
        {
            globals::integer potential_twin = (m1 / delta) - 1;
            if(!CONTAINS(S, potential_twin))
            {
                std::unique_lock l{M};
                R.insert(potential_twin);
            }
        }
    }
    

    if(x == S.begin())  return;
        
    
    BACKWARD_CALCS(*x, *S.begin());
    if(d == 0) [[unlikely]]
    {
        globals::integer potential_twin = (m1 / delta) - 1;
        if(!CONTAINS(S, potential_twin))
        {
            std::unique_lock l{M};
            R.insert(potential_twin);
        }
    }

    counter = 0;
    for(auto iter = std::prev(x); iter != S.begin() && counter < range; --iter)
    {
        counter++;
        BACKWARD_CALCS(*x, *iter);

        if(d == 0) [[unlikely]]
        {
            globals::integer potential_twin = (m1 / delta) - 1;
            if(!CONTAINS(S, potential_twin))
            {
                std::unique_lock l{M};
                R.insert(potential_twin);
            }
        }
    }
    
}

template<>
void generate_twins_forward_only<config::OPTIMIZATION_LEVELS::CONSTANT_RANGE_OPTIMIZATION>(arguments& args)
{
    globals::integer d, delta, m1;
    size_t counter = 0;
    auto [x, S, R, M, C] = args;
    size_t range = std::get<constant_range_settings>(C).range;

    globals::integer xp1 = *x + 1;
    for(auto iter = std::next(x); iter != S.end() && counter < range; ++iter)
    {
        counter++;
        auto y = *iter;
        FORWARD_CALCS(*x, y);

        if(d == 0) [[unlikely]]
        {
            globals::integer potential_twin = (m1 / delta) - 1;
            if(!CONTAINS(S, potential_twin))
            {
                std::unique_lock l{M};
                R.insert(potential_twin);
            }
        }
    }
}


template<>
void generate_twins<config::OPTIMIZATION_LEVELS::GLOBAL_K_OPTIMIZATION>(arguments& args)
{
    globals::integer d, delta, m1, maxy, maxz;
    auto [x, S, R, M, C] = args;
    double k = std::get<global_k_settings>(C).current_k;

    size_t counter = 0;
    {
        globals::floating f{*x};                                                            
        f *= k;                                                             
        maxy = f; 
    }
    {
        globals::floating f{*x};                                                            
	    f *= (2.0 - k);
	    maxz = f;                                                                        
    }

    size_t forward_range = std::distance(x, S.lower_bound(maxy));
    size_t backward_range = std::distance(S.upper_bound(maxz), x);

    globals::integer xp1 = *x + 1;
    for(auto iter = std::next(x); iter != S.end() && counter < forward_range; ++iter)
    {
        counter++;
        auto y = *iter;
        FORWARD_CALCS(*x, y);

        if(d == 0) [[unlikely]]
        {
            globals::integer potential_twin = (m1 / delta) - 1;
            if(!CONTAINS(S, potential_twin))
            {
                std::unique_lock l{M};
                R.insert(potential_twin);
            }
        }
    }
    
    if(x == S.begin()) return;
        
    BACKWARD_CALCS(*x, *S.begin());
    if(d == 0) [[unlikely]]
    {
        globals::integer potential_twin = (m1 / delta) - 1;
        if(!CONTAINS(S, potential_twin))
        {
            std::unique_lock l{M};
            R.insert(potential_twin);
        }
    }

    counter = 0;

    for(auto iter = std::prev(x); iter != S.begin() && counter < backward_range; --iter)
    {
        counter++;
        BACKWARD_CALCS(*x, *iter);

        if(d == 0) [[unlikely]]
        {
            globals::integer potential_twin = (m1 / delta) - 1;
            if(!CONTAINS(S, potential_twin))
            {
                std::unique_lock l{M};
                R.insert(potential_twin);
            }
        }
    }
}

template<>
void generate_twins_forward_only<config::OPTIMIZATION_LEVELS::GLOBAL_K_OPTIMIZATION>(arguments& args)
{
    globals::integer d, delta, m1, maxy, maxz;
    auto [x, S, R, M, C] = args;
    double k = std::get<global_k_settings>(C).current_k;

    size_t counter = 0;
    {
        globals::floating f{*x};                                                            
        f *= k;                                                             
        maxy = f; 
    }
    {
        globals::floating f{*x};                                                            
	    f *= (2.0 - k);
	    maxz = f;                                                                        
    }

    size_t forward_range = std::distance(x, S.lower_bound(maxy));
    size_t backward_range = std::distance(S.upper_bound(maxz), x);

    globals::integer xp1 = *x + 1;
    for(auto iter = std::next(x); iter != S.end() && counter < forward_range; ++iter)
    {
        counter++;
        auto y = *iter;
        FORWARD_CALCS(*x, y);

        if(d == 0) [[unlikely]]
        {
            globals::integer potential_twin = (m1 / delta) - 1;
            if(!CONTAINS(S, potential_twin))
            {
                std::unique_lock l{M};
                R.insert(potential_twin);
            }
        }
    }
    
    if(x == S.begin()) return;
        
    BACKWARD_CALCS(*x, *S.begin());
    if(d == 0) [[unlikely]]
    {
        globals::integer potential_twin = (m1 / delta) - 1;
        if(!CONTAINS(S, potential_twin))
        {
            std::unique_lock l{M};
            R.insert(potential_twin);
        }
    }

    counter = 0;

    for(auto iter = std::prev(x); iter != S.begin() && counter < backward_range; --iter)
    {
        counter++;
        BACKWARD_CALCS(*x, *iter);

        if(d == 0) [[unlikely]]
        {
            globals::integer potential_twin = (m1 / delta) - 1;
            if(!CONTAINS(S, potential_twin))
            {
                std::unique_lock l{M};
                R.insert(potential_twin);
            }
        }
    }
}


void after_completion(const globals::set& S)
{
    std::string output_fd = "./result/";
	std::string output_fl = CONFIG.OutputFile + "_" + std::to_string(CONFIG.Smoothness);

	std::filesystem::create_directories(output_fd);
	splitfile file(output_fd, output_fl, CONFIG.MaxFileSize * 1000000ull);
	file.init();

	if(CONFIG.MaxBitSizeToSave == 1024)
	{
		for(auto x : S)
		{
			file << x.get_str() << "\n";
		}

		return;
	}
	int smallest_to_save = static_cast<int>(CONFIG.MinBitSizeToSave);
	int biggest_to_save = static_cast<int>(CONFIG.MaxBitSizeToSave);
	for(auto x : S)
	{
		int size = mpz_sizeinbase(x.get_mpz_t(), 2);
		if(size >= smallest_to_save && size <= biggest_to_save)
		{
			file << (std::string(x.get_str()) + std::string("\n"));
		}
	}
}

void printProgress(double percentage)
{
	static constexpr auto PBSTR = "================================================================"
								  "======================";
	static constexpr auto PBWIDTH = 86;

	int val = static_cast<int>(percentage * 100);
	int lpad = static_cast<int>(percentage * PBWIDTH);
	int rpad = PBWIDTH - lpad;

	printf("\r|%.*s%*s|", lpad, PBSTR, rpad, "");
	fflush(stdout);
}

template<config::OPTIMIZATION_LEVELS O>
void generator_filler(globals::set& S, globals::wset& work_set)
{
    std::mutex mtx;
    globals::set temporary_results{};
    size_t processed_numbers = 0;
    size_t numbers_to_process = S.size();
    std::mutex print_mutex;
    configuration conf;
    
    auto progress_bar = [&processed_numbers, &numbers_to_process]() {
        while(processed_numbers != numbers_to_process)
        {
            printProgress(processed_numbers / static_cast<double>(numbers_to_process));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        printProgress(1.0);
        std::cout << "\r|                         ||  FILLING PROCESS COMPLETED  ||                            |" << std::endl;
    };

    std::thread progress_task(progress_bar);

    benchmark b;
    b.start_bench();

    conf =  constant_range_settings{ S.size() / 8 };
    for(auto x = S.begin(); x != S.end(); x++)
    {
        CONFIG.ThreadPool.push_task([&S, &temporary_results, &mtx, &conf, &print_mutex, &processed_numbers] (iterator x) { 
            arguments pack(x, S, temporary_results, mtx, conf);
            generate_twins_forward_only<config::OPTIMIZATION_LEVELS::CONSTANT_RANGE_OPTIMIZATION>(pack); 
            std::unique_lock l(print_mutex);
            processed_numbers++;
            }, x);
    }
    CONFIG.ThreadPool.wait_for_tasks();
    for(auto& b : temporary_results)
    {
        work_set.push_back(S.insert(b).first);
    }
    b.conclude_bench();
    progress_task.join();
    table_line_string(temporary_results.size(), b.seconds_passed(), S.size());
    temporary_results.clear();


    if constexpr (O != config::OPTIMIZATION_LEVELS::NO_OPTIMIZATION )
    {
        if(!CONFIG.FinisherCompletelyNaive)
            conf =  constant_range_settings{ 10000 };
    }

    while(!work_set.empty())
    {
        benchmark b;
        b.start_bench();
        for(const auto& x : work_set)
        {
            CONFIG.ThreadPool.push_task([&S, &temporary_results, &mtx, &conf] (iterator x) { 
                arguments pack(x, S, temporary_results, mtx, conf);
                generate_twins<config::OPTIMIZATION_LEVELS::CONSTANT_RANGE_OPTIMIZATION>(pack); 
                }, x);
        }

        CONFIG.ThreadPool.wait_for_tasks();
        work_set.clear();
        for(auto& b : temporary_results)
        {
            work_set.push_back(S.insert(b).first);
        }
        b.conclude_bench();
        table_line_string(temporary_results.size(), b.seconds_passed(), S.size());
        temporary_results.clear();
    }



}

template<config::OPTIMIZATION_LEVELS O>
void generator()
{
    globals::set S{};
    globals::wset work_set{};
    std::mutex mtx;

    globals::set temporary_results{};

	std::cout << "\nEXECUTING: " << globals::GetOptimizationLevel(O) << std::endl;
	std::cout << "SMOOTHNESS: " << CONFIG.Smoothness << std::endl;
	std::cout << "THREAD COUNT: " << CONFIG.ThreadPool.get_thread_count() << std::endl;
    table_header_string();

    benchmark btot{};
    btot.start_bench();
    for(int i = 0; i < CONFIG.Smoothness; i++)
    {
        auto inserted = S.insert(i+1);
        work_set.push_back(inserted.first);
    }

    configuration conf;
    if constexpr (O == config::OPTIMIZATION_LEVELS::NO_OPTIMIZATION)
    {
        conf = no_settings{};
    }
    else if constexpr(O == config::OPTIMIZATION_LEVELS::CONSTANT_RANGE_OPTIMIZATION)
    {
        conf = constant_range_settings{static_cast<size_t>(CONFIG.RangeCurrent)};
    }
    else if constexpr(O == config::OPTIMIZATION_LEVELS::GLOBAL_K_OPTIMIZATION)
    {
        conf = global_k_settings{2.0, CONFIG.KCurrent, 2.0};
    }

    bool filler_executed = !CONFIG.RunAdditionalNaiveFinisher;

    while(!work_set.empty())
    {
        
        if constexpr (O == config::OPTIMIZATION_LEVELS::GLOBAL_K_OPTIMIZATION)
        {
            if(S.size() < 100000)
            {
                conf = global_k_settings{2.0, CONFIG.KCurrent, CONFIG.KCurrent};
            }
        }
        benchmark b;
        b.start_bench();
        for(const auto& x : work_set)
        {
            CONFIG.ThreadPool.push_task([&S, &temporary_results, &mtx, &conf] (iterator x) { 
                arguments pack(x, S, temporary_results, mtx, conf);
                generate_twins<O>(pack); 
                }, x);
        }

        CONFIG.ThreadPool.wait_for_tasks();
        work_set.clear();
        for(auto& b : temporary_results)
        {
            work_set.push_back(S.insert(b).first);
        }
        b.conclude_bench();
        table_line_string(temporary_results.size(), b.seconds_passed(), S.size());
        temporary_results.clear();
        
        if constexpr (O != config::OPTIMIZATION_LEVELS::NO_OPTIMIZATION)
        {
            if(!filler_executed && work_set.empty())
            {
                filler_executed = true;
                if(CONFIG.FinisherCompletelyNaive)
                    generator_filler<config::OPTIMIZATION_LEVELS::NO_OPTIMIZATION>(S, work_set);
                else 
                    generator_filler<O>(S, work_set);
            }
        }
    }

    btot.conclude_bench();
    std::cout << "FOUND: " << S.size() << std::endl;
    std::cout << "Total time taken: " << btot.seconds_passed() << " seconds" << std::endl;
    after_completion(S);
}
