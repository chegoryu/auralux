cmake_minimum_required(VERSION 3.19)
project(Auralux)

set(CMAKE_CXX_STANDARD 17)
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

set(GAME_MAPS
    data/maps/small_map.txt
    data/maps/big_map.txt
    data/maps/symmetrical_big_map.txt

    data/maps/ffa_4_players_map_01.txt
    data/maps/ffa_4_players_map_02.txt
    data/maps/ffa_4_players_map_03.txt

    data/maps/pvp_map_01.txt
    data/maps/pvp_map_02.txt
    data/maps/pvp_map_03.txt
    data/maps/pvp_map_04.txt
    data/maps/pvp_map_05.txt
    data/maps/pvp_map_06.txt
    data/maps/pvp_map_07.txt
    data/maps/pvp_map_08.txt
    data/maps/pvp_map_09.txt
)
foreach(GAME_MAP IN LISTS GAME_MAPS)
    configure_file(${CMAKE_SOURCE_DIR}/${GAME_MAP} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

set(GAME_RUNNER_CONFIGS data/game_runner_configs/example_runner_config.txt)
foreach(GAME_RUNNER_CONFIG IN LISTS GAME_RUNNER_CONFIGS)
    configure_file(${CMAKE_SOURCE_DIR}/${GAME_RUNNER_CONFIG} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

set(TOURNAMENT_RUNNER_CONFIGS data/tournament_runner_configs/example_tournament_config.txt)
foreach(TOURNAMENT_RUNNER_CONFIG IN LISTS TOURNAMENT_RUNNER_CONFIGS)
    configure_file(${CMAKE_SOURCE_DIR}/${TOURNAMENT_RUNNER_CONFIG} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

# Test binary
add_executable(test_bin bin/test/main.cpp ${LIBRARY_GAME})
target_link_libraries(test_bin ${REQUIRED_LIBS_QUALIFIED})

# Ejudge interactor
add_executable(ejudge_interactor bin/ejudge_interactor/main.cpp ${LIBRARY_GAME} ${LIBRARY_TESTLIB})

# Game runner
add_executable(game_runner bin/game_runner/main.cpp ${LIBRARY_GAME})
target_link_libraries(game_runner ${REQUIRED_LIBS_QUALIFIED})

# Tournament runner
add_executable(tournament_runner bin/tournament_runner/main.cpp ${LIBRARY_GAME})
target_link_libraries(tournament_runner ${REQUIRED_LIBS_QUALIFIED})

# Example player
add_executable(example_player bin/example_player/main.cpp)

