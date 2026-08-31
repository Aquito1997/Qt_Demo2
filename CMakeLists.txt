cmake_minimum_required(VERSION 3.15)
project(QtWidgetApplication VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(ALL_CONFIGS
    "Debug"
    "Release"
    "RelWithDebInfo"
    "MinSizeRel"

    # 自定义配置类型
    "Default"

    # "All"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_PREFIX_PATH /opt/Qt_5_10/5.10.0/gcc_64)
    set(Qt5_DIR "/opt/Qt_5_10/5.10.0/gcc_64/lib/cmake/Qt5")
    find_package(ZLIB REQUIRED)
    find_package(PNG REQUIRED)
    find_package(BZip2 REQUIRED)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_PREFIX_PATH D:\\Program\\Qt_5_10\\5.10.0\\mingw53_32)
endif()

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets Gui Core OpenGL)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Gui Core OpenGL)

set(Freetype_ROOT "${CMAKE_SOURCE_DIR}/lib/freetype")
find_package(Freetype REQUIRED)

# 生成 ui_xxx.h 到uiHeader下
file(MAKE_DIRECTORY src/uiHeader)
set(QtUiHeader ${CMAKE_CURRENT_SOURCE_DIR}/src/uiHeader)
set(Qt5_Headers src/UI/mainwindow.ui)
get_target_property(Qt5_uic_location Qt5::uic IMPORTED_LOCATION)

if(NOT Qt5_uic_location)
    find_program(UIC_EXECUTABLE uic HINTS "${Qt5_DIR}/../../../bin" REQUIRED)
    set(Qt5_uic_location "${UIC_EXECUTABLE}")
else()
    message(STATUS "uic location: ${Qt5_uic_location}")
endif()

foreach(ui_file ${Qt5_Headers})
    get_filename_component(ui_name ${ui_file} NAME_WE)
    execute_process(
        COMMAND ${Qt5_uic_location}
        ${CMAKE_CURRENT_SOURCE_DIR}/${ui_file}
        -o ${QtUiHeader}/ui_${ui_name}.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE uic_result
        OUTPUT_VARIABLE uic_output
        ERROR_VARIABLE uic_error
    )

    if(NOT uic_result EQUAL 0)
        message(WARNING "Failed to generate ui_${ui_name}.h: ${uic_error}")
    else()
        message(STATUS "Generated: ${QtUiHeader}/ui_${ui_name}.h")
    endif()
endforeach()

include_directories(${QtUiHeader})
include_directories(${CMAKE_SOURCE_DIR}/lib/freetype/include/freetype2)
include_directories(${CMAKE_SOURCE_DIR}/src/UI)

set(PROJECT_SOURCES
    src/UI/mainwindow.h
    src/UI/mainwindow.cpp
    src/UI/OpenGLWidget.h
    src/UI/OpenGLWidget.cpp

    src/BE/main.cpp

    src/BE/LoadText.h
    src/BE/LoadText.cpp
    src/BE/pick.h
    src/BE/pick.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::OpenGL)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Freetype::Freetype)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE
        freetype
        png
        z
        bz2
        brotlidec
        brotlicommon
    )
endif()

# target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL)
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${PROJECT_NAME})
endif()

# set_target_properties(${PROJECT_NAME} PROPERTIES
# ${BUNDLE_ID_OPTION}
# MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
# MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
# MACOSX_BUNDLE TRUE
# WIN32_EXECUTABLE TRUE
# )
set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE false # 开启qDebug
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/"

    # RUNTIME_OUTPUT_DIRECTORY_DEFAULT "${CMAKE_BINARY_DIR}/"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # set(QT_BIN_DIR "D:/Program/Qt_5_10/5.10.0/mingw53_32/bin")
    set(QT_BIN_DIR "D:\\Program\\Qt_5_10_1\\5.10.1\\mingw53_32\\bin")

    set(EXE_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set(QT_DLLS
        Qt5Core.dll
        Qt5Gui.dll
        Qt5Widgets.dll
        Qt5Cored.dll
        Qt5Guid.dll
        Qt5Widgetsd.dll
        Qt5OpenGLd.dll

        # Qt5Network.dll
        # Qt5Sql.dll
    )

    foreach(DLL ${QT_DLLS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${QT_BIN_DIR}/${DLL}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${DLL}"
            COMMENT "Copying ${DLL} to executable directory"
        )
    endforeach()

    set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib/freetype/lib/")
    set(FREETYPE_DLLS
        libfreetyped.a)

    foreach(DLL ${FREETYPE_DLLS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${LIB_DIR}/${DLL}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${DLL}"
            COMMENT "Copying ${DLL} to executable directory"
        )
    endforeach()

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/res/UbuntuMono-Regular.ttf"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/UbuntuMono-Regular.ttf"
        COMMENT "Copying UbuntuMono-Regular.ttf to executable directory"
    )
endif()

# ----------------------
# 为每个配置创建目标
foreach(CONFIG ${ALL_CONFIGS})
    # 创建构建目录
    set(BUILD_DIR "${CMAKE_BINARY_DIR}/build-${CONFIG}")

    # 创建自定义目标
    add_custom_target(build-${CONFIG}
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${BUILD_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${BUILD_DIR}"
        COMMAND ${CMAKE_COMMAND} -S "${CMAKE_BINARY_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=${CONFIG}
        COMMAND ${CMAKE_COMMAND} --build "${BUILD_DIR}"
        COMMENT "Building ${CONFIG} configuration"
    )
endforeach()

# 创建构建所有配置的目标
add_custom_target(build-all
    DEPENDS build-Debug build-Release build-RelWithDebInfo build-MinSizeRel
    COMMENT "Building all configurations"
)
