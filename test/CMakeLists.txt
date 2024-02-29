set(
  fiona_test_sources

  test/http_test.cpp
  test/semantic_test.cpp
  test/eisen_test.cpp)

foreach(test_source ${fiona_test_sources})
  cmake_path(SET test_path ${test_source})
  cmake_path(GET test_path STEM test_filename)
  fiona_test(${test_filename})
endforeach()

add_executable(json_bench json_bench.cpp json.cpp json.hpp json_fwd.hpp)
target_link_libraries(json_bench PRIVATE Boost::headers)
