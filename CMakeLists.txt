cmake_minimum_required(VERSION 3.28)

add_compile_definitions(CONFIG_FREERTOS_USE_TICK_HOOK)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(mastering_freertos)