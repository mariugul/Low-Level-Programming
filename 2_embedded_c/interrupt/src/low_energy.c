#include "../inc/low_energy.h"
#include "../inc/efm32gg.h"
#include <stdbool.h>
#include <stdint.h>

/** Initialize Low Energy Operations*/
void low_energy_init()
{
    low_energy_mode(); // Low energy mode 2

    // Enable more energy savings
    // like disable RAM blocks etc.
    //*EMU_MEMCTRL = 0x07;               // Power down RAM block 1-3
}

/** Go in low energy mode and deepsleep */
void sleep()
{
    __asm__("wfi"); // Sleep
}

/** Set in low energy mode*/
void low_energy_mode()
{
    //*SCR |= 0x06; // Set in low energy mode 2
    *SCR = 0x0; // Set in low energy mode 1
}
