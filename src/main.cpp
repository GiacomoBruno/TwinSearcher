#include "searcher/algorithm.h"

int main()
{
    config::LoadConfigFile();

    switch (static_cast<config::OPTIMIZATION_LEVELS>(CONFIG.OptimizationType))
    {
    case config::OPTIMIZATION_LEVELS::NO_OPTIMIZATION:
        searcher::algorithm<config::OPTIMIZATION_LEVELS::NO_OPTIMIZATION>();
        break;
    case config::OPTIMIZATION_LEVELS::CONSTANT_RANGE_OPTIMIZATION:
        searcher::algorithm<config::OPTIMIZATION_LEVELS::CONSTANT_RANGE_OPTIMIZATION>();
        break;
    case config::OPTIMIZATION_LEVELS::GLOBAL_K_OPTIMIZATION:
        break;
    default: break;
    }

    return 0;
}