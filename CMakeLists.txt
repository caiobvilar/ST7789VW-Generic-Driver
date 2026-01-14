cmake_minimum_required(VERSION 3.13)

project(st7789 VERSION 1.0.0 LANGUAGES C)

# Optionally allow the user to toggle ports on/off
option(ST7789_ENABLE_SPI_PORT "Build SPI port glue" ON)
option(ST7789_ENABLE_SERIAL_PORT "Build serial port glue" OFF)

# Library sources
set(ST7789_SOURCES
    core/src/st7789.c
)

add_library(st7789 STATIC ${ST7789_SOURCES})

# Public include dir so consuming targets can do: target_link_libraries(app PRIVATE st7789)
target_include_directories(st7789
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/core/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/../../Board/STM32F411E/TFT-SPI-2-GTM020-02
)

# Keep compile options generic; MCU/optimization flags belong in the parent project
target_compile_features(st7789 PUBLIC c_std_99)

# Install and package configuration disabled for submodule usage
# Uncomment if you want to install this library standalone
# include(CMakePackageConfigHelpers)
# install(TARGETS st7789 ...)
