cmake_minimum_required(VERSION 2.6)

project(LuaGlue CXX)

option(LUAGLUE_TYPECHECK "enable strict typechecking (slows things down quite a bit)" OFF)

# tell CMake to search first in the cmake subdirectory for FIND_PACKAGE() or INCLUDE()
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/)
set(WARNINGS -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef)

add_definitions(${WARNINGS})

if(LUAGLUE_TYPECHECK STREQUAL ON)
	add_definitions(-DLUAGLUE_TYPECHECK)
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
	add_definitions(-DLG_DEBUG -O0 -ggdb3)
elseif(CMAKE_BUILD_TYPE STREQUAL Prof)
	add_definitions(-DLG_DEBBUG -O2 -ggdb3 -march=native)
else()
	add_definitions(-O2 -s -march=native)
endif()

add_definitions(-Wextra -Wall -pipe -std=gnu++11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR})

function(copy_to_build file_name)
	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${file_name}
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${file_name} ${CMAKE_CURRENT_BINARY_DIR}/${file_name}
		MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${file_name})
endfunction()

add_subdirectory(examples)
