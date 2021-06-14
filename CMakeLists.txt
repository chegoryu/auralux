cmake_minimum_required(VERSION 3.19)
project(Auralux)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)
# set(CMAKE_AUTOUIC ON)

# set(QT_VERSION 5)
# set(REQUIRED_LIBS Core)
# set(REQUIRED_LIBS_QUALIFIED Qt5::Core)

include_directories(.)


SET(LIBRARY_GAME library/game/game.cpp library/game/game.h library/game/player.cpp library/game/player.h library/game/text_player.h library/game/text_player.cpp library/game/default_players.h library/game/default_players.cpp library/game/game_state.cpp library/game/game_state.h)
SET(LIBRARY_TESTLIB library/testlib/testlib.h)

add_executable(test bin/test/main.cpp ${LIBRARY_GAME})
add_executable(ejudge_interactor bin/ejudge_interactor/main.cpp ${LIBRARY_GAME} ${LIBRARY_TESTLIB})

# if (NOT CMAKE_PREFIX_PATH)
#     message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
#             "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
# endif ()

# find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
# target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})
