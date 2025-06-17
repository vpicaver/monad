cmake_minimum_required(VERSION 3.10)
project(monad VERSION 1.0.0 LANGUAGES CXX)

option(MONAD_TESTING "Enable testing for the project" OFF)

find_package(Qt6 REQUIRED COMPONENTS Core)
qt_standard_project_setup()

#Add address sanitizer for clang build only
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_compile_options($<$<CONFIG:Debug>:-fsanitize=address>)
    add_link_options($<$<CONFIG:Debug>:-fsanitize=address>)
endif()

add_library(monad STATIC)
target_include_directories(monad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(monad
    PUBLIC
    Qt6::Core
)

target_sources(monad PRIVATE
    Monad/Monad.h
    Monad/Result.h
)

if(MONAD_TESTING)
    add_subdirectory(tests)
endif()

# set_target_properties(asyncfuture PROPERTIES PUBLIC_HEADER "Monad.h" "Result.h")

# install(TARGETS monad
#     LIBRARY DESTINATION lib
#     PUBLIC_HEADER DESTINATION include
# )

export(TARGETS monad NAMESPACE monad:: FILE monadConfig.cmake)
set(CMAKE_EXPORT_PACKAGE_REGISTRY ON)
export(PACKAGE monad)
