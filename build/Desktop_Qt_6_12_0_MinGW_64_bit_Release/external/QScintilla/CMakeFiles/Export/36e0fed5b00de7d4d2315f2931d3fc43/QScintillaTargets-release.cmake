#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "QScintilla::qscintilla2_qt6" for configuration "Release"
set_property(TARGET QScintilla::qscintilla2_qt6 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(QScintilla::qscintilla2_qt6 PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/libqscintilla2_qt6.dll.a"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "Qt6::Widgets;Qt6::Gui;Qt6::PrintSupport"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/libqscintilla2_qt6.dll"
  )

list(APPEND _cmake_import_check_targets QScintilla::qscintilla2_qt6 )
list(APPEND _cmake_import_check_files_for_QScintilla::qscintilla2_qt6 "${_IMPORT_PREFIX}/lib/libqscintilla2_qt6.dll.a" "${_IMPORT_PREFIX}/bin/libqscintilla2_qt6.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
