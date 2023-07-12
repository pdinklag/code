# we need the iopp submodule, initialize it
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules")
include(InitGitSubmodules)
add_subdirectory(iopp)

add_executable(test-universe test_universe.cpp)
target_link_libraries(test-universe PRIVATE code)
add_test(universe ${CMAKE_CURRENT_BINARY_DIR}/test-universe)

add_executable(test-binary test_binary.cpp)
target_link_libraries(test-binary PRIVATE code)
add_test(binary ${CMAKE_CURRENT_BINARY_DIR}/test-binary)

add_executable(test-unary test_unary.cpp)
target_link_libraries(test-unary PRIVATE code)
add_test(unary ${CMAKE_CURRENT_BINARY_DIR}/test-unary)

add_executable(test-vbyte test_vbyte.cpp)
target_link_libraries(test-vbyte PRIVATE code)
add_test(vbyte ${CMAKE_CURRENT_BINARY_DIR}/test-vbyte)

add_executable(test-elias-gamma test_elias_gamma.cpp)
target_link_libraries(test-elias-gamma PRIVATE code)
add_test(elias-gamma ${CMAKE_CURRENT_BINARY_DIR}/test-elias-gamma)

add_executable(test-elias-delta test_elias_delta.cpp)
target_link_libraries(test-elias-delta PRIVATE code)
add_test(elias-delta ${CMAKE_CURRENT_BINARY_DIR}/test-elias-delta)

add_executable(test-huffman test_huffman.cpp)
target_link_libraries(test-huffman PRIVATE code iopp)
add_test(huffman ${CMAKE_CURRENT_BINARY_DIR}/test-huffman)

add_executable(test-rice test_rice.cpp)
target_link_libraries(test-rice PRIVATE code)
add_test(rice ${CMAKE_CURRENT_BINARY_DIR}/test-rice)

add_executable(test-universal test_universal.cpp)
target_link_libraries(test-universal PRIVATE code iopp)
add_test(universal ${CMAKE_CURRENT_BINARY_DIR}/test-universal)

add_executable(test-examples test_examples.cpp)
target_link_libraries(test-examples PRIVATE code iopp)
add_test(examples ${CMAKE_CURRENT_BINARY_DIR}/test-examples)
