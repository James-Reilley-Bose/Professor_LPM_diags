files: 
led_test.sh
led_test.service



This Procedure will add the led_test.sh as a service that is started each time the system starts
without any user interaction.



If you need to change the partition protection:

  adb shell mount -orw,remount /

Copy the files:

  adb push led_test.sh /etc/initscripts/
  adb push led_test.service /etc/systemd/system/


set file protections:

  adb shell chmod +x /etc/initscripts/led_test.sh
  adb shell chmod 644  /etc/systemd/system/led_test.service

enable service:

  adb shell systemctl daemon-reload
  adb shell systemctl enable led_test.service


start service:

  adb shell systemctl start led_test.service


It should be running now(if mute button is not pressed).


Power cycle the system and the led test should be running.

Users guide:
When mic mute is active(red) - blink sequence is stopped
                               +/- can be used to speed of slow blink rate
			       default is on for 1 second
			       range  0 .1 .2 .3 .4 .5 .6 .7 .8 .9 1 2
			       Each - or + button press selects the next delay
			   

