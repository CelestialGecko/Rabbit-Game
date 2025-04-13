cmake_minimum_required(VERSION 3.10)
project(Burrow-Exodus VERSION 1.0)

# Set C plus plus stuff
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# sdl stuff
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gfc/sdl/include)
include_directories(${CMAKE_SOURCE_DIR}/gfc/gfc/include)

# pain
set(GFC_SOURCES
    gfc/gfc/Game.cpp
    gfc/gfc/GameApp.cpp
    gfc/gfc/Graphics.cpp
    gfc/gfc/SDL_collide.cpp
    gfc/gfc/Sound.cpp
    gfc/gfc/Sprite.cpp
    gfc/gfc/SpriteOval.cpp
    gfc/gfc/SpriteRect.cpp
    gfc/gfc/SpriteText.cpp
    gfc/gfc/stdafx.cpp
)

# GLOB sounds funky
file(GLOB GAME_SOURCES "game/game/*.cpp")

# executable
add_executable(${PROJECT_NAME} ${GAME_SOURCES} ${GFC_SOURCES})

# maths lib
target_link_libraries(${PROJECT_NAME} PRIVATE m)

# images and sound
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/game/game/images DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/game/game/sounds DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
