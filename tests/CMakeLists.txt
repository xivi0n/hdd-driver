file(GLOB tests_src
  "*.c"
)

foreach (test_src ${tests_src})
    get_filename_component(testname ${test_src} NAME_WLE)
    add_executable(${testname} ${testname}.c)
    target_link_libraries(${testname} PUBLIC driver)
    target_include_directories(${testname} PUBLIC 
            "${PROJECT_BINARY_DIR}" 
            "${PROJECT_SOURCE_DIR}/driver")
endforeach()