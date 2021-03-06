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

copy_to_build(props.lua)
add_executable(props props.cpp props.lua)
target_link_libraries(props ${LUA_LIBRARIES})

copy_to_build(plugin_test.lua)
add_executable(plugin_test plugin_test.cpp plugin_test.lua)
if(NOT MSYS)
	target_link_libraries(plugin_test ${LUA_LIBRARIES} dl)
else(NOT MSYS)
	target_link_libraries(plugin_test ${LUA_LIBRARIES})
endif(NOT MSYS)

add_library(plugin MODULE plugin.cpp)
if(MSYS OR MINGW)
	target_link_libraries(plugin ${LUA_LIBRARIES})
endif(MSYS OR MINGW)

copy_to_build(light_sandbox.lua)
add_executable(light_sandbox light_sandbox.cpp light_sandbox.lua)
target_link_libraries(light_sandbox ${LUA_LIBRARIES})

copy_to_build(array.lua)
add_executable(arrays array.cpp array.lua)
target_link_libraries(arrays ${LUA_LIBRARIES})

copy_to_build(stdfunc.lua)
add_executable(stdfunc stdfunc.cpp stdfunc.lua)
target_link_libraries(stdfunc ${LUA_LIBRARIES})

copy_to_build(luavalue.lua)
add_executable(luavalue luavalue.cpp luavalue.lua)
target_link_libraries(luavalue ${LUA_LIBRARIES})

add_executable(mfile mfile1.cpp mfile2.cpp mfilemain.cpp)
target_link_libraries(mfile ${LUA_LIBRARIES})
