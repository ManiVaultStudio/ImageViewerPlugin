cmake_minimum_required(VERSION 3.12.0)

set(PROJECT "ImageViewerPlugin")

PROJECT(${PROJECT})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP" )
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG /NODEFAULTLIB:LIBCMT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

find_package(OpenGL REQUIRED)
find_package(Qt5 COMPONENTS Widgets WebEngineWidgets REQUIRED)
find_package(OpenMP)

set(PLUGIN
	src/Common.h
	src/ImageViewerPlugin.h
	src/ImageViewerPlugin.cpp
)

file(GLOB MODEL src/*Model.* src/*Layer.* src/*Dataset.* src/*Layer.* src/*Item.* src/Range.* src/*ProxyModel.* src/*Node.* src/*Renderable*.* src/Channel*.*)
file(GLOB RENDERING src/*Renderer.* src/*Actor.* src/*Prop.* src/*Shape.*)
file(GLOB WIDGETS src/*Widget.* src/*Color*.*)
file(GLOB UIS src/*.ui)
file(GLOB SHADERS src/*.glsl )
file(GLOB JSON src/*.json)
file(GLOB QRC src/*.qrc)

set(SOURCES ${PLUGIN} ${MODEL} ${RENDERING} ${WIDGETS})
set(AUX ${JSON} ${QRC})

source_group(Plugin FILES ${PLUGIN})
source_group(Model FILES ${MODEL})
source_group(Rendering FILES ${RENDERING})
source_group(Widgets FILES ${WIDGETS})
source_group(UI FILES ${UIS})
source_group(Shaders FILES ${SHADERS})
source_group(Aux FILES ${AUX})

# Note: src is included for ClustersLayerWidget.h
include_directories("$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/include/" src)

QT5_WRAP_UI(UI_HEADERS ${UIS})

add_library(${PROJECT} SHARED ${SOURCES} ${UIS} ${SHADERS} ${AUX})

target_compile_features(${PROJECT} PRIVATE cxx_std_17)

if(MSVC)
	set(LIB_SUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX}")
else()
	set(LIB_SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()

target_link_libraries(${PROJECT} ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT} Qt5::Widgets)
target_link_libraries(${PROJECT} Qt5::WebEngineWidgets)

target_link_libraries(${PROJECT} "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${LIB_SUFFIX}")
target_link_libraries(${PROJECT} "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}PointData${LIB_SUFFIX}")
target_link_libraries(${PROJECT} "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}ImageData${LIB_SUFFIX}")

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
	"$<TARGET_FILE:${PROJECT}>"
	"$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/Plugins/$<TARGET_FILE_NAME:${PROJECT}>"
)