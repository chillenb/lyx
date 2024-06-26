# This file is part of LyX, the document processor.
# Licence details can be found in the file COPYING.
#
# Copyright (c) 2012 Kornel Benko, <kornel@lyx.org>
#

project(testTex2lyx)

#file(GLOB _tex_tests RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.ltx" "${CMAKE_CURRENT_SOURCE_DIR}/*.tex")
#
#list(REMOVE_ITEM _tex_tests DummyDocument.tex)

set(_tex_tests test.ltx
               algo2e.tex
               beamer.tex
               box-color-size-space-align.tex
               CJK.tex
               CJKutf8.tex
               listpreamble.tex
               tabular-x-test.tex
               test-insets-basic.tex
               test-insets.tex
               test-memoir.tex
               test-minted.tex
               test-modules.tex
               test-refstyle-theorems.tex
               test-scr.tex
               test-structure.tex
               verbatim.tex
               XeTeX-polyglossia.tex)

set(runtestsScript "${TOP_SRC_DIR}/src/tex2lyx/test/runtests.py")
set(scriptDir "${TOP_SRC_DIR}/lib/scripts")

foreach(_fl ${_tex_tests})
  set(fl ${_fl})
  add_test(NAME tex2lyx/roundtrip/${_fl}
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND}
      -DLYX_TESTS_USERDIR=${LYX_TESTS_USERDIR}
      -DLYX_USERDIR_VER=${LYX_USERDIR_VER}
      -DLYX_PYTHON_EXECUTABLE=${LYX_PYTHON_EXECUTABLE}
      -DPY_SCRIPT=${runtestsScript}
      -DTEX2LYX_EXE=$<TARGET_FILE:${_tex2lyx}>
      -DSCRIPT_DIR=${scriptDir}
      -DWORKDIR=${CMAKE_CURRENT_BINARY_DIR}
      -DTESTFILE=${fl}
      -P ${TOP_SRC_DIR}/src/tex2lyx/test/runtests.cmake)
  set_tests_properties(tex2lyx/roundtrip/${_fl} PROPERTIES RESOURCE_LOCK "runtests.lock" LABELS "roundtrip:tex2lyx")
  add_test(NAME tex2lyx/cmplyx/${_fl}
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND}
      -DLYX_TESTS_USERDIR=${LYX_TESTS_USERDIR}
      -DLYX_USERDIR_VER=${LYX_USERDIR_VER}
      -DLYX_PYTHON_EXECUTABLE=${LYX_PYTHON_EXECUTABLE}
      -DPY_SCRIPT=${runtestsScript}
      -DFIRST_PARAM=uselyx2lyx
      -DTEX2LYX_EXE=$<TARGET_FILE:${_tex2lyx}>
      -DSCRIPT_DIR=${scriptDir}
      -DWORKDIR=${CMAKE_CURRENT_BINARY_DIR}
      -DTESTFILE=${fl}
      -P ${TOP_SRC_DIR}/src/tex2lyx/test/runtests.cmake)
  set_tests_properties(tex2lyx/cmplyx/${_fl} PROPERTIES RESOURCE_LOCK "runtests.lock" LABELS "cmplyx:tex2lyx")
endforeach()

add_dependencies(lyx_run_tests ${_tex2lyx} ${_lyx})

set(LyxTestFiles "")            # "'" separated test-filenames
set(LyxRemoveFiles)             # Extra files created by runtests.py
foreach(_fl ${_tex_tests})
  set(LyxTestFiles "${LyxTestFiles}'${_fl}")
  if (_fl MATCHES "^\(.*\)\\.\([^\\.]+)$")
    list(APPEND LyxRemoveFiles "${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_MATCH_1}.lyx.tex")
  endif()
endforeach()
foreach(_lrf "DummyDocument.lyx.tex" "Dummy~Document.lyx.tex" "foo.pdf" "xfigtest.pdf" "xfigtest.pdftex_t")
  list(APPEND LyxRemoveFiles "${CMAKE_CURRENT_SOURCE_DIR}/${_lrf}")
endforeach()
list(APPEND LyxRemoveFiles "${TOP_SRC_DIR}/lib/examples/longsheet.tex")

add_custom_command(
  OUTPUT Tex2lyxFilesUpdated
  COMMAND ${CMAKE_COMMAND}
  -DCOMMAND="${LYX_PYTHON_EXECUTABLE}"
  -DENVIRON="${LYX_USERDIR_VER}=${LYX_TESTS_USERDIR}"
  -DPARAMS="${runtestsScript}'$<TARGET_FILE:${_tex2lyx}>'${scriptDir}'${CMAKE_CURRENT_SOURCE_DIR}"
  -DSINGLEPARLIST="${LyxTestFiles}"
  -P ${TOP_SRC_DIR}/src/tex2lyx/test/env_custom_command.cmake
  COMMAND ${CMAKE_COMMAND} -E touch Tex2lyxFilesUpdated
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  DEPENDS ${_tex2lyx} ${_lyx}
  )

add_custom_target(updatetex2lyxtests DEPENDS Tex2lyxFilesUpdated)
set_target_properties(updatetex2lyxtests PROPERTIES FOLDER "tests/tex2lyx")

# In order to not pollute the source directory
# now remove some untracked files created by the target updatetex2lyxtests
add_custom_command(
  OUTPUT UpdateFilesRemoved
  COMMAND ${CMAKE_COMMAND} -E remove -f ${LyxRemoveFiles}
  COMMAND ${CMAKE_COMMAND} -E touch UpdateFilesRemoved
  DEPENDS Tex2lyxFilesUpdated
)

add_custom_target(cleanupdatetex2lyxtests DEPENDS UpdateFilesRemoved updatetex2lyxtests)
set_target_properties(cleanupdatetex2lyxtests PROPERTIES FOLDER "tests/tex2lyx")
