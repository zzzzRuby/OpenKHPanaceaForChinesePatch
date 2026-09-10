cmake_minimum_required(VERSION 3.21)

project(OpenKhResearchPanacea LANGUAGES C CXX)

set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_MSVC_RUNTIME_LIBRARY
	"$<$<CONFIG:Debug>:MultiThreadedDebugDLL>$<$<NOT:$<CONFIG:Debug>>:MultiThreaded>")

if (NOT MSVC)
	message(FATAL_ERROR "Panacea must be built with MSVC")
endif()

set(PANACEA_SOURCES 
	src/dllmain.cpp
	src/KingdomApi.cpp
	src/OpenKH.cpp
	src/Panacea.cpp
	src/KingdomApi.h
	src/OpenKH.h
	src/Panacea.h)

function(panacea_dll TARGET)
	target_include_directories(${TARGET} PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/dependencies"
	)
	target_compile_definitions(${TARGET} PRIVATE
		UNICODE _UNICODE
		_CRT_SECURE_NO_WARNINGS
		_WINDOWS
		_USRDLL
		OPENKHRESEARCHPANACEA_EXPORTS
	)
	target_compile_options(${TARGET} PRIVATE
		/W3
		/sdl
		/permissive-
		$<$<CONFIG:Release>:/Zi>
	)
	target_link_options(${TARGET} PRIVATE
		/SUBSYSTEM:WINDOWS
		/MANIFESTUAC:NO
		"/DELAYLOAD:bass.dll"
		"/DELAYLOAD:bass_vgmstream.dll"
		$<$<CONFIG:Release>:/DEBUG>
		$<$<CONFIG:Release>:/OPT:ICF>
		$<$<CONFIG:Release>:/OPT:REF>
	)
	set_target_properties(${TARGET} PROPERTIES
		INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
		OUTPUT_NAME DBGHELP)

	target_link_libraries(${TARGET} PRIVATE
		Shlwapi
		"${CMAKE_CURRENT_SOURCE_DIR}/dependencies/bass.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/dependencies/bass_vgmstream.lib"
		delayimp
	)
endfunction()

add_library(OpenKH.Panacea SHARED ${PANACEA_SOURCES})
panacea_dll(OpenKH.Panacea)
set_target_properties(OpenKH.Panacea PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
add_library(OpenKH.Panacea.DI SHARED ${PANACEA_SOURCES} src/kh1_font_patch.c)
panacea_dll(OpenKH.Panacea.DI)
target_compile_definitions(OpenKH.Panacea.DI PRIVATE PANACEA_WITH_DYNAMIC_INJECT)
set_target_properties(OpenKH.Panacea.DI PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/DI)