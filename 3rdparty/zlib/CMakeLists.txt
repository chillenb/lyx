cmake_minimum_required(VERSION 3.10)

set(LYX_IPO_SUPPORTED FALSE)
if (POLICY CMP0069)
  cmake_policy(SET CMP0069 NEW)
  if (LYX_USE_IPO MATCHES "ON")
    set(LYX_IPO_SUPPORTED YES)
  endif()
endif()
if (POLICY CMP0075)
  cmake_policy(SET CMP0075 NEW)
endif()

project(zlib C)

set(VERSION "1.3.1")
set(SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/${VERSION})

handle_warning_options("zlib" "CC" -Wno-deprecated-non-prototype -Wno-strict-prototypes -Wnested-anon-types -Wno-implicit-fallthrough -Wno-format-nonliteral -Wno-reserved-macro-identifier -Wno-cast-align)

include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)
enable_testing()

check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(stdint.h    HAVE_STDINT_H)
check_include_file(stddef.h    HAVE_STDDEF_H)

#
# Check to see if we have large file support
#
set(CMAKE_REQUIRED_DEFINITIONS -D_LARGEFILE64_SOURCE=1)
# We add these other definitions here because CheckTypeSize.cmake
# in CMake 2.4.x does not automatically do so and we want
# compatibility with CMake 2.4.x.
if(HAVE_SYS_TYPES_H)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_SYS_TYPES_H)
endif()
if(HAVE_STDINT_H)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_STDINT_H)
endif()
if(HAVE_STDDEF_H)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_STDDEF_H)
endif()
check_type_size(off64_t OFF64_T)
if(HAVE_OFF64_T)
   add_definitions(-D_LARGEFILE64_SOURCE=1)
endif()
set(CMAKE_REQUIRED_DEFINITIONS) # clear variable

#
# Check for fseeko
#
check_function_exists(fseeko HAVE_FSEEKO)
if(NOT HAVE_FSEEKO)
    add_definitions(-DNO_FSEEKO)
endif()

#
# Check for unistd.h
#
check_include_file(unistd.h Z_HAVE_UNISTD_H)

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    include_directories(${SRCDIR})
endif()

if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    # If we're doing an out of source build and the user has a zconf.h
    # in their source tree...
    if(EXISTS ${SRCDIR}/zconf.h)
        message(STATUS "Renaming")
        message(STATUS "    ${SRCDIR}/zconf.h")
        message(STATUS "to 'zconf.h.included' because this file is included with zlib")
        message(STATUS "but CMake generates it automatically in the build directory.")
        file(RENAME ${SRCDIR}/zconf.h ${SRCDIR}/zconf.h.included)
  endif()
endif()

set(Z_PREFIX 1)
configure_file(	${SRCDIR}/zconf.h.cmakein
		${CMAKE_CURRENT_BINARY_DIR}/zconf.h @ONLY)


#============================================================================
# zlib
#============================================================================

set(ZLIB_PUBLIC_HDRS
    ${CMAKE_CURRENT_BINARY_DIR}/zconf.h
    ${SRCDIR}/zlib.h
)
set(ZLIB_PRIVATE_HDRS
    ${SRCDIR}/crc32.h
    ${SRCDIR}/deflate.h
    ${SRCDIR}/gzguts.h
    ${SRCDIR}/inffast.h
    ${SRCDIR}/inffixed.h
    ${SRCDIR}/inflate.h
    ${SRCDIR}/inftrees.h
    ${SRCDIR}/trees.h
    ${SRCDIR}/zutil.h
)
set(ZLIB_SRCS
    ${SRCDIR}/adler32.c
    ${SRCDIR}/compress.c
    ${SRCDIR}/crc32.c
    ${SRCDIR}/deflate.c
    ${SRCDIR}/gzclose.c
    ${SRCDIR}/gzlib.c
    ${SRCDIR}/gzread.c
    ${SRCDIR}/gzwrite.c
    ${SRCDIR}/inflate.c
    ${SRCDIR}/infback.c
    ${SRCDIR}/inftrees.c
    ${SRCDIR}/inffast.c
    ${SRCDIR}/trees.c
    ${SRCDIR}/uncompr.c
    ${SRCDIR}/zutil.c
)

add_library(zlibstatic STATIC ${ZLIB_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})
target_include_directories(zlibstatic PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${SRCDIR})

set(ZLIB_LIBRARY zlibstatic CACHE STRING "Zlib library" FORCE)
set(ZLIB_INCLUDE_DIR  ${SRCDIR} CACHE STRING "Zlib include dir" FORCE)
set(ZLIB_FOUND CACHE STRING "Zlib found" FORCE )

set_target_properties(zlibstatic PROPERTIES
	FOLDER "3rd_party"
	INTERPROCEDURAL_OPTIMIZATION ${LYX_IPO_SUPPORTED})
