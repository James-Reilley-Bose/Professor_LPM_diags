#!/bin/bash

mount -orw,remount /

echo "copy led_test.service to /etc/systemd/system/"
cp /opt/Bose/led_test.service /etc/systemd/system/
chmod 664 /etc/systemd/system/led_test.service

echo "copy led_test.sh  to /etc/initscripts/"
cp /opt/Bose/led_test.sh /etc/initscripts/
chmod +x /etc/initscripts/led_test.sh

echo "configure the services"
systemctl daemon-reload
sleep 1
systemctl enable led_test.service

