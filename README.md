# GCMake
A simple CMakeLists.txt generator that adds glob capabilities without removing performance.

# Bulding
To build use typical cmake build commands (e.g. mkdir build, cd build, cmake ..).
Note: on windows you must use gcc or somehow have access to the glibc libraries (this is going to change in the future).

# Usage
First, create your CMake configuration and then save it as GCMakeLists.txt (This is what would normally go in CMakeLists.txt).
Then, make a GCMake.json file in the same directory, this is were the configuration for glob goes. There are currently two options: 
* "files" - This is a list of string patterns that specify what files GCMake should include in the generated CMakeLists.txt
* "dirs" - This is a list of string patterns that specify what sub-directories GCMake can search when looking for files
Here is an example configuration from this project:
```CMake
cmake_minimum_required(VERSION 3.1)
project(MyProject)
add_executable(MyProject ${GCMAKE_FILES})
target_include_directories(GCMake PRIVATE include)
```
Notice the `${GCMAKE_FILES}` variable, this is were the files that are specified are put.

Once the CMakeLists.txt file is generated it should look something like this:
```CMake
########GCMake Generated Block DO NOT EDIT########
set(GCMAKE_FILES  "./src/glob.c" "./src/GCMake.c" "./src/cJSON.c")
##################################################
cmake_minimum_required(VERSION 3.1)
project(GCMake)
add_executable(GCMake ${GCMAKE_FILES})
target_include_directories(GCMake PRIVATE include)
```

Once the generator has run successfully you can run cmake normally.