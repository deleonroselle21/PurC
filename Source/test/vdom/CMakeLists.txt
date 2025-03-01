include(PurCCommon)
include(target/PurC)
include(GoogleTest)

enable_testing()

# test_vdom
PURC_EXECUTABLE_DECLARE(test_vdom)

list(APPEND test_vdom_PRIVATE_INCLUDE_DIRECTORIES
    ${PURC_DIR}/include
    ${PurC_DERIVED_SOURCES_DIR}
    ${PURC_DIR}
    ${CMAKE_BINARY_DIR}
    ${WTF_DIR}
)

PURC_EXECUTABLE(test_vdom)

set(test_vdom_SOURCES
    test_vdom.cpp
)

set(test_vdom_LIBRARIES
    PurC::PurC
    gtest_main
    gtest
    pthread
)

PURC_COMPUTE_SOURCES(test_vdom)
PURC_FRAMEWORK(test_vdom)
GTEST_DISCOVER_TESTS(test_vdom DISCOVERY_TIMEOUT 10)

# test_vdom_gen
PURC_EXECUTABLE_DECLARE(test_vdom_gen)

list(APPEND test_vdom_gen_PRIVATE_INCLUDE_DIRECTORIES
    ${PURC_DIR}/include
    ${PurC_DERIVED_SOURCES_DIR}
    ${PURC_DIR}
    ${CMAKE_BINARY_DIR}
    ${WTF_DIR}
    #ugly: for hvml_token and hvml_parser
    ${PURC_DIR}/hvml
)

PURC_EXECUTABLE(test_vdom_gen)

set(test_vdom_gen_SOURCES
    test_vdom_gen.cpp
)

set(test_vdom_gen_LIBRARIES
    PurC::PurC
    gtest_main
    gtest
    pthread
)

PURC_COMPUTE_SOURCES(test_vdom_gen)
PURC_FRAMEWORK(test_vdom_gen)
GTEST_DISCOVER_TESTS(test_vdom_gen DISCOVERY_TIMEOUT 10)

