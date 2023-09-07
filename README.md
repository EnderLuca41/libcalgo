# libcalgo
This just a collection of different algorithms that I wanted to write (currently primarly hashing and encryption algorithms),
Use it for whatever you want (for example as a refrence), but be aware of [the license](LICENSE).

## Using cryptographic algorithms
I **strongly** disguise you to use any of the cryptographic algortihms in a real-world codebase.
These implementations are made for pure practice reasons and are not made to really be secure.
A badly implemented cryptographic algorithm can be a big secruity hole in your sofware.
If you want secure implementations to use in your codebase, [libcrypto](https://www.openssl.org/docs/man3.0/man7/crypto.html) is your way to go.

But if you want to understand these algotithms and mabye compare your practice implementation to other implementations, 
then this libray is you way to go.

## Compatibility
All Algorthms are written to run on a Little-Endian x86 or x86-64 mashine,
there are maybe more incompatibilities because I only tested it on my home computer.
I test every algorithm that I write, but there are maybe some edge-cases that I didn't thought about.

## Compiling
If you want to compile the library, you need the following tools installed:

* [git](https://git-scm.com/)
* [make](https://www.gnu.org/software/make/)
* [CMake](https://cmake.org/)
* A C compiler like [GCC](https://gcc.gnu.org/) or [Clang](https://clang.llvm.org/)

First clone the library with
```console
$ git clone https://github.com/EnderLuca41/libcalgo
```
and change the directory

```console
$ cd libcalgo
```

Build the makefile

```console
$ cmake .
```
and compile

```console
$ make
```

Now you have .so (shared object) lying in your directory that you can use.
