cmake_minimum_required(VERSION 3.12)

set(PICO_BOARD "pico")

#include(../pico_sdk_import.cmake)

project(mono_modulation)

pico_sdk_init()


add_executable(mono_modulation
  ADSR.c
  LFO.c
  test.c
)

target_include_directories(mono_modulation PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(mono_modulation PRIVATE
  pico_stdlib
  hardware_pwm
)

pico_enable_stdio_usb(mono_modulation 0)
pico_enable_stdio_uart(mono_modulation 1)

pico_add_extra_outputs(mono_modulation)