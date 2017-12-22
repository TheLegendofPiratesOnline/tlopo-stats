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
    src/net/rpcServer.cxx
    src/net/rpcConnection.cxx
    src/event/eventListener.cxx
    src/event/eventManager.cxx
    src/avatar/avatarManager.cxx
    src/collector/eventCollector.cxx
    src/collector/statCollectorBase.cxx
    src/collector/statCollector.cxx
    src/collector/incrementalStatCollector.cxx
    src/collector/statCollectorManager.cxx
    src/report/report.cxx
    src/report/periodicReport.cxx
    src/report/incrementalPeriodicReport.cxx
    src/report/dailyReport.cxx
    src/report/monthlyReport.cxx
    src/report/yearlyReport.cxx
    src/database/cache.cxx
    src/database/cachedDoIdMap.cxx
    src/database/cachedStatCollectorMap.cxx
    src/database/database.cxx
    src/database/dummyDatabase.cxx
    src/database/mongoDatabase.cxx
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

# Test:
find_package(PythonLibs)
find_package(PythonInterp)
if(PYTHONINTERP_FOUND)
    enable_testing()
	  if(PYTHON_VERSION_MAJOR EQUAL 2)
		    set(PYTHON2_EXECUTABLE "${PYTHON_EXECUTABLE}")
	  else()
		    set(PYTHON2_EXECUTABLE python2)
	  endif()

    add_test(av_mgr "${PYTHON2_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/test/test_av_mgr.py")
    add_test(rpc "${PYTHON2_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/test/test_rpc.py")
    add_test(collectors "${PYTHON2_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/test/test_collectors.py")
endif()
