### Liquid Crystal ESP-IDF version 

This is a port of the Arduino LiquidCrystal library to ESP-IDF. It is compatible both with the ESP32 and the ESP8266.
and C/C++.

This library might be slower than the Arduino version, but it allows you to use real time OSes like FreeRTOS.\
This means it is not blocking and you can use it in a task.
! This is not I2C compatible, it is only for 8 bits or 4 bits connections.

## Installation

To install it clone this repository in your components folder and add it to your main CMakeLists.txt file like this:

```CMake
cmake_minimum_required(VERSION 3.5)
set(EXTRA_COMPONENT_DIRS ../components)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(awesome-project)
```

## Usage

To see an example of usage, check the example folder.

  - Create your LiquidCrystal descriptor

```C
liquid_crystal_t lcd = {
    .rs_pin = GPIO_NUM_10,
    .rw_pin = GPIO_NUM_11,
    .enable_pin = GPIO_NUM_12,
    .data_pins = {GPIO_NUM_35, GPIO_NUM_36, GPIO_NUM_37, GPIO_NUM_38},
    .num_lines = 2,
    .row_offsets = {0x00, 0x40, 0x14, 0x54},
    .is_8bit = false,
    .is_2rows = true,
    .is_large_font = false
};
```

  - Intialize it

```C
lcd_init(&lcd);
```

  - Use it

```C
lcd_print(&lcd, "Hello World!");
```

And that's it!


## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

Feel free to contribute to this project, I will be happy to merge your pull requests.