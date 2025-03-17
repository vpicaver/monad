cmake_minimum_required(VERSION 3.10)
project(monad VERSION 1.0.0 LANGUAGES CXX)

option(ENABLE_TESTING "Enable testing for the project" OFF)

add_library(monad INTERFACE)
target_include_directories(monad INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()

install(TARGETS monad
    LIBRARY DESTINATION lib
    "Monad.h" DESTINATION include
    "Result.h" DESTINATION include
)

export(TARGETS monad NAMESPACE monad:: FILE monadConfig.cmake)
set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE monad)
