cmake_minimum_required(VERSION 3.1)
project(cbot)

#set c++11
set (CMAKE_CXX_STANDARD 11)

#if additional cmake modules are needed put them in a ./CMakeFindModules folder
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeFindModules )

#set protobuf target
SET(PROTOBUF_DEFS
	${CMAKE_CURRENT_SOURCE_DIR}/src/mumble/proto/mumble.proto
)

#add include dir and binary dir (for protobuf headers)
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_BINARY_DIR}
)

#source
# add additional source files here
SET(BASE_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/io/Connector.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/mumble/MumbleConnector.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/mumble/MumbleHeader.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/io/SSLClientSocket.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/model/Entity.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/model/Channel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/model/User.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/bot/Bot.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/mumble/SimpleMumbleBot.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/mumble/MoveMumbleBot.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/audio/OpusAdapter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/model/SimpleList.hpp
)
SET(HTTP_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/src/mumble/HttpMumbleBot.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/extensions/http/HttpRelay.cpp
)


#openssl (for secured connections)
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

#Protobuf (mumble protocol)
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

#libconfig++ (for config parsing)
find_package(LibConfig++ REQUIRED)
include_directories(${LIBCONFIGPP_INCLUDE_DIRS})

#opus (voice codec)
find_package(Opus REQUIRED)
include_directories(${OPUS_INCLUDE_DIRS})

#mandatory libraries
LIST(APPEND DEPENDENCIES
 	${PROTOBUF_LIBRARY}
	${LIBCONFIGPP_LIBRARIES}
	${OPENSSL_LIBRARIES}
	${OPUS_LIBRARIES}
)


#optional libraries

#curl (for http connections. Extension.)
find_package(CURL)
if(${CURL_FOUND})
	include_directories(${CURL_INCLUDE_DIR})
	LIST(APPEND DEPENDENCIES ${CURL_LIBRARIES})
	LIST(APPEND BASE_SRC ${HTTP_SRC})
	add_definitions(-DHTTP_ENABLED)
endif(${CURL_FOUND})

#generate protobuf files and add to project
file(GLOB PROTO_FILES ${PROTOBUF_DEFS})
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HS ${PROTO_FILES})
SET_SOURCE_FILES_PROPERTIES(${PROTO_SRCS} ${PROTO_HS} PROPERTIES GENERATED TRUE)
LIST(APPEND BASE_SRC ${PROTO_SRCS} ${PROTO_HS})


#generate/set executable
add_executable(cbot ${BASE_SRC})
set(EXECUTABLE_OUTPUT_PATH ./bin)

#linking
target_link_libraries(cbot "-pthread" ${DEPENDENCIES})
