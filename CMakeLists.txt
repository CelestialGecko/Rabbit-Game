cmake_minimum_required(VERSION 3.10)
project(Burrow-Exodus VERSION 1.0)

# Set C plus plus stuff
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# need the sdl stuff
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf REQUIRED)

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

# directories needed
target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/gfc
    ${CMAKE_CURRENT_SOURCE_DIR}/game/game
)

# linking the libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2::SDL2
    SDL2::SDL2_image
    SDL2::SDL2_mixer
    SDL2::SDL2_ttf
)

# images and sound
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/game/game/images DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/game/game/sounds DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
