#pragma once

#include <chrono>

template <class T>
concept DurationType = std::is_same_v<T, std::chrono::nanoseconds> || std::is_same_v<T, std::chrono::microseconds> ||
    std::is_same_v<T, std::chrono::milliseconds> || std::is_same_v<T, std::chrono::seconds> || std::is_same_v<T, std::chrono::minutes>;

class stopwatch
{
   private:
    using t = std::chrono::steady_clock::time_point;

    t start_time{};
    t pause_time{};

   public:
    void start();
    void pause();

    template <DurationType T>
    [[nodiscard]] double elapsed() const
    {
        auto duration = duration_cast<std::chrono::nanoseconds>(pause_time - start_time).count();
        if constexpr (std::is_same_v<T, std::chrono::nanoseconds>)
            return static_cast<double>(duration);
        else if constexpr (std::is_same_v<T, std::chrono::microseconds>)
            return duration / 1000.0;
        else if constexpr (std::is_same_v<T, std::chrono::milliseconds>)
            return duration / 1000000.0;
        else if constexpr (std::is_same_v<T, std::chrono::seconds>)
            return duration / 1000000000.0;
        else // minutes
            return duration / 60000000000.0;
    }
};
