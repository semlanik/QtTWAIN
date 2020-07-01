function(extract_qt_variable VARIABLE)
    if(NOT DEFINED QT_QMAKE_EXECUTABLE)
        find_program(QT_QMAKE_EXECUTABLE "qmake")
        if(QT_QMAKE_EXECUTABLE STREQUAL QT_QMAKE_EXECUTABLE-NOTFOUND)
            message(FATAL_ERROR "Could not find qmake executable")
        endif()
    endif()
    execute_process(
        COMMAND ${QT_QMAKE_EXECUTABLE} -query ${VARIABLE}
        OUTPUT_VARIABLE ${VARIABLE}
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(${VARIABLE} ${${VARIABLE}} PARENT_SCOPE)
endfunction()

function(add_target_windeployqt)
if(WIN32)
    set(options)
    set(oneValueArgs QML_DIR TARGET)
    set(multiValueArgs)
    cmake_parse_arguments(add_target_windeployqt "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    find_program(WINDEPLOYQT_EXECUTABLE "windeployqt" PATHS ${PATH})
    if(WINDEPLOYQT_EXECUTABLE STREQUAL WINDEPLOYQT_EXECUTABLE-NOTFOUND)
        message(WARNING "windeployqt is not found in specified PATH! Please, copy dependencies manually")
    else()
        if(DEFINED add_target_windeployqt_QML_DIR)
            set(QML_DIR --qmldir ${add_target_windeployqt_QML_DIR})
        endif()
        add_custom_command(TARGET ${add_target_windeployqt_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:QtTWAIN> $<TARGET_FILE_DIR:${add_target_windeployqt_TARGET}>
            COMMAND ${WINDEPLOYQT_EXECUTABLE} ${QML_DIR} $<TARGET_FILE_DIR:${add_target_windeployqt_TARGET}>)
    endif()
endif()
endfunction()
