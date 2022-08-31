cmake_minimum_required(VERSION 3.16)
set(CMAKE_C_STANDARD 99)

project(calgolib)
set(CMAKE_BUILD_TYPE Release)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)
add_library(calgolib SHARED src/HashMap.c src/hashing/fnv.c src/hashing/sha/sha224_256.c src/hashing/sha/sha384_512.c src/hashing/sha/fsha224_256.c src/util.c src/sorting.c)
target_link_libraries(calgolib m)

install(TARGETS calgolib DESTINATION /usr/lib)