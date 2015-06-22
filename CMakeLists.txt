cmake_minimum_required(VERSION 2.8.3)
project(youbot_mannequin)


## check required prerequisites
#SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/external/cmake_modules")
#SET(Boost_USE_STATIC_LIBS ON)
#SET(Boost_ADDITIONAL_VERSIONS "1.39" "1.39.0" "1.40" "1.40.0" "1.41" "1.41.0" "1.42" "1.42.0" "1.43" "1.43.0" "1.44" "1.44.0" "1.45" "1.45.0" "1.46" "1.46.0")
find_package(Boost REQUIRED COMPONENTS thread date_time filesystem system)	
find_package(catkin)
find_package(Curses)
find_package(Threads)
find_package(youbot_driver)

#OPTION(USE_ROS "Enable ROS as compile tool" ON)

#SET(ROS_ROOT_PATH $ENV{ROS_ROOT})
#IF(DEFINED ROS_ROOT_PATH AND USE_ROS MATCHES ON)
#	include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)
#    rosbuild_init()
#    rosbuild_find_ros_package(youbot_driver)
#    SET(OODL_YOUBOT_LIBRARIES YouBotDriver)
#    SET(OODL_YOUBOT_CONFIG_DIR ${youbot_driver_PACKAGE_PATH}/config/)
#ELSE(DEFINED ROS_ROOT_PATH AND USE_ROS MATCHES ON)
#    FIND_PACKAGE(OODL_YouBot REQUIRED)
#ENDIF(DEFINED ROS_ROOT_PATH AND USE_ROS MATCHES ON)

## preprocessor definitions
#ADD_DEFINITIONS(-DYOUBOT_CONFIGURATIONS_DIR="${OODL_YOUBOT_CONFIG_DIR}/")
#ADD_DEFINITIONS(-DCONFIG_DIR="${PROJECT_SOURCE_DIR}/config")

## build parameters and paths
SET(CMAKE_BUILD_TYPE DEBUG) #enable debug mode (e.g. for embedded gdb in eclipse )
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/ CACHE PATH "Configure the executable output path.")
#SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib/ CACHE PATH "Configure the library output path.")

## Compile and link
#ADD_EXECUTABLE(youbot_mannequin_mode
#  src/main.cpp
#)

#TARGET_LINK_LIBRARIES(youbot_mannequin_mode
#	${OODL_YOUBOT_LIBRARIES}
#	${Boost_LIBRARIES}
#	pthread
#	ncurses
#)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES apc_cortex
#  CATKIN_DEPENDS apc_robot roscpp rospy apc_msgs
#  DEPENDS system_lib
)

##TODO: use ${youbot_driver} to find youbot configuration directory
add_definitions(-DYOUBOT_CONFIGURATIONS_DIR="/home/youbot/hydro_wksp/src/youbot_driver/config/")
add_definitions(-DCONFIG_DIR="${PROJECT_SOURCE_DIR}/config")

include_directories(include)
include_directories(
  ${catkin_INCLUDE_DIRS}
  ${OODL_YOUBOT_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	~/youbot_driver
	~/youbot_driver/soem/src
)

add_executable(mannequin_mode src/main.cpp)
target_link_libraries(	mannequin_mode      
												${catkin_LIBRARIES}
												${OODL_YOUBOT_LIBRARIES}
												${Boost_LIBRARIES}
												${CMAKE_THREAD_LIBS_INIT}
												${CURSES_LIBRARY}
												pthread
												ncurses
												YouBotDriver
										 )

#INSTALL(TARGETS youbot_mannequin_mode RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
