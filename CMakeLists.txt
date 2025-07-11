cmake_minimum_required(VERSION 3.5)
project(my_app CXX)

include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)

# Set C++ version depending on feature availability
if (NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
    # User has set standard manually
elseif (HAS_ANY AND HAS_STRING_VIEW AND HAS_COROUTINE)
    set(CMAKE_CXX_STANDARD 20)
elseif (HAS_ANY AND HAS_STRING_VIEW)
    set(CMAKE_CXX_STANDARD 17)
else ()
    set(CMAKE_CXX_STANDARD 14)
endif ()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Main executable
add_executable(${PROJECT_NAME} main.cc)

# Find Drogon (system-installed)
find_package(Drogon CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)

# Version message
if (CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "C++17 or higher is required")
elseif (CMAKE_CXX_STANDARD LESS 20)
    message(STATUS "Using C++17")
else ()
    message(STATUS "Using C++20")
endif ()

# Auto-discover source files
aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)

# Generate views
drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
                    ${CMAKE_CURRENT_BINARY_DIR})

# Include directories (✅ fixed syntax here)
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/models
        ${CMAKE_CURRENT_SOURCE_DIR}/sbe/my_app_messages
)

# Add source files to target
target_sources(${PROJECT_NAME}
    PRIVATE
        ${CTL_SRC}
        ${FILTER_SRC}
        ${PLUGIN_SRC}
        ${MODEL_SRC}
)

# Optional: dynamically load views
# set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)

# Unit test directory
add_subdirectory(test)
