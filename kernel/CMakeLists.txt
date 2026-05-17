cmake_minimum_required(VERSION 3.10)
project(kaizor_kernel_bridge)

add_library(kaizor_kernel_bridge
    kaizor_init.cpp
    freebsd_bridge.cpp
    init_system.cpp
    startup.cpp
    hardware.cpp
    mount_manager.cpp
    devfs_manager.cpp
    rc_service.cpp
    sysctl_manager.cpp
    jail_manager.cpp
    console_logger.cpp
    kaizor_bootstrap.cpp
)

target_include_directories(kaizor_kernel_bridge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
