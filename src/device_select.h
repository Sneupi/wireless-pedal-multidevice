#ifndef DEVICE_SELECT_H
#define DEVICE_SELECT_H

//-----------------------------------------------------------------------------
// Define the device to be flashed
//-----------------------------------------------------------------------------

#define FLASH_PEDAL 
// #define FLASH_CONTROLLER
// #define FLASH_RECEIVER

//-----------------------------------------------------------------------------
// (DO NOT EDIT THIS) Include the appropriate device
//-----------------------------------------------------------------------------
#if defined(FLASH_PEDAL) || defined(FLASH_CONTROLLER) || defined(FLASH_RECEIVER)
    
    #if defined(FLASH_PEDAL) && defined(FLASH_CONTROLLER) || defined(FLASH_PEDAL) && defined(FLASH_RECEIVER) || defined(FLASH_CONTROLLER) && defined(FLASH_RECEIVER)
        #error "Only one device can be flashed at a time"
    #endif

    #if defined(FLASH_PEDAL)
        #include "pedal/pedal.hpp"
    #elif defined(FLASH_CONTROLLER)
        #include "controller/controller.hpp"
    #elif defined(FLASH_RECEIVER)
        #include "receiver/receiver.hpp"
    #endif

#else
    #error "No device selected to flash"
#endif


#endif // DEVICE_SELECT_H