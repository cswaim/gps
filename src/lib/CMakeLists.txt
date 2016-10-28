########################################################################
#setup library
########################################################################

list(APPEND gps_sources
    gps_test.cpp
    tsip.cpp
    gps_survey.cpp
    )

set(gps_sources "${gps_sources}" PARENT_SCOPE)
if(NOT gps_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT gps_sources)


add_executable(gps_test gps_test.cpp tsip.cpp)
add_executable(gps_survey gps_survey.cpp tsip.cpp)
target_link_libraries(gps_survey ${Boost_LIBRARIES})

########################################################################
# Install built library files
########################################################################
install(TARGETS gps_test gps_survey
		RUNTIME DESTINATION /usr/local/bin    
		)	          
