#INCLUDE(FindPkgConfig)
#PKG_CHECK_MODULES(PC_VANDEVENDER vandevender)

#FIND_PATH(
    #VANDEVENDER_INCLUDE_DIRS
    #NAMES vandevender/api.h
    #HINTS $ENV{VANDEVENDER_DIR}/include
        #${PC_VANDEVENDER_INCLUDEDIR}
    #PATHS ${CMAKE_INSTALL_PREFIX}/include
          #/usr/local/include
          #/usr/include
#)

#FIND_LIBRARY(
    #VANDEVENDER_LIBRARIES
    #NAMES gnuradio-vandevender
    #HINTS $ENV{VANDEVENDER_DIR}/lib
        #${PC_VANDEVENDER_LIBDIR}
    #PATHS ${CMAKE_INSTALL_PREFIX}/lib
          #${CMAKE_INSTALL_PREFIX}/lib64
          #/usr/local/lib
          #/usr/local/lib64
          #/usr/lib
          #/usr/lib64
#)

#INCLUDE(FindPackageHandleStandardArgs)
#FIND_PACKAGE_HANDLE_STANDARD_ARGS(VANDEVENDER DEFAULT_MSG VANDEVENDER_LIBRARIES VANDEVENDER_INCLUDE_DIRS)
#MARK_AS_ADVANCED(VANDEVENDER_LIBRARIES VANDEVENDER_INCLUDE_DIRS)

