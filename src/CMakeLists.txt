add_subdirectory(common)

add_executable(particle main.cpp)
target_link_libraries(particle PRIVATE common)