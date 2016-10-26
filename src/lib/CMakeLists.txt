########################################################################
#setup library
########################################################################
include_directories(${CMAKE_SOURCE_DIR}/src/lib)

list(APPEND gps_sources
    gps_main.cpp
    tsip.cpp
    )

set(gps_sources "${gps_sources}" PARENT_SCOPE)
if(NOT gps_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT gps_sources)


add_executable(gps_main gps_main.cpp tsip.cpp)
#target_link_libraries(gps_main build)

