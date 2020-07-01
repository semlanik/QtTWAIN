set(TARGET QtTWAIN)
set(TARGET_EXPORT ${TARGET}Targets)
set(TARGET_CONFIG ${TARGET}Config)

set(TARGET_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET})
set(TARGET_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
set(TARGET_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
set(TARGET_BINDIR ${CMAKE_INSTALL_BINDIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Gui REQUIRED)

include(${QT_TWAIN_CMAKE_DIR}/GenerateQtHeaders.cmake)
include(${QT_TWAIN_CMAKE_DIR}/QtTWAINCommon.cmake)

file(GLOB SOURCES
    qtwaincontext.cpp
    qtwaineventfilter.cpp
    qtwainscanner.cpp
    qtwainscannermanager.cpp
    qtwainscanner_p.cpp
    dsm/qdsminterface.cpp)

file(GLOB HEADERS
    twain.h
    qtwaincontext_p.h
    qtwaineventfilter_p.h
    qtwain_p.h
    qtwainscanner_p.h
    dsm/qdsminterface.h)

file(GLOB PUBLIC_HEADER
    qtwainglobal.h
    qtwainscanner.h
    qtwainscannermanager.h)

generate_qt_headers(PUBLIC_HEADER ${PUBLIC_HEADER} COMPONENT ${TARGET})

add_library(${TARGET} SHARED ${SOURCES})

if(NOT DEFINED QT_QMAKE_EXECUTABLE)
    find_program(QT_QMAKE_EXECUTABLE "qmake")
    if(QT_QMAKE_EXECUTABLE STREQUAL QT_QMAKE_EXECUTABLE-NOTFOUND)
        message(FATAL_ERROR "Could not find qmake executable")
    endif()
endif()

extract_qt_variable(QT_HOST_DATA)

target_compile_definitions(${TARGET} PRIVATE QT_BUILD_TWAIN_LIB PUBLIC QT_TWAIN_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    QT_TWAIN_VERSION_MINOR=${PROJECT_VERSION_MINOR})

set_target_properties(${TARGET} PROPERTIES VERSION ${PROJECT_VERSION} PUBLIC_HEADER "${PUBLIC_HEADER};${GENERATED_PUBLIC_HEADER}" OUTPUT_NAME ${TARGET})
target_include_directories(${TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${QT_TWAIN_BINARY_DIR}/include/${TARGET}>
    $<INSTALL_INTERFACE:${TARGET_INCLUDE_DIR}>
)

target_compile_features(${TARGET} PUBLIC cxx_std_14
                                         cxx_auto_type
                                         cxx_decltype
                                         cxx_final
                                         cxx_override
                                         cxx_nullptr
                                         cxx_lambdas
                                         cxx_func_identifier)

target_link_libraries(${TARGET} PUBLIC Qt5::Core Qt5::Gui)

add_library(${QT_TWAIN_PROJECT}::${TARGET} ALIAS ${TARGET})

install(TARGETS ${TARGET}
    EXPORT ${TARGET_EXPORT} COMPONENT dev
    ARCHIVE DESTINATION ${TARGET_LIB_DIR} COMPONENT lib
    PUBLIC_HEADER DESTINATION ${TARGET_INCLUDE_DIR} COMPONENT dev
    LIBRARY DESTINATION ${TARGET_LIB_DIR} COMPONENT lib
    RUNTIME DESTINATION ${TARGET_BINDIR} COMPONENT lib)

install(EXPORT ${TARGET_EXPORT} NAMESPACE ${QT_TWAIN_PROJECT}:: FILE ${TARGET_EXPORT}.cmake DESTINATION ${TARGET_CMAKE_DIR} COMPONENT dev)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${TARGET_CONFIG}.cmake.in" "${QT_TWAIN_BINARY_DIR}/${TARGET_CONFIG}.cmake"
    INSTALL_DESTINATION "${TARGET_CMAKE_DIR}")

install(FILES "${QT_TWAIN_BINARY_DIR}/${TARGET_CONFIG}.cmake" DESTINATION "${TARGET_CMAKE_DIR}" COMPONENT dev)

export(TARGETS ${TARGET} NAMESPACE ${QT_TWAIN_PROJECT}:: FILE ${TARGET_EXPORT}.cmake)
