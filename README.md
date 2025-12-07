# PelletBoiler

PelletBoiler is a firmware project designed to manage and visualize the operations of a pellet boiler system. This software leverages advanced tools and frameworks to provide a user-friendly interface and efficient control mechanisms for boiler systems, making it highly adaptable for embedded applications.

## Features

- **Boiler Management**: Real-time monitoring and control of pellet boiler operations.
- **Dynamic Configuration**: Easily adjust configurations on the fly.
- **Graphical User Interface**: Utilizes the **LVGL** graphics library for an intuitive and customizable display.
- **ESP-IDF Integration**: Built on the ESP-IDF framework for seamless compatibility with ESP32 hardware.
- **Modular Structure**: Well-organized codebase with dedicated components for UI, configuration, and system management.

## Technologies Used

- **LVGL (Light and Versatile Graphics Library)**: For UI design and rendering.
- **ESP-IDF (Espressif IoT Development Framework)**: To power the embedded firmware on ESP32 hardware.
- **CMake**: Build system for compiling and organizing project dependencies.
- **C and C++**: Primary programming languages used to achieve low-level hardware interaction and high-level logic.

## File Organization

- **`main`**: Contains the core firmware logic.
- **`lvgl_uifiles`**: UI layouts and assets designed with LVGL.
- **`DynamicConfig`**: Handles dynamic settings and configurations.
- **`diagram.json`**: Wokwi schema file. Used for emulating the device.
- **`sdkconfig`/`sdkconfig.defaults`**: Configuration files for ESP-IDF.

## Getting Started

1. Clone the repository:
   ```bash
   git clone https://github.com/IliaSalaur/PelletBoiler.git
   cd PelletBoiler
   ```
2. Use ESP-IDF to set up your environment and compile the project:
   ```bash
   idf.py set-target esp32
   idf.py build
   idf.py flash
   ```

3. Monitor the output using:
   ```bash
   idf.py monitor
   ```

4. Ensure the boiler's hardware interfaces, sensors, and display are properly connected.

## License

This project is licensed under the AGPL-3.0 License. See the [LICENSE](./LICENSE) file for details.