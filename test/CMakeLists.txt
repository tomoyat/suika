# CTestのテストを追加
include(GoogleTest)

set(SOURCE_LIST
        UdpTest.cpp
)

foreach (SOURCE ${SOURCE_LIST})
    get_filename_component(TEST_NAME ${SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${SOURCE})
    target_link_libraries(${TEST_NAME} gtest_main lib)
    gtest_discover_tests(${TEST_NAME})
endforeach ()

foreach (SOURCE ${SOURCE_LIST})
    get_filename_component(TEST_NAME ${SOURCE} NAME_WE)
    target_include_directories( ${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib)
endforeach ()
