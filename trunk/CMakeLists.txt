cmake_minimum_required(VERSION 2.8)

project(SoftwareUpdate)

find_package(wxWidgets COMPONENTS base core net REQUIRED)

set(SRCS
	SoftwareUpdateApp.cpp
	SoftwareUpdateApp.h
	UpdateChecker.h
	UpdateChecker.cpp
	)

include(${wxWidgets_USE_FILE})	
	
add_executable(${CMAKE_PROJECT_NAME} WIN32 ${SRCS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${wxWidgets_LIBRARIES})
