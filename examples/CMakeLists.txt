cmake_minimum_required(VERSION 2.8)
set(EXAMPLES true CACHE BOOL "Examples: Include example programs in build")
set(EXAMPLES_SBLK_PAR false CACHE BOOL "Examples-sblk: Enable parallel implementation of sblk")
if (NOT EXAMPLES)
	return()
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DNVM_DEBUG_ENABLED")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

include_directories("${CMAKE_SOURCE}/include")

set(SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/addr.c
	${CMAKE_CURRENT_SOURCE_DIR}/info.c
	${CMAKE_CURRENT_SOURCE_DIR}/vblk.c
	${CMAKE_CURRENT_SOURCE_DIR}/sblk.c)

#
# We link against the lightnvm_a to avoid the runtime dependency on liblightnvm
# NOTE: The dependency on libudev is carried on from liblightnvm
#
foreach(SRC_FN ${SOURCE_FILES})
	get_filename_component(SRC_FN_WE ${SRC_FN} NAME_WE)
	set(EXE_FN "nvm_${SRC_FN_WE}")
	add_executable(${EXE_FN} ${SRC_FN})
	target_link_libraries(${EXE_FN} pthread udev lightnvm_a)

	# Enable OpenMP for sblk
	if (${EXAMPLES_SBLK_PAR} AND ${SRC_FN_WE} STREQUAL "sblk")
		find_package(OpenMP)
		if (OPENMP_FOUND)
			set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
		endif()
	endif()
	install(TARGETS ${EXE_FN} DESTINATION bin)
endforeach()

