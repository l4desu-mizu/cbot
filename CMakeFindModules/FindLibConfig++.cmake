# - Try to find LibConfig++
# Once done this will define
#  LIBCONFIGPP_FOUND - System has LibConfig++
#  LIBCONFIGPP_INCLUDE_DIRS - The LibConfig++ include directories
#  LIBCONFIGPP_LIBRARIES - The libraries needed to use LibConfig++
#  LIBCONFIGPP_DEFINITIONS - Compiler switches required for using LibConfig++

find_package(PkgConfig)
pkg_check_modules(PC_LIBCONFIGPP QUIET libconfig++)
set(LIBCONFIGPP_DEFINITIONS ${PC_LIBCONFIGPPCFLAGS_OTHER})

find_path(LIBCONFIGPP_INCLUDE_DIR libconfig.h++
          HINTS ${PC_LIBCONFIGPP_INCLUDEDIR} ${PC_LIBCONFIGPP_INCLUDE_DIRS})

find_library(LIBCONFIGPP_LIBRARY NAMES libconfig++ config++
             HINTS ${PC_LIBCONFIGPP_LIBDIR} ${PC_LIBCONFIGPP_LIBRARY_DIRS} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBCONFIGPP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBCONFIGPP DEFAULT_MSG
                                  LIBCONFIGPP_LIBRARY LIBCONFIGPP_INCLUDE_DIR)

mark_as_advanced(LIBCONFIGPP_INCLUDE_DIR LIBCONFIGPP_LIBRARY)

set(LIBCONFIGPP_LIBRARIES ${LIBCONFIGPP_LIBRARY} )
set(LIBCONFIGPP_INCLUDE_DIRS ${LIBCONFIGPP_INCLUDE_DIR} )
