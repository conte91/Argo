FIND_PACKAGE( Boost COMPONENTS system thread date_time REQUIRED )

ADD_EXECUTABLE( baseStation
  baseStation.cpp ValueUpdater.cpp JsonServer.cpp Timer.cpp OnRequest.cpp JsonManager.cpp OnRequest.cpp BaseStations/DummyBS.cpp
)
ADD_EXECUTABLE( bigHugeServer bigHugeServer.cpp JsonServer.cpp JsonManager.cpp)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(".")
TARGET_LINK_LIBRARIES(baseStation ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(bigHugeServer ${Boost_LIBRARIES})
