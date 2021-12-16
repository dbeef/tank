include(Protocompiler.cmake)

set(PROTOCOMPILER_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(PROTO_FILES_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(PROTO_FILES Services.proto)
compile_protos("${PROTO_FILES}")

add_library(Protos STATIC ${COMPILED_FILES})
target_include_directories(Protos INTERFACE ${PROTOCOMPILER_OUTPUT_DIR})
target_link_libraries(Protos PUBLIC Dependencies)
set_target_properties(Protos PROPERTIES CXX_STANDARD 17)
