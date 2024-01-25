########GCMake Generated Block DO NOT EDIT########
set(GCMAKE_FILES  "./src/glob.c" "./src/GCMake.c" "./src/cJSON.c")
##################################################
cmake_minimum_required(VERSION 3.2)
project(GCMake)
add_executable(GCMake ${GCMAKE_FILES})
target_include_directories(GCMake PRIVATE include)
set_target_properties(GCMake PROPERTIES OUTPUT_NAME "gcm")
