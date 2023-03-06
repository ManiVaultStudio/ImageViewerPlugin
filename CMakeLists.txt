cmake_minimum_required(VERSION 3.12.0)

set(PROJECT "ImageViewerPlugin")

PROJECT(${PROJECT})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

find_package(OpenGL REQUIRED)
find_package(Qt6 COMPONENTS Widgets WebEngineWidgets OpenGL OpenGLWidgets REQUIRED)
find_package(OpenMP)
# Normalize the incoming install path
file(TO_CMAKE_PATH $ENV{HDPS_INSTALL_DIR} INSTALL_DIR)

set(PLUGIN
    src/ImageViewerPlugin.h
    src/ImageViewerPlugin.cpp
)

set(MODEL
    src/Layer.h
    src/Layer.cpp
    src/LayersModel.h
    src/LayersModel.cpp
    src/Renderable.h
    src/Renderable.cpp
)

set(RENDERING
    src/ImageProp.h
    src/ImageProp.cpp
    src/LayersRenderer.h
    src/LayersRenderer.cpp
    src/Prop.h
    src/Prop.cpp
    src/QuadShape.h
    src/QuadShape.cpp
    src/SelectionProp.h
    src/SelectionProp.cpp
    src/SelectionToolProp.h
    src/SelectionToolProp.cpp
    src/Shape.h
    src/Shape.cpp
)

set(WIDGETS
    src/ImageViewerWidget.h
    src/ImageViewerWidget.cpp
)

set(DIALOGS
    src/ConvertToImagesDatasetDialog.h
    src/ConvertToImagesDatasetDialog.cpp
    src/ExportToImageDialog.h
    src/ExportToImageDialog.cpp
)

set(ACTIONS
    src/GeneralAction.h
    src/GeneralAction.cpp
    src/GlobalViewSettingsAction.h
    src/GlobalViewSettingsAction.cpp
    src/ImageSettingsAction.h
    src/ImageSettingsAction.cpp
    src/LayersAction.h
    src/LayersAction.cpp
    src/MainToolbarAction.h
    src/MainToolbarAction.cpp
    src/MiscellaneousAction.h
    src/MiscellaneousAction.cpp
    src/PositionAction.h
    src/PositionAction.cpp
    src/ScalarChannelAction.h
    src/ScalarChannelAction.cpp
    src/SelectionAction.h
    src/SelectionAction.cpp
    src/SettingsAction.h
    src/SettingsAction.cpp
    src/SubsetAction.h
    src/SubsetAction.cpp
    src/ZoomAction.h
    src/ZoomAction.cpp
    src/ZoomToolbarAction.h
    src/ZoomToolbarAction.cpp
)

set(SHADERS
    res/shaders/ImageFragment.glsl
    res/shaders/ImageVertex.glsl
    res/shaders/SelectionFragment.glsl
    res/shaders/SelectionVertex.glsl
    res/shaders/SelectionToolFragment.glsl
    res/shaders/SelectionToolVertex.glsl
    res/shaders/SelectionToolOffScreenFragment.glsl
    res/shaders/SelectionToolOffScreenVertex.glsl
)

set(JSON
    src/ImageViewerPlugin.json
)

set(QRC
    res/resources.qrc
)

set(SOURCES ${PLUGIN} ${MODEL} ${RENDERING} ${WIDGETS} ${DIALOGS} ${ACTIONS})
set(AUX ${JSON} ${QRC})

source_group(Plugin FILES ${PLUGIN})
source_group(Model FILES ${MODEL})
source_group(Rendering FILES ${RENDERING})
source_group(Widgets FILES ${WIDGETS})
source_group(Dialogs FILES ${DIALOGS})
source_group(Actions FILES ${ACTIONS})
source_group(Shaders FILES ${SHADERS})
source_group(Aux FILES ${AUX})
source_group(Resources FILES res/resources.qrc)

QT6_WRAP_UI(UI_HEADERS ${UIS})
QT6_ADD_RESOURCES(RESOURCE_FILES res/resources.qrc)

add_library(${PROJECT} SHARED ${SOURCES} ${UIS} ${SHADERS} ${AUX} ${RESOURCE_FILES})

# Note: src is included for ClustersLayerWidget.h
target_include_directories(${PROJECT} PRIVATE src)
target_include_directories(${PROJECT} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${PROJECT} PRIVATE cxx_std_17)

if(MSVC)
    set(LIB_SUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX}")
else()
    set(LIB_SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()

target_link_libraries(${PROJECT} PRIVATE ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT} PRIVATE Qt6::Widgets)
target_link_libraries(${PROJECT} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${PROJECT} PRIVATE Qt6::OpenGL)
target_link_libraries(${PROJECT} PRIVATE Qt6::OpenGLWidgets)

target_link_libraries(${PROJECT} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${LIB_SUFFIX}")
target_link_libraries(${PROJECT} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}PointData${LIB_SUFFIX}")
target_link_libraries(${PROJECT} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}ClusterData${LIB_SUFFIX}")
target_link_libraries(${PROJECT} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}ImageData${LIB_SUFFIX}")

install(TARGETS ${PROJECT}
   RUNTIME DESTINATION Plugins COMPONENT SHAREDLIB
   LIBRARY DESTINATION Plugins COMPONENT LINKLIB
   ARCHIVE DESTINATION lib COMPONENT LINKLIB
)

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}"
    --install ${CMAKE_CURRENT_BINARY_DIR}
    --config $<CONFIGURATION>
    --prefix ${INSTALL_DIR}/$<CONFIGURATION>
    --verbose
)

# Automatically set the debug environment (command + working directory) for MSVC in debug mode
set_property(TARGET ${PROJECT} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug,${INSTALL_DIR}/release>)
set_property(TARGET ${PROJECT} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug/HDPS.exe,${INSTALL_DIR}/release/HDPS.exe>)
