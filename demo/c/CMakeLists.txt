cmake_minimum_required(VERSION 3.20)
project(porcupine_demo_c)

set(CMAKE_C_STANDARD 99)

list(APPEND COMMON_LIBS dl)
list(APPEND MIC_LIBS pthread m)

list(APPEND INCLUDES "${PROJECT_SOURCE_DIR}/../../include")

include_directories(${INCLUDES})

add_compile_options("-O3;")

add_executable(
        porcupine_demo_mic
        porcupine_demo_mic.c)

add_executable(
        porcupine_demo_file
        porcupine_demo_file.c)

if (NOT WIN32)
    target_link_libraries(porcupine_demo_mic ${COMMON_LIBS} ${MIC_LIBS})
    target_link_libraries(porcupine_demo_file ${COMMON_LIBS})
endif()