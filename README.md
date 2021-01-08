![](https://github.com/Pikachuxxxx/R8/blob/dev/resources/Branding/R8-Tw.png)

# About
R8 is a simple and easy to use 8-bit Rendering and Rasterization API written in C. The design is similar to and inspired by OpenGL1.1. 

## Getting Started
Since the API was inspiered by OpenGL most of it's working and architecuture is very simple to use and extend upon. As of version 1.0 R8 is only CPU accelerated and does not provide any kind of GPU acceleration. However it is capable of rendering a lot of stuff. It was designed to be Cross-Platform.

## Features
- Written in C
- Extremely fast and efficient custom data types and math
- 8-bit colors suppor for lightweight rendering and avoid any performance overloads
- Face culling and wireframe mode for all primitive types
- Custom CPU optimized Raster engine written in C
- Cross platform (Windows, MacOS, Linux) and uses native std libraries, does not have any kind of external dependencies

## Why C and not C++?

C code is slim, highly portable, compiles fast and for a low-level software renderer one doesn't need lots of object-oriented or templated code.

## Building 
The project uses CMake build system to build either a static or dynamic library. However because of it's simplicity the core API can be extended as you want and used directly as source.

#### Configurations 
The API uses various configurations to enabled or disable specific features for fine tuning. 

(To be Updated...)
