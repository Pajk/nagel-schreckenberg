# Locate and configure the GAlib library.
#
# Defines the following variables:
#
#  GALIB_FOUND - System has GAlib
#  GALIB_INCLUDE_DIRS - The GAlib include directories
#  GALIB_LIBRARIES - The libraries needed to use GAlib
#
# Copyright 2013 Pavel Pokorny

FIND_PATH(GALIB_INCLUDE_DIR ga/ga.h galib247)

FIND_LIBRARY(GALIB_LIBRARY ga galib247/ga)

SET(GALIB_LIBRARIES ${GALIB_LIBRARY} )
SET(GALIB_INCLUDE_DIRS ${GALIB_INCLUDE_DIR} )

# MESSAGE(STATUS "GAlib library ${GALIB_LIBRARIES}")
# MESSAGE(STATUS "GAlib include directory ${GALIB_INCLUDE_DIRS}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GALIB DEFAULT_MSG
                                  GALIB_LIBRARY GALIB_INCLUDE_DIR)

MARK_AS_ADVANCED(GALIB_LIBRARY GALIB_INCLUDE_DIR)
