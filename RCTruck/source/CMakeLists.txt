# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(source)


# Add custom compiler flags
if(CONFIG_COMPILER_WARNINGS)
    add_compile_options(-Wno-format)  # Suppress the format string warning
    add_compile_options(-Wno-error=format)  # Disable treating it as an error
endif()