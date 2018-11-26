file name: led_test.sh

This is a shell script that can be pushed to the Flipper Riviera
and run to do a quick test of the led funtion.

Test description:
 The test lights the LEDs one at a time for the selected intensity and duration.
 The sequence is as follows-
 White LEDs 0 1 2 3 7 11 12 13 14 (left to right, 9 LEDs)
 Red LEDs   4 10                  (left to right, 2 LEDs)
 Green LEDs 5 9                   (left to right, 2 LEDs)
 Blue LEDs  6 8                   (left to right, 2 LEDs)
                                           total 15 LEDs

MUTE Button functions as a pause when in repeat loop mode.
MIC mute must be off to run in all cases.

Running in repeat loop mode the MIC mute on (RED) will pause the repeat loop.
When the MIC mute is in the OFF position the loop will resume from LED 1 in the sequence.
Pressing the MIC mute(putting it in MIC mute)immediately pauses the sequence.


If you need to change the partition protection:

  adb shell mount -orw,remount /opt/Bose

Copy the file:

  adb push led_test.sh /opt/Bose/


set file protection:

  adb shell chmod 755 /opt/Bose/led_test.sh


run the test:

  adb shell /opt/Bose/led_test.sh


There are 3 optional arguments:

  seconds delay after each led is lit (default is 1 second)

  LED intensity (0-4095) (default is 100)

  repeat loop delay (seconds)


Example 1:

  use 1 second delay, intensity set to 1000 (bright!)
 
  adb shell /opt/Bose/led_test.sh 1 1000

Example 2:

  use 3 second delay, intensity set to 10 (dim)
 
  adb shell /opt/Bose/led_test.sh 3 10

Example 3:

  use 3 second delay, intensity set to 100 and repeat after a 5 second delay
 
  adb shell /opt/Bose/led_test.sh 3 10 5

