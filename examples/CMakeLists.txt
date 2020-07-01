set(TARGET simplescan)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Quick Qml REQUIRED)
find_package(QtTWAINProject CONFIG REQUIRED COMPONENTS QtTWAIN)

file(GLOB SOURCES main.cpp
    scanengine.cpp)

add_executable(${TARGET} ${SOURCES} qml.qrc)

add_target_windeployqt(TARGET ${TARGET} QML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/qml)

target_link_libraries(${TARGET} PRIVATE examples_common QtTWAINProject::QtTWAIN Qt5::Quick Qt5::Qml)
