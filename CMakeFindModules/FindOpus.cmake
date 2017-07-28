# - Try to find Opus
# Once done this will define
#  OPUS_FOUND - System has LibOpus
#  OPUS_INCLUDE_DIRS - The LibOpus include directories
#  OPUS_LIBRARIES - The libraries needed to use LibOpus
#  OPUS_DEFINITIONS - Compiler switches required for using LibOpus

find_package(PkgConfig)
pkg_check_modules(PC_OPUS QUIET opus)
set(OPUS_DEFINITIONS ${PC_OPUSCFLAGS_OTHER})

find_path(OPUS_INCLUDE_DIR opus.h
          HINTS ${PC_OPUS_INCLUDEDIR} ${PC_OPUS_INCLUDE_DIRS})

find_library(OPUS_LIBRARY NAMES libopus opus
             HINTS ${PC_OPUS_LIBDIR} ${PC_OPUS_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set OPUS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(OPUS DEFAULT_MSG
                                  OPUS_LIBRARY OPUS_INCLUDE_DIR)

mark_as_advanced(OPUS_INCLUDE_DIR OPUS_LIBRARY)

set(OPUS_LIBRARIES ${OPUS_LIBRARY} )
set(OPUS_INCLUDE_DIRS ${OPUS_INCLUDE_DIR} )
