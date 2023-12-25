include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        Archive_tests
        Archive_tests.cpp
        ${PROJECT_SOURCE_DIR}/parser/parser.cpp
        ${PROJECT_SOURCE_DIR}/parser/parser.h
        ${PROJECT_SOURCE_DIR}/write/write.cpp
        ${PROJECT_SOURCE_DIR}/write/write.h
        ${PROJECT_SOURCE_DIR}/hamming/hamming.cpp
        ${PROJECT_SOURCE_DIR}/hamming/hamming.h)

include_directories(Archive_tests PUBLIC ${PROJECT_SOURCE_DIR}../parser
                                                ${PROJECT_SOURCE_DIR}../write
                                                ${PROJECT_SOURCE_DIR}../hamming)

target_link_libraries(
        Archive_tests
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(Archive_tests)

set_target_properties(Archive_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
