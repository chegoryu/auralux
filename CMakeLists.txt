cmake_minimum_required(VERSION 3.19)
project(Auralux)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH /usr/local/opt/qt@5/lib/cmake/Qt5;)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_VERSION 5)
set(REQUIRED_LIBS Core)
set(REQUIRED_LIBS_QUALIFIED Qt5::Core)

if (NOT CMAKE_PREFIX_PATH)
    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\" or -DCMAKE_PREFIX_PATH=/usr/include/{host}/qt{version}/ on Ubuntu)")
endif ()

find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)

include_directories(.)

set(LIBRARY_GAME
    library/game/default_players.cpp
    library/game/default_players.h
    library/game/game.cpp
    library/game/game.h
    library/game/game_logger.cpp
    library/game/game_logger.h
    library/game/game_map.cpp
    library/game/game_map.h
    library/game/game_result.cpp
    library/game/game_result.h
    library/game/game_state.cpp
    library/game/game_state.h
    library/game/game_visualizer.cpp
    library/game/game_visualizer.h
    library/game/player.cpp
    library/game/player.h
    library/game/text_player.cpp
    library/game/text_player.h
)
set(LIBRARY_TESTLIB library/testlib/testlib.h)

configure_file(${CMAKE_SOURCE_DIR}/library/maps/small_map.txt ${CMAKE_CURRENT_BINARY_DIR})
configure_file(${CMAKE_SOURCE_DIR}/library/maps/big_map.txt ${CMAKE_CURRENT_BINARY_DIR})

# Test binary
add_executable(test bin/test/main.cpp ${LIBRARY_GAME})
target_link_libraries(test ${REQUIRED_LIBS_QUALIFIED})

# Ejudge interactor
add_executable(ejudge_interactor bin/ejudge_interactor/main.cpp ${LIBRARY_GAME} ${LIBRARY_TESTLIB})

# Game runner
add_executable(game_runner bin/game_runner/main.cpp ${LIBRARY_GAME})
target_link_libraries(test ${REQUIRED_LIBS_QUALIFIED})