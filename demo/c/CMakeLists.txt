cmake_minimum_required(VERSION 3.4)
project(porcupine_demo_c)

set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Release)

set(COMMON_LIBS dl)
set(MIC_LIBS pthread m)

include_directories("${PROJECT_SOURCE_DIR}/../../include")

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