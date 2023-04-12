#include "stopwatch.h"

using namespace std::chrono;

void stopwatch::start()
{
    start_time = steady_clock::now();
}

void stopwatch::pause()
{
    pause_time = steady_clock::now();
}
