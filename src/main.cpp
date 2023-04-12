#include "searcher/algorithm.h"

int main()
{
    config::LoadConfigFile();

    switch (static_cast<config::OPTIMIZATION_LEVELS>(CONFIG.OptimizationType))
    {
        using enum config::OPTIMIZATION_LEVELS;
    case NO_OPTIMIZATION:
        searcher::algorithm<NO_OPTIMIZATION>();
        break;
    case CONSTANT_RANGE:
        searcher::algorithm<CONSTANT_RANGE>();
        break;
    case GLOBAL_K:
        searcher::algorithm<GLOBAL_K>();
        break;
    default: break;
    }

    return 0;
}