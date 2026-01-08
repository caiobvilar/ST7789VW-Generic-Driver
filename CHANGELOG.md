# Changelog

All notable changes to this project will be documented in this file.

## [1.0.0] - 2026-01-08
### Added
- Initial release of `st7789` static library.
- SPI and Serial port glue selectable via CMake options:
  - `ST7789_ENABLE_SPI_PORT` (default ON)
  - `ST7789_ENABLE_SERIAL_PORT` (default OFF)
- Installable CMake package with exported target `st7789::st7789`.
- GitHub Actions CI building a small configuration matrix.
- CPack packaging to produce `.tar.gz` and `.zip` artifacts.
