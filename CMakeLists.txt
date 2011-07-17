cmake_minimum_required (VERSION 2.6)

if (NOT CMAKE_INSTALL_PREFIX)
	set (CMAKE_INSTALL_PREFIX /usr/local)
endif ()

#set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/tools/cmake/modules ${CMAKE_MODULE_PATH})
set (PROJECT basketpwd)
set (PROJECT_VERSION 0.4.4)
set (HEADERS 
	aboutdialog.h
	src/basketutils.h
	src/changepassword.h
	src/mainwindow.h
	src/settingsdialog.h
	model/basketbaseitem.h
	model/basketmodel.h)
set (SOURCES
	aboutdialog.cpp
	src/basketutils.cpp
	src/changepassword.cpp
	src/main.cpp
	src/mainwindow.cpp
	src/settingsdialog.cpp
	model/basketbaseitem.cpp
	model/basketmodel.cpp)
set (FORMS
	aboutdialog.ui
	ui/mainwindow.ui
	ui/changepassword.ui
	ui/settingsdialog.ui)

set (LANG_PATH l10n)
set (LANGUAGES en)
set (RESOURCES icon-images.qrc)
set (TRANSLATIONS_BINARY qt_ru.qm)
set (DESKTOP_FILE tools/${PROJECT}.desktop)
set (ICON_FILE images/${PROJECT}.png)
set (DOCS ChangeLog.txt README)

set (QT_USE_QTXML TRUE)
project (${PROJECT})

configure_file (
	"config.h.in"
	"config.h"
	)
include_directories (model src ${CMAKE_CURRENT_BINARY_DIR})

find_package (Qt4 REQUIRED)
#find_package (LibGcrypt REQUIRED)
find_package (OpenSSL REQUIRED)
include (${QT_USE_FILE})
qt4_add_resources (QRC_SOURCES ${RESOURCES})
qt4_wrap_cpp (MOC_SOURCES ${HEADERS})
qt4_wrap_ui (UI_SOURCES ${FORMS})
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

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES})
source_group ("Forms Files" FILES ${UI_SOURCES})
source_group ("Generated Files" FILES ${MOC_SOURCES})
source_group ("Resource Files" FILES ${QRC_SOURCES})

add_executable (${PROJECT} ${HEADERS} ${SOURCES} ${UI_SOURCES} ${MOC_SOURCES} ${QRC_SOURCES} ${TRANSLATIONS})

target_link_libraries (${PROJECT} ${QT_LIBRARIES} ${OPENSSL_LIBRARIES})
install (TARGETS ${PROJECT} DESTINATION bin)
install (FILES ${DESKTOP_FILE} DESTINATION share/applications)
install (FILES ${ICON_FILE} DESTINATION share/pixmaps)
install (FILES ${DOCS} DESTINATION share/doc/${PROJECT}-${PROJECT_VERSION})
