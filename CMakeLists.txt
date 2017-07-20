cmake_minimum_required(VERSION 2.8)
project(cbot)

set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeFindModules )

SET(PROTOBUF_DEFS
	${CMAKE_CURRENT_SOURCE_DIR}/src/mumble/proto/mumble.proto
)
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_BINARY_DIR}
)

#openssl
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

#Protobuf
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

#curl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

file(GLOB PROTO_FILES ${PROTOBUF_DEFS})
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HS ${PROTO_FILES})
SET_SOURCE_FILES_PROPERTIES(${PROTO_SRCS} ${PROTO_HS} PROPERTIES GENERATED TRUE)#forces to generate them before compiling?

#source
# Hier können zusätzliche Quelldateien eingtragen werden, die dann automatisch dem Projekt hinzugefügt werden
SET(BASE_SRC
	${PROTO_SRCS}
	${PROTO_HS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/mumble/MumbleConnector.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/mumble/MumbleHeader.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SSLClientSocket.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Entities.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Bot.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/SimpleMumbleBot.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/extensions/http/HttpRelayBot.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/SimpleList.hpp
)

#generate/set executable
add_executable(cbot ${BASE_SRC})
set(EXECUTABLE_OUTPUT_PATH ./bin)

#linking
target_link_libraries(cbot ${OPENSSL_LIBRARIES} "-pthread" ${PROTOBUF_LIBRARY} ${CURL_LIBRARIES})
