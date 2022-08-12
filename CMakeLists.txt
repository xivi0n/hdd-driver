cmake_minimum_required(VERSION 3.10)

project(DiskDriver)

add_subdirectory(driver)
add_subdirectory(tests)

file(GLOB tests_src
  "tests/*.c"
)

ADD_CUSTOM_TARGET(tests ALL)
foreach (test_src ${tests_src})
    get_filename_component(testname ${test_src} NAME_WLE)
    add_custom_command(TARGET tests
                        COMMAND make ${testname})
endforeach()
