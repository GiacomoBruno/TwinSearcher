#pragma once
#include "searcher_configuration.h"
#include "tree_type.h"

class global_variables
{
   public:
    static global_variables &instance()
    {
        static global_variables m_Instance{};
        return m_Instance;
    }

    configuration conf{};
    globals::set  S{};

   private:
    global_variables(){};
};