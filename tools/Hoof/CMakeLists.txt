project(Hoof CXX)

find_package(LLVM REQUIRED)

# work around a bunch of warnings in llvm/clang.
set(WARNINGS_TO_REMOVE -Wundef -Wunused-parameter -Wsign-conversion -Wstrict-aliasing -Wsign-promo -Wswitch-default -Wctor-dtor-privacy)
set(WARNINGS_TO_ADD "-Wno-unused-parameter -Wno-strict-aliasing -fno-strict-aliasing -Wno-ctor-dtor-privacy")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CFLAGS}")

foreach(ITEM ${WARNINGS_TO_REMOVE})
	string(REPLACE "${ITEM}" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endforeach(ITEM)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	LLVM_CONFIG(LLVM_HOST_TARGET host-target)
	
	FIND_PROGRAM(LLVM_CC_COMPILER NAMES g++ c++ gcc cc)
	
	if(NOT LLVM_CC_COMPILER)
		message(SEND_ERROR "Failed to find gcc :(")
	endif(NOT LLVM_CC_COMPILER)
	
	execute_process(COMMAND ${LLVM_CC_COMPILER} "-dumpmachine" OUTPUT_VARIABLE LLVM_CC_TARGET OUTPUT_STRIP_TRAILING_WHITESPACE)

	execute_process(COMMAND ${LLVM_CC_COMPILER} "-print-prog-name=cc1plus" OUTPUT_VARIABLE LLVM_CC1_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
	message("CC1: " ${LLVM_CC1_PATH})
	if(NOT LLVM_CC1_PATH OR "${LLVM_CC1_PATH}" STREQUAL "cc1plus")
		message(SEND_ERROR "Failed to get cc1plus program path")
	endif()
	
	execute_process(COMMAND echo COMMAND "${LLVM_CC1_PATH}" "-imultiarch" "${LLVM_CC_TARGET}" "-dM" "-E" "-v" "-std=gnu++11" OUTPUT_VARIABLE LLVM_CC1_OUTPUT ERROR_VARIABLE LLVM_CC1_OUTPUT OUTPUT_STRIP_TRAILING_WHITESPACE)
	#message("CC1 output: " ${LLVM_CC1_OUTPUT} " error: " ${LLVM_CC1_ERROR})
	if(NOT LLVM_CC1_OUTPUT)
		message(SEND_ERROR "Failed to get cc1plus output")
	endif()
	
	string(REGEX REPLACE "[\r\n]+" ";" LLVM_CC_LINES "${LLVM_CC1_OUTPUT}")
	
	set(LLVM_IN_SEARCH 0)
	foreach(ITEM ${LLVM_CC_LINES})
	#	message("cc: " ${ITEM})
		string(REGEX MATCH "^#include <...> search starts here:" MATCH_SEARCH "${ITEM}")
		string(REGEX MATCH "^End of search list." MATCH_SEARCH_END "${ITEM}")
		string(REGEX MATCH "^#define" MATCH_MACRO "${ITEM}")
		if(MATCH_SEARCH)
			set(LLVM_IN_SEARCH 1)
		elseif(MATCH_SEARCH_END)
			set(LLVM_IN_SEARCH 0)
		endif()
		
		if(LLVM_IN_SEARCH EQUAL 1)
			list(APPEND LLVM_SEARCH_PATH "${ITEM}")
		elseif(MATCH_MACRO)
			list(APPEND LLVM_BUILTIN_MACROS "${ITEM}")
		endif()
	endforeach()
	
	list(REMOVE_AT LLVM_SEARCH_PATH 0)
	
	set(LLVM_BUILTIN_MACROS_HEADER "${CMAKE_CURRENT_BINARY_DIR}/builtin_macros.h")
	set(LLVM_BUILTIN_INC_HEADER "${CMAKE_CURRENT_BINARY_DIR}/builtin_inc.h")
	
	file(WRITE "${LLVM_BUILTIN_INC_HEADER}" "const char *builtinIncludePaths[] = {")
	foreach(ITEM ${LLVM_SEARCH_PATH})
		string(STRIP "${ITEM}" ITEM)
		file(APPEND "${LLVM_BUILTIN_INC_HEADER}" "\"${ITEM}\",")
	endforeach()
	file(APPEND "${LLVM_BUILTIN_INC_HEADER}" "};")

	file(WRITE "${LLVM_BUILTIN_MACROS_HEADER}" "const char *builtinMacros = ")
	foreach(ITEM ${LLVM_BUILTIN_MACROS})
		string(STRIP "${ITEM}" ITEM)
		string(REPLACE "\"" "\\\"" ITEM ${ITEM})
		file(APPEND "${LLVM_BUILTIN_MACROS_HEADER}" "\"${ITEM}\\n\"\n")
	endforeach()
	file(APPEND "${LLVM_BUILTIN_MACROS_HEADER}" ";")
	
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CFLAGS} ${WARNINGS_TO_ADD}")

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LLVM_LDFLAGS}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR})

SET(SRC 
	src/main.cpp
	src/ASTConsumer.cpp
	src/ASTVisitor.cpp
	src/Hoof.cpp
)

add_executable(hoof ${SRC})
target_link_libraries(hoof ${LLVM_LIBS})
