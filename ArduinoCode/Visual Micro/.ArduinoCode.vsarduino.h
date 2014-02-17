#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
//Board = Arduino Mega 2560 or Mega ADK
#define __AVR_ATmega2560__
#define ARDUINO 101
#define __AVR__
#define F_CPU 16000000L
#define __cplusplus
#define __attribute__(x)
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __asm__ 
#define __volatile__
#define __builtin_va_list
#define __builtin_va_start
#define __builtin_va_end
#define __DOXYGEN__
#define prog_void
#define PGM_VOID_P int
#define NOINLINE __attribute__((noinline))

typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}

//
//
int get_breaker(int x);
void update_breakers();
void initiate_breakers();
void snprintSwitches(char *buff, int maxChars);
void * create_state();
void random_test();
void send_states();
void send_calibration();
void send_test_data();
void update_controls();
void update_starter();
void update_switches();
void update_ventilation_speed(char *buff);
void update_trim_display(char *buff);
void update_flaps_display(char *buff);
void update_stall_warning(char *buff);
void update_generator_warning(char *buff);
void update_fuel_pressure_warning(char *buff);

#include "D:\Arduino Program\Clean Config ArduPilot\ArduPilot-Arduino-1.0.3-windows\hardware\arduino\variants\mega\pins_arduino.h" 
#include "D:\Arduino Program\Clean Config ArduPilot\ArduPilot-Arduino-1.0.3-windows\hardware\arduino\cores\arduino\arduino.h"
#include "D:\Code\KatanaSim\KatanaSim\ArduinoCode\ArduinoCode.ino"
#include "D:\Code\KatanaSim\KatanaSim\ArduinoCode\Circuit_Breakers.ino"
#include "D:\Code\KatanaSim\KatanaSim\ArduinoCode\HelperFunctions.ino"
#include "D:\Code\KatanaSim\KatanaSim\ArduinoCode\Send.ino"
#include "D:\Code\KatanaSim\KatanaSim\ArduinoCode\Updates.ino"
#endif
