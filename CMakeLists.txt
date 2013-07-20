cmake_minimum_required (VERSION 2.6)

if (NOT DEBUG)
  if ( CMAKE_BUILD_TYPE STREQUAL Debug )
    set (DEBUG 1)
  else ( CMAKE_BUILD_TYPE STREQUAL Debug )
    set (DEBUG 0)
  endif ( CMAKE_BUILD_TYPE STREQUAL Debug )
endif (NOT DEBUG)


if (NOT PROJECT_PORTABLE)
  set (PROJECT_PORTABLE 0)
endif ()

if (NOT NOT_INSTALL_PROJECT_DOCS)
  set (NOT_INSTALL_PROJECT_DOCS NO)
endif ()

IF (WIN32)
  IF ( PROJECT_PORTABLE STREQUAL 1 )
    set (EXE_ADD_PORTABLE -portable)
  ENDIF ()

  if (NOT VCREDIST_DIR)
    set (VCREDIST_DIR "C:/Program Files/Microsoft SDKs/Windows/v7.1/Redist/VC" )
  endif ()

  if (NOT NSIS_DIR)
    set (NSIS_DIR "c:/Program Files (x86)/NSIS")
  endif (NOT NSIS_DIR)
ENDIF (WIN32)

set (PROJECT_NAME "basketpwd")

set (PROJECT ${PROJECT_NAME})
set (LIB_PROJECT basketlib)
set (PROJECT_VERSION 0.4.8)
set (HEADERS 
	src/aboutdialog.h
	src/changepassword.h
	src/mainwindow.h
	src/settingsdialog.h
	model/basketmodel.h
    src/basketcommon.h
    src/passwordwidget.h) 
set (SOURCES
	src/aboutdialog.cpp
	src/changepassword.cpp
	src/main.cpp
	src/mainwindow.cpp
	src/settingsdialog.cpp
	model/basketmodel.cpp
    src/basketcommon.cpp
    src/passwordwidget.cpp)
set (FORMS
	ui/aboutdialog.ui
	ui/mainwindow.ui
	ui/changepassword.ui
	ui/settingsdialog.ui)
set (LIB_HEADERS
  libs/basketpwd/basketbaseitem.h
  libs/basketpwd/basketpwd-libs_global.h
  libs/basketpwd/basketutils.h
  libs/basketpwd/coreplugin.h
  libs/basketpwd/plugin_share.h)
set (LIB_SOURCES
  libs/basketpwd/basketbaseitem.cpp
  libs/basketpwd/basketutils.cpp
  libs/basketpwd/coreplugin.cpp)

set (LANG_PATH l10n)
set (LANGUAGES en)
#set (RESOURCES icon-images.qrc)
set (RESOURCES basketpwd.qrc icon-images.qrc)
set (TRANSLATIONS_BINARY qt_ru.qm)
set (DESKTOP_FILE tools/${PROJECT}.desktop)
set (ICON_DEST_PATH share/icons/hicolor)
set (ICON_SIZES
  16x16
  22x22
  32x32
  48x48
  64x64
  128x128
  256x256)

set (DOCS ChangeLog.txt README)

set (MACOSX_NAME "Basket of passwords")
set (MACOSX_BUNDLE_NAME ${MACOSX_NAME}.app)

set (QT_USE_QTXML TRUE)
set (QT_USE_QTNETWORK FALSE)

project (${PROJECT})

configure_file (
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
)

include_directories (model src ${CMAKE_CURRENT_BINARY_DIR})

if (WIN32)
    find_package (Qt4 COMPONENTS QtMain QtCore QtGui QtXml REQUIRED)
else (WIN32)
    find_package (Qt4 COMPONENTS QtCore QtGui QtXml REQUIRED)
endif (WIN32)
find_package (OpenSSL REQUIRED)
include (${QT_USE_FILE})
qt4_add_resources (QRC_SOURCES ${RESOURCES})
qt4_wrap_cpp (MOC_SOURCES ${HEADERS})
qt4_wrap_cpp (MOC_LIB_SOURCES ${LIB_HEADERS})
qt4_wrap_ui (UI_SOURCES ${FORMS})

IF(APPLE)
  INCLUDE_DIRECTORIES ( /Developer/Headers/FlatCarbon )
  FIND_LIBRARY(CARBON_LIBRARY Carbon)
  MARK_AS_ADVANCED (CARBON_LIBRARY)
  SET(EXTRA_LIBS ${CARBON_LIBRARY})
ENDIF(APPLE)

foreach (LANGUAGE ${LANGUAGE})
	set (TS ${LANG_PATH}/${LANGUAGE}.ts)
	set (QM ${LANG_PATH}/${LANGUAGE}.qm)
	set (TRANSLATIONS ${TRANSLATIONS} ${TS})
	set (TRANSLATIONS_BINARY ${TRANSLATIONS_BINARY} ${QM})
	add_custom_command (
		OUTPUT ${QM}
		COMMAND ${QT_LRELEASE_EXECUTABLE} ${TS}
		MAIN_DEPENDENCY ${TS})
