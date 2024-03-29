#
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

# Compile CRT for given arch and get crt0.o, crti.o, and crtn.o.
# Add crt0.o and crti.o to the start of all targets link flags.
# Add crtn.o to the end of all targets link flags.

# Compile sel4runtime library from src.
# Use src as private includes for crt and sel4runtime.
# Use include as public includes for crt and sel4runtime.

cmake_minimum_required(VERSION 3.12.0)

project(sel4runtime C ASM)
set(configure_string "")
config_string(
    Sel4RuntimeRootStack
    SEL4RUNTIME_ROOT_STACK
    "Size of the initial stack for the root task"
    DEFAULT
    16384
    UNQUOTE
)
config_string(
    Sel4RuntimeStaticTLS
    SEL4RUNTIME_STATIC_TLS
    "Size of static TLS area for new processes"
    DEFAULT
    16384
    UNQUOTE
)
add_config_library(sel4runtime "${configure_string}")
list(APPEND crt_files crt/sel4_arch/${KernelSel4Arch}/crti.S crt/sel4_arch/${KernelSel4Arch}/crtn.S)
if("${KernelArch}" STREQUAL "riscv" OR "${KernelArch}" STREQUAL "loongarch")
    list(APPEND crt_files crt/arch/${KernelArch}/crt0.S crt/arch/${KernelArch}/sel4_crt0.S)
else()
    list(
        APPEND
            crt_files crt/sel4_arch/${KernelSel4Arch}/crt0.S
            crt/sel4_arch/${KernelSel4Arch}/sel4_crt0.S
    )
endif()
add_library(sel4runtime_crt_obj OBJECT ${crt_files})
target_include_directories(
    sel4runtime_crt_obj
    PRIVATE
        src
        src/mode/${KernelWordSize}
        src/arch/${KernelArch}
        src/sel4_arch/${KernelSel4Arch}
    PUBLIC include
)
target_link_libraries(sel4runtime_crt_obj sel4_autoconf sel4runtime_Config)
add_custom_command(
    OUTPUT
        "${CMAKE_BINARY_DIR}/lib/crt0.o" "${CMAKE_BINARY_DIR}/lib/crti.o"
        "${CMAKE_BINARY_DIR}/lib/crtn.o"
    DEPENDS sel4runtime_crt_obj
    COMMAND
        ${CMAKE_COMMAND} -E env cp "$<TARGET_OBJECTS:sel4runtime_crt_obj>" ${CMAKE_BINARY_DIR}/lib/
    COMMAND
        ${CMAKE_COMMAND} -E env mv ${CMAKE_BINARY_DIR}/lib/crt0.S${CMAKE_C_OUTPUT_EXTENSION}
        ${CMAKE_BINARY_DIR}/lib/crt0.o
    COMMAND
        ${CMAKE_COMMAND} -E env mv ${CMAKE_BINARY_DIR}/lib/crti.S${CMAKE_C_OUTPUT_EXTENSION}
        ${CMAKE_BINARY_DIR}/lib/crti.o
    COMMAND
        ${CMAKE_COMMAND} -E env mv ${CMAKE_BINARY_DIR}/lib/crtn.S${CMAKE_C_OUTPUT_EXTENSION}
        ${CMAKE_BINARY_DIR}/lib/crtn.o COMMAND_EXPAND_LISTS
)
add_custom_target(
    sel4runtime_crt
    DEPENDS
        "${CMAKE_BINARY_DIR}/lib/crt0.o" "${CMAKE_BINARY_DIR}/lib/crti.o"
        "${CMAKE_BINARY_DIR}/lib/crtn.o"
)
list(
    APPEND
        sources
        src/crt1.c
        src/start.c
        src/start_root.c
        src/env.c
        src/init.c
        src/memset.c
        src/memcpy.c
        src/vsyscall.c
)
if(("${KernelSel4Arch}" STREQUAL "aarch32") OR ("${KernelSel4Arch}" STREQUAL "arm_hyp"))
    list(
        APPEND
            sources src/sel4_arch/${KernelSel4Arch}/__aeabi_read_tp.S
            src/sel4_arch/${KernelSel4Arch}/__aeabi_read_tp_c.c
    )
endif()

# The sel4runtime library.
add_library(sel4runtime STATIC ${crt_files} ${sources})
target_include_directories(
    sel4runtime
    PRIVATE
        src
        src/mode/${KernelWordSize}
        src/arch/${KernelArch}
        src/sel4_arch/${KernelSel4Arch}
    PUBLIC
        include
        include/mode/${KernelWordSize}
        include/arch/${KernelArch}
        include/sel4_arch/${KernelSel4Arch}
)
add_dependencies(sel4runtime sel4runtime_crt)

# A C library is still needed here to provide memcpy, stdint.h, and
# elf.h
target_link_libraries(sel4runtime PUBLIC sel4 sel4_autoconf PRIVATE sel4runtime_Config)
