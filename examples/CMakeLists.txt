project(examples CXX)

find_package(Lua52 REQUIRED)

include_directories(${LUA_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

copy_to_build(foo.lua)
add_executable(foo foo.cpp foo.lua)
target_link_libraries(foo ${LUA_LIBRARIES})

copy_to_build(prof.lua)
add_executable(prof prof.cpp prof.lua)
target_link_libraries(prof ${LUA_LIBRARIES})

copy_to_build(invoke.lua)
add_executable(invoke invoke.cpp invoke.lua)
target_link_libraries(invoke ${LUA_LIBRARIES})

#add_executable(constobj constobj.cpp)
#target_link_libraries(constobj ${LUA_LIBRARIES})

copy_to_build(shared_ptr.lua)
add_executable(shared_ptr shared_ptr.cpp shared_ptr.lua)
target_link_libraries(shared_ptr ${LUA_LIBRARIES})
