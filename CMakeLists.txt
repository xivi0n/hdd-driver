cmake_minimum_required(VERSION 3.10)

project(DiskDriver)

add_subdirectory(driver)

file(GLOB tests_src
    "tests/*.c"
)

foreach (test_src ${tests_src})
    get_filename_component(testname ${test_src} NAME_WLE)
    add_executable(${testname} tests/${testname}.c)
    target_link_libraries(${testname} PUBLIC Driver)
    target_include_directories(${testname} PUBLIC 
            "${PROJECT_BINARY_DIR}" 
            "${PROJECT_SOURCE_DIR}/Driver")
endforeach()