endforeach ()


add_custom_target (
	translations
	COMMAND ${QT_LUPDATE_EXECUTABLE} ${HEADERS} ${SOURCES} -ts ${TRANSLATIONS})

add_custom_command (
	TARGET translations
	COMMAND ${QT_LRELEASE_EXECUTABLE} ${TRANSLATIONS})

IF(APPLE)
add_custom_target (
  clean-mac-bundle
  rm -rf ${MACOSX_BUNDLE_NAME}
)

add_custom_target (
  mac-pre-bundle
  DEPENDS ${PROJECT} clean-mac-bundle
  # make a dirs
  COMMAND mkdir -p ${MACOSX_BUNDLE_NAME}/Contents/
  COMMAND mkdir -p ${MACOSX_BUNDLE_NAME}/Contents/MacOS
  COMMAND mkdir -p ${MACOSX_BUNDLE_NAME}/Contents/Resources

  # copy binary and plist-files
  COMMAND install -m 0755 -p ${PROJECT} ${MACOSX_BUNDLE_NAME}/Contents/MacOS/${MACOSX_NAME}
  # contents
  COMMAND install -m 0644 -p ${CMAKE_CURRENT_SOURCE_DIR}/tools/Info.plist ${MACOSX_BUNDLE_NAME}/Contents/Info.plist
  COMMAND install -m 0644 -p ${CMAKE_CURRENT_SOURCE_DIR}/tools/PkgInfo ${MACOSX_BUNDLE_NAME}/Contents/PkgInfo
  # resources
  COMMAND install -m 0644 -p ${CMAKE_CURRENT_SOURCE_DIR}/icons/basketpwd.icns ${MACOSX_BUNDLE_NAME}/Contents/Resources/basketpwd.icns
  COMMAND cp -R ${CMAKE_CURRENT_SOURCE_DIR}/tools/mac_resource/* ${MACOSX_BUNDLE_NAME}/Contents/Resources/

)
add_custom_target (
  mac-bundle
  DEPENDS mac-pre-bundle
  COMMAND macdeployqt ${MACOSX_BUNDLE_NAME}
)

set(DMG_DIR "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-build-dmg")
set(BUNDLE_DIR "${CMAKE_BINARY_DIR}/Basket of passwords.app")
add_custom_target (
  mac-dmg
  DEPENDS mac-bundle
 
  COMMAND mkdir -p ${DMG_DIR}
  COMMAND cp -R ${BUNDLE_DIR} ${DMG_DIR}/
  COMMAND ln -sf /Applications ${DMG_DIR}
  COMMAND cp ${CMAKE_SOURCE_DIR}/icons/${PROJECT_NAME}.icns .VolumeIcon.icns
  COMMAND hdiutil create ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}.dmg -srcfolder ${DMG_DIR}
  DEPENDS ${PROJECT_NAME}
  COMMENT "Create DMG image"
   
  #COMMAND macdeployqt ${MACOSX_BUNDLE_NAME} -dmg
)
ENDIF (APPLE)

# Make a windows installer
IF ( WIN32 )
  file(MAKE_DIRECTORY vcredist)
  IF ( CMAKE_SIZEOF_VOID_P STREQUAL 4 )
    set (PROJECT_ARCH 86)
    set (PROJECT_ARCH_BIT 32)
  ELSEIF ( CMAKE_SIZEOF_VOID_P STREQUAL 8 )
    set (PROJECT_ARCH 64)
    set (PROJECT_ARCH_BIT 64)
  ELSE ()
    message( FATAL_ERROR "Architecture may not be checked!")
  ENDIF ()

  IF ( EXISTS "${VCREDIST_DIR}/vcredist_x${PROJECT_ARCH}.exe" )
    message(STATUS "Microsoft Visual C++ Redistributable Package (${VCREDIST_DIR}/vcredist_x${PROJECT_ARCH}.exe) found.")
    IF ( EXISTS "${NSIS_DIR}/makensis.exe" )
      message(STATUS "Nullsoft Scriptable Install System (${NSIS_DIR}/makensis.exe) found.")
      configure_file (
        "${PROJECT_SOURCE_DIR}/tools/nsis/basketpwd.nsi.in"
        "${PROJECT_BINARY_DIR}/basketpwd.nsi"
        @ONLY
        NEWLINE_STYLE WIN32
        )
      add_custom_target (
        nsis
        DEPENDS ${PROJECT} ${LIB_PROJECT} qt-libs
        
        COMMAND "${NSIS_DIR}/makensis.exe" "${PROJECT_BINARY_DIR}/basketpwd.nsi"
        )
    ELSE ( EXISTS "${NSIS_DIR}/makensis.exe" )
      message(STATUS "Nullsoft Scriptable Install System (makensis.exe) not found.")
    ENDIF ( EXISTS "${NSIS_DIR}/makensis.exe" )

    add_custom_target (
      qt-libs
      COMMAND ${CMAKE_COMMAND} -E copy ${QT_BINARY_DIR}/QtCore4.dll ${PROJECT_BINARY_DIR}
      COMMAND ${CMAKE_COMMAND} -E copy ${QT_BINARY_DIR}/QtGui4.dll ${PROJECT_BINARY_DIR}
      COMMAND ${CMAKE_COMMAND} -E copy ${QT_BINARY_DIR}/QtXml4.dll ${PROJECT_BINARY_DIR}
      COMMAND ${CMAKE_COMMAND} -E copy ${OPENSSL_ROOT_DIR}/bin/libeay32.dll ${PROJECT_BINARY_DIR}
      
      COMMAND ${CMAKE_COMMAND} -E copy ${VCREDIST_DIR}/vcredist_x${PROJECT_ARCH}.exe ${PROJECT_BINARY_DIR}
      )

  ELSE ( EXISTS "${VCREDIST_DIR}/vcredist_x${PROJECT_ARCH}.exe" )
    message(STATUS "Microsoft Visual C++ Redistributable Package (vcredist_x${PROJECT_ARCH}.exe) not found.")
  ENDIF ( EXISTS "${VCREDIST_DIR}/vcredist_x${PROJECT_ARCH}.exe" )

ENDIF ( WIN32 )  

include_directories(libs ${OPENSSL_INCLUDE_DIR})

source_group ("Header Files" FILES ${HEADERS} ${LIB_HEADERS})
source_group ("Source Files" FILES ${SOURCES} ${LIB_SOURCES})
source_group ("Forms Files" FILES ${UI_SOURCES})
source_group ("Generated Files" FILES ${MOC_SOURCES} ${MOC_LIB_SOURCES})
source_group ("Resource Files" FILES ${QRC_SOURCES})

if (WIN32)
  add_library (${LIB_PROJECT} SHARED ${LIB_HEADERS} ${LIB_SOURCES} ${MOC_LIB_SOURCES})
  set (GUI_TYPE WIN32)
  set (RC_FILE ${PROJECT_SOURCE_DIR}/basketpwd.rc)
  ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}.o
    COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}.rc 
    -o ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}.o )
  set (RC_FILE ${PROJECT_SOURCE_DIR}/basketpwd.rc)
elseif (APPLE)
  add_library (${LIB_PROJECT} ${LIB_HEADERS} ${LIB_SOURCES} ${MOC_LIB_SOURCES})
#  set(GUI_TYPE MACOSX_BUNDLE)
else ()
  add_library (${LIB_PROJECT} ${LIB_HEADERS} ${LIB_SOURCES} ${MOC_LIB_SOURCES})
endif (WIN32)

add_executable (${PROJECT} ${GUI_TYPE} ${HEADERS} ${SOURCES} ${UI_SOURCES} ${MOC_SOURCES} ${QRC_SOURCES} ${TRANSLATIONS} ${RC_FILE})

set_target_properties(${LIB_PROJECT} PROPERTIES DEFINE_SYMBOL BASKETPWD_LIBRARY)

target_link_libraries (${LIB_PROJECT} ${QT_LIBRARIES} ${OPENSSL_LIBRARIES})
target_link_libraries (${PROJECT} ${QT_LIBRARIES} ${OPENSSL_LIBRARIES} ${LIB_PROJECT} ${EXTRA_LIBS})


if (UNIX AND NOT APPLE)
    install (TARGETS ${PROJECT_NAME} DESTINATION bin)
    install (TARGETS ${LIB_PROJECT} DESTINATION lib${LIB_SUFFIX})
    install(FILES ${DESKTOP_FILE} DESTINATION share/applications)

    IF ( NOT_INSTALL_PROJECT_DOCS )
        message(STATUS "Don't install project documentation")
    ELSE ( NOT_INSTALL_PROJECT_DOCS )
        install (FILES ${DOCS} DESTINATION share/doc/${PROJECT}-${PROJECT_VERSION})
    ENDIF ( NOT_INSTALL_PROJECT_DOCS )

    foreach (ICON_SIZE ${ICON_SIZES})
        install (
            FILES icons/${ICON_SIZE}/${PROJECT_NAME}.png 
            DESTINATION share/icons/hicolor/${ICON_SIZE}/apps)
    endforeach ()

    install (
        FILES images/icons/${PROJECT}.svg
        DESTINATION ${ICON_DEST_PATH}/scalable/apps
    )
    foreach (LIB_HEADER ${LIB_HEADERS})
        install (
            FILES ${LIB_HEADER}
            DESTINATION include/basketpwd
        )
    endforeach()
endif (UNIX AND NOT APPLE)

