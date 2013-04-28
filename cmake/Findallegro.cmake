# - Find allegro
# Find the native ALLEGRO includes and library
#
# ALLEGRO_INCLUDE_DIR - where to find allegro.h, etc.
# ALLEGRO_LIBRARIES - List of libraries when using allegro.
# ALLEGRO_FOUND - True if allegro found.


IF (ALLEGRO_INCLUDE_DIR)
    # Already in cache, be silent
    SET(ALLEGRO_FIND_QUIETLY TRUE)
ENDIF (ALLEGRO_INCLUDE_DIR)

FIND_PATH(ALLEGRO_INCLUDE_DIR allegro.h
/usr/local/include
/usr/include
$ENV{MINGDIR}/include
)

if(UNIX AND NOT CYGWIN)
    exec_program(allegro-config ARGS --libs OUTPUT_VARIABLE ALLEGRO_LIBRARY)
else(UNIX AND NOT CYGWIN)
    SET(ALLEGRO_NAMES alleg alleglib alleg41 alleg42 allegdll)
    FIND_LIBRARY(ALLEGRO_LIBRARY
        NAMES ${ALLEGRO_NAMES}
        PATHS /usr/lib /usr/local/lib $ENV{MINGDIR}/lib)
endif(UNIX AND NOT CYGWIN)

SET( ALLEGRO_LIBRARIES ${ALLEGRO_LIBRARY} )
SET( ALLEGRO_INCLUDE_DIRS ${ALLEGRO_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ALLEGRO DEFAULT_MSG
                                  ALLEGRO_LIBRARY ALLEGRO_INCLUDE_DIR)

MARK_AS_ADVANCED(ALLEGRO_LIBRARY ALLEGRO_INCLUDE_DIR)
