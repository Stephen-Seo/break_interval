cmake_minimum_required(VERSION 3.7)
project(break_interval)

set(break_interval_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")

add_executable(break_interval ${break_interval_SOURCES})

add_compile_options(
    -Wall -Wextra -Wpedantic -Wno-missing-braces
)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Debug', none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

target_compile_options(break_interval PUBLIC
    -Wall -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough
    -Werror=format-security
    -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3
    -D_GLIBCXX_ASSERTIONS
    -fstrict-flex-arrays=3
    -fstack-clash-protection -fstack-protector-strong
    -Wl,-z,nodlopen -Wl,-z,noexecstack
    -Wl,-z,relro -Wl,-z,now
    -Wl,--as-needed -Wl,--no-copy-dt-needed-entries
    -fPIE -pie
)

target_link_options(break_interval PUBLIC
    -Wall -Wformat -Wformat=2 -Wconversion -Wimplicit-fallthrough
    -Werror=format-security
    -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3
    -D_GLIBCXX_ASSERTIONS
    -fstrict-flex-arrays=3
    -fstack-clash-protection -fstack-protector-strong
    -Wl,-z,nodlopen -Wl,-z,noexecstack
    -Wl,-z,relro -Wl,-z,now
    -Wl,--as-needed -Wl,--no-copy-dt-needed-entries
    -fPIE -pie
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(break_interval PUBLIC
        -O3
        -fno-delete-null-pointer-checks -fno-strict-overflow
        -fno-strict-aliasing -ftrivial-auto-var-init=zero
    )
    target_link_options(break_interval PUBLIC
        -O3
        -fno-delete-null-pointer-checks -fno-strict-overflow
        -fno-strict-aliasing -ftrivial-auto-var-init=zero
    )
else()
    target_compile_options(break_interval PUBLIC
        -Og
    )
    target_link_options(break_interval PUBLIC
        -Og
    )
endif()
