#ifndef QROLLE_EEPROM_H
#define QROLLE_EEPROM_H


// EEPROM-related functions and defines to clean up main qrolle.c.
// A header-only implementation.
//
// Authors:
// Antti Nilakari / OH3HMU <anilakar@cc.hut.fi>
//
// Version history:
// 2009-04-15 Initial version / AN


#include <avr/io.h>
#include <avr/eeprom.h>

// Number of VFOs supported
#define EEPROM_NVFO 2


// A semi-random magic number to check the EEPROM contents.
#define EEPROM_MAGIC_NUMBER REVISION


// Default values for EEPROM
#define EEPROM_DEFAULT_VFO 0
#define EEPROM_DEFAULT_VFO0_FREQ 3699000
#define EEPROM_DEFAULT_VFO1_FREQ 14000000
#define EEPROM_DEFAULT_STEP 0


// Structure for saving non-volatile settings
typedef struct settings_s {
    uint8_t magicNumber; // Used to determine EEPROM state
    int vfo_num; // Number of currently selected VFO
    uint32_t vfo[EEPROM_NVFO]; // VFO frequencies
    int32_t step; // step
} settings_t;


// EEPROM address to save the settings to
settings_t EEMEM eeprom_settings_addr;


//! \short Save settings to EEPROM.
//! \param settings address of struct settings_t to read from
static inline void save_settings(const settings_t *settings)
{
    eeprom_write_block(settings, &eeprom_settings_addr, sizeof(settings_t));
}


//! \short Load settings from EEPROM.
//! Writes the loaded data into a preallocated struct settings_t
//! \param settings address of settings_t to write into
static inline void load_settings(settings_t *settings)
{
    eeprom_read_block(settings, &eeprom_settings_addr, sizeof(settings_t));
    
    // Set default values and save in case EEPROM contains garbage
    if (settings->magicNumber != EEPROM_MAGIC_NUMBER)
    {
        settings->magicNumber = EEPROM_MAGIC_NUMBER;
        settings->vfo_num = EEPROM_DEFAULT_VFO;
        settings->vfo[0] = EEPROM_DEFAULT_VFO0_FREQ;
        settings->vfo[1] = EEPROM_DEFAULT_VFO1_FREQ;
        settings->step = EEPROM_DEFAULT_STEP;
        save_settings(settings);
    }
}


#endif // QROLLE_EEPROM_H