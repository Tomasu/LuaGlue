project(examples CXX)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	add_definitions(-DDEBUG -O0 -ggdb3)
else()
	add_definitions(-O2 -s -march=native)
endif()

find_package(Lua52 REQUIRED)

include_directories(${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

copy_to_build(foo.lua)
add_executable(foo foo.cpp foo.lua)
target_link_libraries(foo ${LUA_LIBRARIES})
