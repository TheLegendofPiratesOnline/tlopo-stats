cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
project(tlopostats)

include(FindPackageHandleStandardArgs)

# Find Jansson
find_path(JANSSON_INCLUDE_DIR NAMES jansson.h)
find_library(JANSSON_LIBRARY NAMES jansson)

find_package_handle_standard_args(Jansson DEFAULT_MSG JANSSON_LIBRARY JANSSON_INCLUDE_DIR)

# Find MongoDB
find_path(MONGO_INCLUDE_DIR mongo/client/dbclient.h
	  /usr/include
	  /usr/local/include
	  /opt/local/include
)

find_library(MONGO_LIBRARIES NAMES mongoclient libmongoclient
    PATHS
    /usr/lib
    /usr/local/lib
    /opt/local/lib
)

find_package_handle_standard_args(MongoDB DEFAULT_MSG MONGO_LIBRARIES MONGO_INCLUDE_DIR)

find_package(Boost COMPONENTS system REQUIRED)

set (SRC_FILES
    src/main.cxx
    src/net/util.cxx
    src/net/udpReceiver.cxx
    src/net/jsonUdpReceiver.cxx
    src/event/eventListener.cxx
    src/event/eventManager.cxx
    src/avatar/avatarManager.cxx
    src/collector/eventCollector.cxx
    src/report/report.cxx
    src/report/periodicReport.cxx
    src/report/incrementalPeriodicReport.cxx
    src/report/dailyReport.cxx
    src/report/monthlyReport.cxx
    src/report/yearlyReport.cxx
)

include_directories(${MONGO_INCLUDE_DIR})
include_directories(${JANSSON_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/src)

add_definitions("-pthread -std=c++11 -ggdb")

add_executable(tlopostats ${SRC_FILES})
target_link_libraries(tlopostats
    pthread
    ${JANSSON_LIBRARY}
    ${MONGO_LIBRARIES}
    ${Boost_LIBRARIES}
)
