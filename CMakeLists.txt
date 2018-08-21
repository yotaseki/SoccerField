cmake_minimum_required(VERSION 2.8)
project(soccerField)

set(CMAKE_CXX_STANDARD 11)
set(EIGEN3_INCLUDE_DIR $ENV{EIGEN3_INCLUDE_DIR} CACHE PATH "Eigen installation dir(Default: $EIGEN_HOME)")
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(GLFW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS date_time system filesystem)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
include_directories(
	${EIGEN3_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIR}
	${Boost_INCLUDE_DIRS}
	${GLFW_INCLUDE_DIR}
	)
set(CMake_CXX_FLAGS "-g -Wall")

set(SoccerFieldLib
	main.cpp
	imgui.h
	imconfig.h
	imgui.cpp
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_impl_glfw.h
	imgui_impl_glfw.cpp
	stb_rect_pack.h
	stb_textedit.h
	stb_truetype.h  
	)
add_executable(soccerField
	${SoccerFieldLib}
)

target_link_libraries(soccerField
	${GLFW_LIBRARIES}
	${OPENGL_LIBRARIES}
	m
    ${OpenCV_LIBS}
	${Boost_LIBRARIES}
)

set(BlurLibs
	Blur/main.cpp
	)

add_executable(Blur
	${BlurLibs}
	)

target_link_libraries(Blur
	${OpenCV_LIBS}
	${Boost_LIBRARIES}
	)
