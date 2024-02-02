cmake_minimum_required(VERSION 3.10)
project(RemoteXR VERSION 0.1.0)

include_directories(third_party/openvr/headers)

add_subdirectory(src/native/ovrplugin)