#This file determines what generator is being used and will set which superbuild to use. 

message(STATUS "Determining which build system to use: ")

include(getCompiler.cmake)

if(MSVC_FOUND)
	message("Found Visual Studio! Determining Debug or Release mode...")
  if(Debug_Mode)
		message("Debug found! Using Debug build.")
    set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE INTERNAL "" FORCE)
		set(Superbuild ${CMAKE_SOURCE_DIR}/Superbuild/Win64D/VS/Superbuild.cmake CACHE INTERNAL "")
	else()
		message("Release found! Using Release build.")
    set(CMAKE_CONFIGURATION_TYPES "Release" CACHE INTERNAL "" FORCE)
		set(Superbuild ${CMAKE_SOURCE_DIR}/Superbuild/Win64R/VS/Superbuild.cmake CACHE INTERNAL "")
	endif()
endif()
