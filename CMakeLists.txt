cmake_minimum_required(VERSION 3.16)
set(CMAKE_C_STANDARD 99)

project(libcalgo)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)
add_library(libcalgo SHARED src/HashMap.c src/hashing/fnv.c src/hashing/pearson.c src/hashing/md/md5.c src/hashing/md/md4.c src/hashing/md/md2.c src/hashing/sha1/sha1.c src/hashing/sha2/sha224_256.c src/hashing/sha2/sha384_512.c src/hashing/sha2/fsha224_256.c src/util.c src/sorting.c src/floatvector.c)
target_link_libraries(libcalgo m)

install(TARGETS libcalgo DESTINATION /usr/lib) 