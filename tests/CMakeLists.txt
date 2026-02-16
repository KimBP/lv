# Zero-cost assembly comparison test
add_executable(zero_cost_test zero_cost_test.cpp)
target_link_libraries(zero_cost_test PRIVATE lv::lv)

# Compile-only API smoke test (catches overload ambiguities, missing methods)
add_executable(api_smoke_test api_smoke_test.cpp)
target_link_libraries(api_smoke_test PRIVATE lv::lv)


