
This Procedure will stop and disable the led_test process so that it will not interfere
with user code.


If you need to change the partition protection:

  adb shell mount -orw,remount /

stop and disable service:

  adb shell systemctl stop led_test
  adb shell systemctl disable led_test
  adb shell systemctl daemon-reload



To delete the files:
  Note: the files do not need to be deleted if you may want to re-enable later.    

  adb shell rm /etc/initscripts/led_test.sh
  adb shell rm /etc/systemd/system/led_test.service
