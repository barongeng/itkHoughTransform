set(GDCM_TARGETS_NAME ${ITK-GDCM-targets})
set(GDCM_INSTALL_BIN_DIR ${ITK-GDCM_INSTALL_RUNTIME_DIR})
set(GDCM_INSTALL_LIB_DIR ${ITK-GDCM_INSTALL_LIBRARY_DIR})
set(GDCM_INSTALL_INCLUDE_DIR ${ITK-GDCM_INSTALL_INCLUDE_DIR})
set(GDCM_INSTALL_DATA_DIR ${ITK_INSTALL_DATA_DIR}/gdcm)
set(GDCM_BUILD_TESTING_DEFAULT OFF)

# Tell GDCM to use "system" libraries but give it ITK's libraries:
#
# Expat:
set(GDCM_USE_SYSTEM_EXPAT ON CACHE INTERNAL "")
set(EXPAT_INCLUDE_DIR ${ITK-Expat_INCLUDE_DIRS})
set(EXPAT_LIBRARY ${ITK-Expat_LIBRARIES})
# OpenJPEG:
set(GDCM_USE_SYSTEM_OPENJPEG ON CACHE INTERNAL "")
set(OPENJPEG_MAJOR_VERSION 2)
set(OPENJPEG_LIBRARY ${ITK-OpenJPEG_LIBRARIES})
set(OPENJPEG_LIBRARIES ${OPENJPEG_LIBRARY})
set(OPENJPEG_INCLUDE_DIR ${ITK-OpenJPEG_INCLUDE_DIRS})
# ZLIB:
set(GDCM_USE_SYSTEM_ZLIB ON CACHE INTERNAL "")
if(NOT ITK_USE_SYSTEM_ZLIB)
  set(ZLIB_INCLUDE_DIR ${ITK-ZLIB_SOURCE_DIR}/src/itkzlib)
  set(ZLIB_LIBRARY itkzlib)
endif()

# GDCM uses these to refer to executable and library locations
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

add_subdirectory(gdcm)

foreach(lib
    gdcmCommon
    gdcmDICT
    gdcmDSED
    gdcmIOD
    gdcmMSFF
    gdcmcharls
    gdcmjpeg12
    gdcmjpeg16
    gdcmjpeg8
    gdcmuuid
    )
  if(TARGET ${lib}) # some gdcm targets are conditional
    set_property(TARGET ${lib} PROPERTY VERSION 1)
    set_property(TARGET ${lib} PROPERTY SOVERSION "")
    set_property(TARGET ${lib} PROPERTY OUTPUT_NAME itk${lib}-${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR})
    itk_module_target_export(${lib})
  endif()
endforeach()
