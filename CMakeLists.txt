cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

include(cmake/utils/build_protection.cmake)

set(PROJECT_NAME "ak-file-encryptor")
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RELEASE ON)
set(CMAKE_UPX_COMPRESS ON)

include(cmake/platform/generic_builder.cmake)
