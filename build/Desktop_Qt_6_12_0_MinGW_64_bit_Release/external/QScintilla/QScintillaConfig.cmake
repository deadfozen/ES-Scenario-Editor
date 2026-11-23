
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was QScintillaConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

set(QScintilla_VERSION )

# Export include directories to the configuration file
set(QScintilla_INCLUDE_DIR C:/Program Files (x86)/es-scenario-editor/include)

set(QScintilla_BINARY_DIR C:/Program Files (x86)/es-scenario-editor/bin)

set(QScintilla_LIBRARY_DIR C:/Program Files (x86)/es-scenario-editor/lib)

# Declare dependencies for find_package(QScintilla)
include(CMakeFindDependencyMacro)
find_dependency(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Gui PrintSupport)

# Set libraries
set(QScintilla_LIBRARIES QScintilla::QScintilla)

# Provide the library target
add_library(QScintilla::QScintilla SHARED IMPORTED)
set_target_properties(QScintilla::QScintilla PROPERTIES
    IMPORTED_LOCATION "${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}qscintilla2_qt${QT_VERSION_MAJOR}${CMAKE_SHARED_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${QSCINTILLA_INCLUDE_DIRS}"
)

# Include the exported targets
include("${CMAKE_CURRENT_LIST_DIR}/QScintillaTargets.cmake")
