#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "QScintilla::qscintilla2_qt6d" for configuration "Debug"
set_property(TARGET QScintilla::qscintilla2_qt6d APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(QScintilla::qscintilla2_qt6d PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/libqscintilla2_qt6d.dll.a"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_DEBUG "Qt6::Widgets;Qt6::Gui;Qt6::PrintSupport"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/libqscintilla2_qt6d.dll"
  )

list(APPEND _cmake_import_check_targets QScintilla::qscintilla2_qt6d )
list(APPEND _cmake_import_check_files_for_QScintilla::qscintilla2_qt6d "${_IMPORT_PREFIX}/lib/libqscintilla2_qt6d.dll.a" "${_IMPORT_PREFIX}/bin/libqscintilla2_qt6d.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
