cmake_minimum_required(VERSION 3.12.0)

option(MV_UNITY_BUILD "Combine target source files into batches for faster compilation" OFF)

# -----------------------------------------------------------------------------
# ImageViewer Plugin
# -----------------------------------------------------------------------------
set(PROJECT "ImageViewerPlugin")
PROJECT(${PROJECT})

# -----------------------------------------------------------------------------
# CMake Options
# -----------------------------------------------------------------------------
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

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------
find_package(OpenGL REQUIRED)
find_package(Qt6 COMPONENTS Widgets WebEngineWidgets OpenGL OpenGLWidgets REQUIRED)
find_package(ManiVault COMPONENTS Core PointData ClusterData ImageData CONFIG)
find_package(OpenMP)

# -----------------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------------
set(PLUGIN
    src/ImageViewerPlugin.h
    src/ImageViewerPlugin.cpp
)

set(PLUGIN_MOC_HEADERS
    src/ImageViewerPlugin.h
)

set(MODEL
    src/Layer.h
    src/Layer.cpp
    src/LayersModel.h
    src/LayersModel.cpp
    src/LayersFilterModel.h
    src/LayersFilterModel.cpp
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
    src/ViewSettingsAction.h
    src/ViewSettingsAction.cpp
    src/EditLayersAction.h
    src/EditLayersAction.cpp
    src/EditLayerAction.h
    src/EditLayerAction.cpp
    src/SettingsAction.h
    src/SettingsAction.cpp
)

set(LAYER_ACTIONS
    src/GeneralAction.h
    src/GeneralAction.cpp
    src/ImageSettingsAction.h
    src/ImageSettingsAction.cpp
    src/MiscellaneousAction.h
    src/MiscellaneousAction.cpp
    src/PositionAction.h
    src/PositionAction.cpp
    src/ScalarChannelAction.h
    src/ScalarChannelAction.cpp
    src/SelectionAction.h
    src/SelectionAction.cpp
    src/SubsetAction.h
    src/SubsetAction.cpp
)

set(TOOLBAR_ACTIONS
    src/SelectionToolbarAction.h
    src/SelectionToolbarAction.cpp
    src/InteractionToolbarAction.h
    src/InteractionToolbarAction.cpp
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
    res/resources_imgageviewer.qrc
)

set(SOURCES ${PLUGIN} ${MODEL} ${RENDERING} ${WIDGETS} ${DIALOGS} ${ACTIONS} ${LAYER_ACTIONS} ${TOOLBAR_ACTIONS})
set(AUX ${JSON} ${QRC})

source_group(Plugin FILES ${PLUGIN})
source_group(Model FILES ${MODEL})
source_group(Rendering FILES ${RENDERING})
source_group(Widgets FILES ${WIDGETS})
source_group(Dialogs FILES ${DIALOGS})
source_group(Actions FILES ${ACTIONS})
source_group(Actions\\Layer FILES ${LAYER_ACTIONS})
source_group(Actions\\Toolbars FILES ${TOOLBAR_ACTIONS})
source_group(Shaders FILES ${SHADERS})
source_group(Aux FILES ${AUX})
source_group(Resources FILES ${QRC})

QT6_WRAP_UI(UI_HEADERS ${UIS})
QT6_ADD_RESOURCES(RESOURCE_FILES ${QRC})

# -----------------------------------------------------------------------------
# CMake Target
# -----------------------------------------------------------------------------
add_library(${PROJECT} SHARED ${SOURCES} ${UIS} ${SHADERS} ${AUX} ${RESOURCE_FILES})

# -----------------------------------------------------------------------------
# Target include directories
# -----------------------------------------------------------------------------
# Note: src is included for ClustersLayerWidget.h
target_include_directories(${PROJECT} PRIVATE src)
target_include_directories(${PROJECT} PRIVATE "${ManiVault_INCLUDE_DIR}")

# -----------------------------------------------------------------------------
# Target properties
# -----------------------------------------------------------------------------
target_compile_features(${PROJECT} PRIVATE cxx_std_17)

if(MV_UNITY_BUILD)
    set_target_properties(${PROJECT} PROPERTIES UNITY_BUILD ON)
endif()

# -----------------------------------------------------------------------------
# Target library linking
# -----------------------------------------------------------------------------
target_link_libraries(${PROJECT} PRIVATE ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT} PRIVATE Qt6::Widgets)
target_link_libraries(${PROJECT} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${PROJECT} PRIVATE Qt6::OpenGL)
target_link_libraries(${PROJECT} PRIVATE Qt6::OpenGLWidgets)

target_link_libraries(${PROJECT} PRIVATE ManiVault::Core)
target_link_libraries(${PROJECT} PRIVATE ManiVault::PointData)
target_link_libraries(${PROJECT} PRIVATE ManiVault::ClusterData)
target_link_libraries(${PROJECT} PRIVATE ManiVault::ImageData)

# -----------------------------------------------------------------------------
# Target installation
# -----------------------------------------------------------------------------
install(TARGETS ${PROJECT}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
)

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}"
    --install ${CMAKE_CURRENT_BINARY_DIR}
    --config $<CONFIGURATION>
    --prefix ${ManiVault_INSTALL_DIR}/$<CONFIGURATION>
    --verbose
)

# -----------------------------------------------------------------------------
# Miscellaneous
# -----------------------------------------------------------------------------
# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${PROJECT} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${ManiVault_INSTALL_DIR}/debug,${ManiVault_INSTALL_DIR}/release>)
    set_property(TARGET ${PROJECT} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,"${ManiVault_INSTALL_DIR}/debug/ManiVault Studio.exe","${ManiVault_INSTALL_DIR}/release/ManiVault Studio.exe">)
endif()
