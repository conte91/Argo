FIND_PACKAGE( Boost COMPONENTS system thread REQUIRED )

ADD_EXECUTABLE( server
  server.cpp ValueUpdater.cpp JsonServer.cpp
)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(server ${Boost_LIBRARIES})
