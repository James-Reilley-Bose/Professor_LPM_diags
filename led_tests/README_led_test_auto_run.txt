files: 
led_test.sh
led_test.service
ledtest_load.sh



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



