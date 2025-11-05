# Copyright (C) 2007-2012 LuaDist.
# Created by Peter Drahoš
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

project ( libiconv C )
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

set(CMAKE_MODULE_PATH .)
include ( configure.cmake )

set(LIBICONV_VERSION 1.18)

set(SRCDIR ${LIBICONV_VERSION})

handle_warning_options("libiconv" "CC" -Werror -Wdeprecated-non-prototype -Wstrict-prototypes -Wstatic-in-inline -Wparentheses-equality -Wundef -Wno-reserved-macro-identifier -Wno-unused-parameter -Wno-sign-compare -Wno-type-limits -Wno-implicit-fallthrough -Wno-cast-align -Wno-parentheses-equality)

# Options
option(ENABLE_EXTRA "Enable a few rarely used encodings" OFF)
option(ENABLE_NLS "Translation of program messages to the user's native
   language is requested" OFF)
#option(ENABLE_RELOCATABLE "The package shall run at any location in the file system" ON)

# iconv.h
set(USE_MBSTATE_T 1)
set(BROKEN_WCHAR_H 0)
set(HAVE_WCHAR_T 0)

set(BINDIR ${CMAKE_BINARY_DIR}/libiconv)
configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
configure_file(${SRCDIR}/include/iconv.h.build.in ${CMAKE_CURRENT_BINARY_DIR}/iconv.h)
configure_file(${SRCDIR}/libcharset/include/localcharset.h.build.in ${CMAKE_CURRENT_BINARY_DIR}/localcharset.h)
configure_file(${SRCDIR}/libcharset/include/libcharset.h.build.in ${CMAKE_CURRENT_BINARY_DIR}/libcharset.h)

# Generating these two files causes libcharset build errors, because they are found before the ones in ${CMAKE_CURRENT_BINARY_DIR}.
# Fortunately they are not needed externally, so we do not need to create them.
#configure_file(${SRCDIR}/libcharset/include/localcharset.h.in ${BINDIR}/include/localcharset.h)
#configure_file(${SRCDIR}/libcharset/include/libcharset.h.in ${BINDIR}/include/libcharset.h)
configure_file(${SRCDIR}/srclib/uniwidth.in.h ${BINDIR}/srclib/uniwidth.h)
configure_file(${SRCDIR}/srclib/unitypes.in.h ${BINDIR}/srclib/unitypes.h)
configure_file(${SRCDIR}/include/iconv.h.in ${BINDIR}/include/iconv.h)

# Dirty fix for MinGW
if(MINGW)
  add_definitions(-DELOOP=0 -DHAVE_DECL_STRERROR_R=0)
endif ()

include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR} ${BINDIR}/include ${SRCDIR}/include ${SRCDIR}/srclib)
add_definitions(-Dset_relocation_prefix=libcharset_set_relocation_prefix -Drelocate=libcharset_relocate -DHAVE_CONFIG_H -DINSTALLPREFIX=NULL -DNO_XMALLOC -DBUILDING_LIBCHARSET -DINSTALLDIR="" -DLIBDIR="" -DENABLE_RELOCATABLE=1 -DIN_LIBRARY)

# libcharset
set(SRC_LIBCHARSET ${SRCDIR}/libcharset/lib/localcharset.c)

# libiconv
set(SRC_LIBICONV ${SRCDIR}/lib/compat.c ${SRCDIR}/lib/iconv.c)

add_library(iconv ${SRC_LIBICONV} ${SRC_LIBCHARSET})
set_target_properties(iconv PROPERTIES COMPILE_FLAGS -DBUILDING_LIBICONV)

set(ICONV_INCLUDE_DIR ${BINDIR}/include CACHE STRING "libiconv include  dir" FORCE)
set(ICONV_LIBRARY iconv CACHE STRING "libiconv library" FORCE)
set(ICONV_FOUND iconv CACHE STRING "libiconv found" FORCE)

foreach(_base iconv)
  handle_source_option("CC" ${SRCDIR}/lib/${_base}.c -Wno-unused-macros)
endforeach()

handle_source_option("CC" ${SRCDIR}/lib/iconv.c -Wno-cast-align -Wno-parentheses-equality -Wno-static-in-inline -Wno-unused-parameter -Wno-sign-compare -Wno-type-limits -Wno-missing-field-initializers -Wno-redundant-decls -Wno-implicit-fallthrough -Wno-strict-prototypes)
set_target_properties(iconv PROPERTIES
	FOLDER "3rd_party"
	INTERPROCEDURAL_OPTIMIZATION ${LYX_IPO_SUPPORTED})
