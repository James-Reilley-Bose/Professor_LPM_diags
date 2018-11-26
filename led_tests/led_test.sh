#!/bin/bash
led_delay=1
loop_delay=5
repeat_loop=0
mute_wait=0
intensity=100
mutemode="OFF"

echo $0
if [ $# -gt 0 ]; then
    echo "led_delay = " $1
    led_delay=$1
fi
if [ $# -gt 1 ]; then
    echo "intensity = " $2
    intensity=$2
fi
if [ $# -gt 2 ]; then
    echo "loop_delay = " $3
    loop_delay=$3
    repeat_loop=1
fi

while [ 1 ]
do
 if [ $mute_wait -eq 0 ]; then
  for i in 0 1 2 3 7 11 12 13 14 4 10 5 9 6 8
  do
   if [ $i -eq 0 ]; then
    ledcolor="WHITE"
   fi
   if [ $i -eq 4 ]; then
    ledcolor="RED"
   fi
   if [ $i -eq 5 ]; then
    ledcolor="GREEN"
   fi
   if [ $i -eq 6 ]; then
    ledcolor="BLUE"
   fi
    
   echo $ledcolor " led " $i "for delay = " $led_delay
   echo lpm pt \"dt ps,one,$i,$intensity\" | nc localhost:17000
   sleep $led_delay 
   echo lpm pt \"dt ps,clear\" | nc localhost:17000


   mute_status="$(echo lpm pt \"io r,c,12\" | nc localhost:17000)"
   echo "mute_status = "${mute_status:17:4}



   if [ ${mute_status:17:4} = "High" ]; then
      echo "set mute_wait=1"
      mute_wait=1
      break;
   fi

  done

  if [ $repeat_loop -eq 0 ]; then
      exit
  fi

 else #mute_wait 1
    
  mute_status="$(echo lpm pt \"io r,c,12\" | nc localhost:17000)"
  echo "mute_status = "${mute_status:17:4}
  if [ ${mute_status:17:3} = "Low" ]; then
      echo "set mute_wait=0"
      mute_wait=0
  fi
 fi
 echo "loop_delay " $loop_delay
 sleep $loop_delay
done

