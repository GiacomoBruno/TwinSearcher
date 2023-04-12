#include "config.h"
#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>

namespace config 
{

void LoadConfigFile()
{
    if(!std::filesystem::exists("config.conf"))
		return ReadParameters();
        
	std::ifstream conf("config.conf");
	std::string line;

	auto lowercase = [](std::string& s) {
		std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
			return static_cast<char>(std::tolower(c));
		});
	};

	auto divide = [](const std::string& s, char c) {
		size_t pos = s.find_first_of(c);
		return std::tuple<std::string, std::string>{s.substr(0, pos),
													s.substr(pos + 1, s.size() - pos - 1)};
	};

	while(std::getline(conf, line))
	{
		if(line[0] == '#') // skip comment
			continue;
		lowercase(line);
		auto [name, value] = divide(line, ' ');

		if(name == "maxbitsize")
			CONFIG.MaxBitSize = std::stoi(value);
		else if(name == "minbitsizetosave")
			CONFIG.MinBitSizeToSave = std::stoi(value);
		else if(name == "maxbitsizetosave")
			CONFIG.MaxBitSizeToSave = std::stoi(value);
		else if(name == "optimizationtype")
		{
			if(value == "no_optimization")
				CONFIG.OptimizationType = static_cast<int>(OPTIMIZATION_LEVELS::NO_OPTIMIZATION);
			else if(value == "constant_range_optimization")
				CONFIG.OptimizationType =
					static_cast<int>(OPTIMIZATION_LEVELS::CONSTANT_RANGE);
			else if(value == "variable_range_optimization")
				CONFIG.OptimizationType =
					static_cast<int>(OPTIMIZATION_LEVELS::VARIABLE_RANGE);
			else if(value == "global_k_optimization")
				CONFIG.OptimizationType =
					static_cast<int>(OPTIMIZATION_LEVELS::GLOBAL_K);
		}
		else if(name == "smoothness")
			CONFIG.Smoothness = std::stoi(value);
		else if(name == "maxfilesize")
			CONFIG.MaxFileSize = std::stoi(value);
		else if(name == "range")
			CONFIG.RangeCurrent = std::stoi(value);
		else if(name == "k")
			CONFIG.KCurrent = std::stod(value);
		else if(name == "outputfile")
			CONFIG.OutputFile = value;
		else if(name == "maxnforprimecalcs")
			CONFIG.MaxNForPrimeCalcs = std::stoi(value);
		else if(name == "runadditionalnaivefinisher")
			CONFIG.RunAdditionalNaiveFinisher = value == "true";
		else if(name == "finishercompletelynaive")
			CONFIG.FinisherCompletelyNaive = value == "true";
	}

    }

    void ReadParameters()
    {
        std::cout << "smoothness: ";
        std::cin >> CONFIG.Smoothness;

        std::cout << "optimizations available: ";
        std::cout << "\n\t"
                << "no optimizations: "
                << static_cast<int>(OPTIMIZATION_LEVELS::NO_OPTIMIZATION);
        std::cout << "\n\t"
                << "constant range:   "
                << static_cast<int>(OPTIMIZATION_LEVELS::CONSTANT_RANGE);
        std::cout << "\n\t"
                << "global k:         "
                << static_cast<int>(OPTIMIZATION_LEVELS::GLOBAL_K);
        std::cout << "\n\t"
                << "variable range:   "
                << static_cast<int>(OPTIMIZATION_LEVELS::VARIABLE_RANGE);
        std::cout << "\nchoose: ";
        std::cin >> CONFIG.OptimizationType;

        switch(CONFIG.OptimizationType)
        {
        // no optimization
        case 0:
            break;
            // range optimization
        case 1:
            std::cout << "range: ";
            std::cin >> CONFIG.RangeCurrent;
            break;
            // global k optimization
        case 2:
            std::cout << "k: ";
            std::cin >> CONFIG.KCurrent;
            break;
        }

        bool bit_size_opt = false;
        std::cout << "Ignore big numbers? (0 = no, 1 = yes): ";
        std::cin >> bit_size_opt;

        if(bit_size_opt)
        {
            std::cout << "max bit size: ";
            std::cin >> CONFIG.MaxBitSize;
        }

        bool bit_size_save = false;

        std::cout << "Save only some numbers close to N bit length? (0 = no, 1 = yes): ";
        std::cin >> bit_size_save;

        if(bit_size_save)
        {
            std::cout << "max bit size to save: ";
            std::cin >> CONFIG.MaxBitSizeToSave;
            std::cout << "min bit size to save: ";
            std::cin >> CONFIG.MinBitSizeToSave;
        }

        std::cout << "output file name: ";
        std::cin >> CONFIG.OutputFile;

        std::cout << "maximum file size (in MB): ";
        std::cin >> CONFIG.MaxFileSize;

        std::cout << "run finisher? (1/0) : ";
        std::cin >> CONFIG.RunAdditionalNaiveFinisher;

        std::cout << "completely naive finisher? (1/0) : ";
        std::cin >> CONFIG.FinisherCompletelyNaive;
    }   

}