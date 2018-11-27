#!/bin/bash
let led_delay=1
loop_delay=5
repeat_loop=0
mute_wait=0
intensity=100
mutemode="OFF"
button="00 00"

dl_index=1

set_led_delay(){
    case $1 in
	0)
	    led_delay=0
	    ;;
	1)
	    led_delay=.1
	    ;;
	2)
	    led_delay=.2
	    ;;
	3)
	    led_delay=.3
	    ;;
	4)
	    led_delay=.4
	    ;;
	5)
	    led_delay=.5
	    ;;
	6)
	    led_delay=.6
	    ;;
	7)
	    led_delay=.7
	    ;;
	8)
	    led_delay=.8
	    ;;
	9)
	    led_delay=.9
	    ;;
	10)
	    led_delay=1
	    ;;
	11)
	    led_delay=2
	    ;;
    esac
}



check_change_delay(){
    button="$(echo lpm pt \"dt ps,read\" | nc localhost:17000)"

    if [ "${button:46:8}" = "03 03 01" ]; then
	echo "button +"
	if [ $dl_index -lt '11' ];then
	    dl_index=$(($dl_index + 1))
	fi
	echo lpm pt \"dt ps,one,14,100\" | nc localhost:17000
	set_led_delay $dl_index
	sleep 1
        echo lpm pt \"dt ps,clear\" | nc localhost:17000
    fi

    if [ "${button:46:8}" = "03 05 01" ]; then
	echo "button -"
	if [ $dl_index -gt '0' ];then
	    dl_index=$(($dl_index - 1))
	fi
	echo lpm pt \"dt ps,one,0,100\" | nc localhost:17000
	set_led_delay $dl_index
	sleep 1
        echo lpm pt \"dt ps,clear\" | nc localhost:17000
    fi

    echo "led_delay = "$led_delay "index = " $dl_index
}

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

	check_change_delay

    fi
    echo "loop_delay " $loop_delay
    sleep $loop_delay
done

