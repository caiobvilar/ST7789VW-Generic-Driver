cmake_minimum_required(VERSION 3.13)

project(st7789 VERSION 1.0.0 LANGUAGES C)

# Optionally allow the user to toggle ports on/off
option(ST7789_ENABLE_SPI_PORT "Build SPI port glue" ON)
option(ST7789_ENABLE_SERIAL_PORT "Build serial port glue" OFF)

# Library sources
set(ST7789_SOURCES
    core/src/st7789.c
)

if(ST7789_ENABLE_SPI_PORT)
    list(APPEND ST7789_SOURCES porting/spi/st7789_port_spi.c)
endif()

if(ST7789_ENABLE_SERIAL_PORT)
    list(APPEND ST7789_SOURCES porting/serial/st7789_port_serial.c)
endif()

add_library(st7789 STATIC ${ST7789_SOURCES})

# Public include dir so consuming targets can do: target_link_libraries(app PRIVATE st7789)
target_include_directories(st7789
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/core/include>
        $<INSTALL_INTERFACE:include>
)

# Keep compile options generic; MCU/optimization flags belong in the parent project
target_compile_features(st7789 PUBLIC c_std_99)

# Install and package configuration
include(CMakePackageConfigHelpers)

# Install target and export it for find_package(CONFIG)
install(TARGETS st7789
        EXPORT st7789Targets
        ARCHIVE DESTINATION lib
)

install(DIRECTORY core/include/ DESTINATION include)

install(EXPORT st7789Targets
        NAMESPACE st7789::
        FILE st7789Targets.cmake
        DESTINATION lib/cmake/st7789)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/st7789ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/st7789Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/st7789Config.cmake"
    INSTALL_DESTINATION "lib/cmake/st7789"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/st7789Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/st7789ConfigVersion.cmake"
    DESTINATION lib/cmake/st7789
)

# Packaging via CPack
set(CPACK_PACKAGE_NAME "st7789")
set(CPACK_PACKAGE_VENDOR "st7789 maintainers")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "maintainers@example.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_LIST_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_LIST_DIR}/README.md")
set(CPACK_GENERATOR "TGZ;ZIP")
include(CPack)
