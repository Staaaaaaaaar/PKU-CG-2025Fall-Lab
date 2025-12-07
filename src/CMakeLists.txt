add_subdirectory(common)

add_executable(texture main.cpp)
target_link_libraries(texture PRIVATE common)