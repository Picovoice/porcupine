cmake_minimum_required(VERSION 3.13)
project(porcupine_demo_c)

set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Release)
add_subdirectory(pvrecorder)

set(COMMON_LIBS dl)
set(MIC_LIBS pthread m)

include_directories("${PROJECT_SOURCE_DIR}/../../include")

add_executable(
        porcupine_demo_mic
        porcupine_demo_mic.c
        $<TARGET_OBJECTS:pv_recorder_object>)
target_include_directories(porcupine_demo_mic PRIVATE pvrecorder/include)

add_executable(
        porcupine_demo_file
        porcupine_demo_file.c)
target_include_directories(porcupine_demo_file PRIVATE dr_libs)

if (NOT WIN32)
    target_link_libraries(porcupine_demo_mic ${COMMON_LIBS} ${MIC_LIBS})
    target_link_libraries(porcupine_demo_file ${COMMON_LIBS})
    if((${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm") AND (UNIX AND NOT APPLE))
        target_link_libraries(porcupine_demo_mic atomic)
    endif()
endif()
