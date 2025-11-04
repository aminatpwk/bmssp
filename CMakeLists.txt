cmake_minimum_required(VERSION 3.31)
project(bmss)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(bmss main.cpp
        include/graphs/Directedgraph.h
        include/graphs/edge.h
        include/graphs/INode.h
        include/graphs_impl/Directedgraph.cpp
        include/graphs_impl/node.cpp
        include/graphs_impl/edge.cpp
)
