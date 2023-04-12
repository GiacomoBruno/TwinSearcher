#pragma once
#include "../config.h"
#include "global_variables.h"
#include "searcher_configuration.h"
#include "tree_type.h"

namespace searcher
{

#if TREE_TYPE == STL_TREE
#    define INSERT_NEW_TWIN() work_set.push_back(insertion.first)
#else
#    define INSERT_NEW_TWIN() work_set.push_back(x)
#endif

using namespace globals;
using optimization = config::OPTIMIZATION_LEVELS;
using gb           = global_variables;

struct twin_generator
{
    template <optimization O>
    void generate_fw() noexcept
    {
        x_type x = GET_X(current_element);
        generate_forward<O>(x);
        current_element++;
        elements_to_work_on--;
    }

    template <optimization O>
    void generate_bw() noexcept
    {
        x_type x = GET_X(current_element);
        generate_backward<O>(x);
        current_element++;
        elements_to_work_on--;
    }

    template <optimization O>
    void generate() noexcept
    {
        x_type x = GET_X(current_element);
        generate_forward<O>(x);
        if (x != gb::instance().S.begin())
            generate_backward<O>(x);

        current_element++;
        elements_to_work_on--;
    }

    [[nodiscard]] bool has_work() const noexcept
    {
        return elements_to_work_on > 0;
    }

    std::set<integer> results{};
    integer           d{};
    integer           delta{};
    integer           m1{};
    work_set_iterator current_element{};
    size_t            elements_to_work_on{};

   private:
    template <optimization O>
    void generate_forward(x_type &) noexcept;

    template <optimization O>
    void generate_backward(x_type &) noexcept;
};

} // namespace searcher