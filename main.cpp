//
// main.cpp 
// Main file
// ----------------------------------
// Developed with embedXcode 
// http://embedXcode.weebly.com
//
// Project Tubers
//
// Created by Peter Rockwell, 2012-12-12 8:26 PM
// Peter Rockwell
//	
//
// Copyright © Peter Rockwell, 2012
// Licence CC = BY NC SA
//
// See Tubers.ino and ReadMe.txt for references
//
// ----------------------------------
// DO NOT EDIT THIS FILE.
// THE SKETCH IS IN Tubers.ino
// ----------------------------------
//


// Core library
#if defined(MPIDE) // chipKIT specific
#include "WProgram.h"
#elif defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
#include "Energia.h"
  #if defined(__LM4F120H5QR__) // StellarPad specific
  #include "inc/lm4f120h5qr.h"
  #include "driverlib/rom.h"
  #include "driverlib/sysctl.h"
  #endif
#elif defined(MAPLE_IDE) // Maple specific
#include "WProgram.h"
#elif defined(CORE_TEENSY) // Teensy specific
#include "WProgram.h"
#elif defined(WIRING) // Wiring specific
#include "Wiring.h"
#elif defined(ARDUINO) && (ARDUINO >= 100) // Arduino 1.0x and 1.5x specific
#include "Arduino.h" 
#elif defined(ARDUINO) && (ARDUINO < 100)  // Arduino 23 specific
#include "WProgram.h"
#else // error
#error Platform not defined
#endif // end IDE

// Sketch
#include "Tubers.ino"

#if defined(MAPLE_IDE) // Maple specific
// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated objects that need libmaple may fail.
__attribute__(( constructor )) void premain() {
    init();
}
#endif

// Main
int main(void) {
    
    // Init
    
#if defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
#if defined(__LM4F120H5QR__) // StellarPad specific
    ROM_FPULazyStackingEnable();
    
    timerInit();
    
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    
#else // LaunchPad and FraunchPad specific
    init();
#endif
    
#elif defined(CORE_TEENSY) // Teensy specific
    _init_Teensyduino_internal_();
    
#elif defined(WIRING) // Wiring specific
    boardInit();
    
#else // General case
    init();
    delay(1);
    
#if defined(ARDUINO) && (ARDUINO >= 100) // Arduino 1.0 + 1.5 specific
#if defined(USBCON) // Arduino 1.0 + 1.5 specific
    USBDevice.attach();
#endif
#endif
    
#endif
    
    // Setup
    
    setup();
    
    // Loop
    
    for (;;) {
        
        loop();
        
#if defined(ENERGIA)
#elif defined(MPIDE)
#elif defined(CORE_TEENSY)
#ifdef USB_SERIAL
        usb_serial_class Serial;
#endif
#elif defined(WIRING)
#elif defined(ARDUINO) && (ARDUINO >= 100) // Arduino 1.0 + 1.5 specific
        if (serialEventRun) serialEventRun();
#endif
    }
    
    return 0;
}