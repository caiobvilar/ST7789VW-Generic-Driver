# ST7789VW-Generic-Driver

Generic C driver for the ST7789VW display controller.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## CMake Options

- `ST7789_ENABLE_SPI_PORT`: Build SPI port glue (default: ON)
- `ST7789_ENABLE_SERIAL_PORT`: Build serial port glue (default: OFF)

Example:

```bash
cmake -S . -B build -DST7789_ENABLE_SERIAL_PORT=ON
cmake --build build -j
```

## Consume the Library

### add_subdirectory (in-tree)

```cmake
add_subdirectory(ST7789VW-Generic-Driver)
target_link_libraries(my_app PRIVATE st7789)
```

### find_package (installed)

After `cmake --install` or packaging, consumers can do:

```cmake
find_package(st7789 CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE st7789::st7789)
```

Headers install to `include/` and the CMake package files to `lib/cmake/st7789`.
