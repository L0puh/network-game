cmake_minimum_required(VERSION 3.4)
project(network_game)


add_executable(server src/server.cpp net/net_server.cpp net/net_game.cpp net/net_client.cpp)
add_executable(client net/net_client.cpp src/client.cpp net/net_game.cpp net/net_server.cpp)
