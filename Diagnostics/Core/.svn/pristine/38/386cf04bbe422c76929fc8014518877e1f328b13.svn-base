---++PORTING:

---+++Requirements:
To run this driver you will need a SPI driver that allows you to write
and read to the bus.  In addition this driver will need the ability to
call a function to signal that a transfer is complete.

In addition to the SPI driver you will also need an interface to a
timer that has the ability to measure time in micro seconds, and the
ability to interrupt on the falling edge of a GPIO pin.  One of these
will be used for the GDO2 output of the cc2500.

---+++Implementation:
To port the cc2500 driver to your platform you will first have to
create a cc2500_platform.h file.  This file contains the basic
interfaces the driver uses to control the chip.  There is an empty
version included named cc2500_platform_empty.h.  

The next thing you will have to do is set up the ISRs to call the
apropritate driver functions. A falling edge on the cc2500 GDO2 line
needs to call "void CC2500_HandleGDO2Interrupt(void)".

You will have to ensure that the CC2500_TxWatchdog function is called
periodically while the chip is transmitting.  It is OK to call this
function all the time to avoid cheking if the driver is in the
transmit state.  An interval of a millsecond or less will be
fine. Defender calls it once every 100uS or so.

The CC2500_Init() fucntion has to be called once at system startup.
The interface into the driver is through CC2500_Command().