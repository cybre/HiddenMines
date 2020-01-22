cmake_minimum_required(VERSION 3.14)
project(OSSeminarski C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

add_executable(server server/server.h server/server.c server/player-loop.c server/lobby-loop.c player.h list.h list.c util.h parser.c)
add_executable(client client/client.c player.h common.h util.h client/client.h client/communication-loop.c client/chat-loop.c client/game-loop.c client/ui.h client/nuklear/nuklearapp.c parser.c)

if (WIN32)
    target_link_libraries(server wsock32 ws2_32)
    target_link_libraries(client wsock32 ws2_32 user32 gdi32 Msimg32)
endif()