cmake_minimum_required(VERSION 3.16)
project(twinsmooth_searcher)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(twinsmooth_searcher)

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    if (supported)
        message(STATUS "IPO / LTO enabled")
        set_property(TARGET twinsmooth_searcher PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else ()
        message(STATUS "IPO / LTO not supported: <${error}>")
    endif ()
endif ()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(GMP_INCLUDE_DIR "/opt/homebrew/include")
    set(GMP_LIBRARY_DIR "/opt/homebrew/lib")
    link_directories(${GMP_LIBRARY_DIR})
    include_directories(twinsmooth_searcher ${GMP_INCLUDE_DIR})
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif ()




target_sources(twinsmooth_searcher
        PRIVATE ${PROJECT_SOURCE_DIR}/src/benchmark.cpp
        PRIVATE ${PROJECT_SOURCE_DIR}/src/config.cpp
        PRIVATE ${PROJECT_SOURCE_DIR}/src/splitfile.cpp
        PRIVATE ${PROJECT_SOURCE_DIR}/src/output.cpp
        PRIVATE ${PROJECT_SOURCE_DIR}/src/main.cpp
        )
target_include_directories(twinsmooth_searcher
        PUBLIC ${PROJECT_SOURCE_DIR}/inc
        PUBLIC ${PROJECT_SOURCE_DIR}/third_party
        PRIVATE ${PROJECT_SOURCE_DIR}/src
        )
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(twinsmooth_searcher gmp)
else ()
    target_link_libraries(twinsmooth_searcher gmp pthread)
endif ()