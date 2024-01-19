cmake_minimum_required(VERSION 3.1)
project(GCMake)
add_executable(GCMake src/GCMake.c src/cJSON.c src/cJSON.c)
target_include_directories(GCMake PUBLIC include)
