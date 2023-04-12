#pragma once
#include <vector>
#include "BS_thread_pool_light.h"

//#define CHECK_BITSIZE
#define CONFIG config::Configuration::Instance()

namespace config {
    
    enum class OPTIMIZATION_LEVELS
    {
        NO_OPTIMIZATION = 0,
        CONSTANT_RANGE = 1,
        GLOBAL_K = 2,
        VARIABLE_RANGE = 3
    };

    class Configuration
    {
    public:
        static Configuration& Instance()
        {
            static Configuration instance;
            return instance;
        }

        static constexpr int ChunkSize{100};
        int MaxBitSize{1024};
        int MinBitSizeToSave{0};
        int MaxBitSizeToSave{1024};
        int OptimizationType{0};
        int Smoothness{0};
        int MaxFileSize{20};
        int MaxNForPrimeCalcs{4};
        bool RunAdditionalNaiveFinisher{false};
        bool FinisherCompletelyNaive{false};

        int RangeStart{0};
        int RangeEnd{0};
        int RangeCurrent{0};
        int RangeStep{0};

        double KStart{0.0};
        double KEnd{0.0};
        double KCurrent{0.0};
        double KStep{0.0};

        std::vector<int>& GetRelevantPrimes();
        BS::thread_pool_light ThreadPool{};
        std::string OutputFile{};

    private:
        Configuration() = default;
        std::vector<int> RelevantPrimes{};
    };

    void LoadConfigFile();

    void ReadParameters();
}