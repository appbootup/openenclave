# Copyright (c) Open Enclave SDK contributors.
# Licensed under the MIT License.

set (EDL_FILE ../libcxx.edl)

add_custom_command(
    OUTPUT libcxx_u.h libcxx_u.c
    DEPENDS ${EDL_FILE} edger8r
    COMMAND edger8r --untrusted ${EDL_FILE} --search-path ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(libcxx_host host.cpp libcxx_u.c)

target_link_libraries(libcxx_host oehostapp)

target_include_directories(libcxx_host PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
