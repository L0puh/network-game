cmake_minimum_required(VERSION 3.4)
project(network_game)


add_executable(server server.cpp net_server.cpp net_game.cpp net_client.cpp)
add_executable(client net_client.cpp client.cpp net_game.cpp net_server.cpp)
