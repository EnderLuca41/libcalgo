cmake_minimum_required(VERSION 3.16)
set(CMAKE_C_STANDARD 99)

project(libcalgo)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)
add_library(libcalgo SHARED src/HashMap.c  src/crypto/aes.c src/crypto/adfgx.c src/crypto/bacon.c src/crypto/beaufort.c src/crypto/columnar_transposition.c src/crypto/autokey_vigenere.c src/crypto/four_square.c src/crypto/two_square.c src/crypto/route.c src/crypto/rail_fence.c src/crypto/affine.c src/crypto/ceasar.c src/crypto/vigenere.c src/crypto/nihilist.c src/crypto/des.c src/crypto/mono_alphabetic.c src/crypto/xor.c src/crypto/triple_des.c src/crypto/vatsyayana.c src/crypto/atbash.c src/crypto/homophonic.c src/crypto/playfair.c src/crypto/hill.c src/hashing/fnv.c src/hashing/pearson.c src/hashing/superfasthash.c src/hashing/buzhash.c src/hashing/jenkins/one_at_a_time.c src/hashing/jenkins/lookup2.c src/hashing/jenkins/lookup3.c src/hashing/md/md5.c src/hashing/md/md4.c src/hashing/md/md2.c src/hashing/sha1/sha1.c src/hashing/sha2/sha224_256.c src/hashing/sha2/sha384_512.c src/hashing/sha2/fsha224_256.c src/util.c src/sorting.c src/floatvector.c)
set_target_properties(libcalgo PROPERTIES PUBLIC_HEADER "Include/hashing.h;Include/floatvector.h")
target_link_libraries(libcalgo m)

install(TARGETS libcalgo 
    LIBRARY DESTINATION /usr/lib
    PUBLIC_HEADER DESTINATION /usr/include/libcalgo) 