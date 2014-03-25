#!/bin/bash
day=`date "+%d"`
if [ ! -d "/tmp/smon" ];
then
	mkdir "/tmp/smon"
fi
CMD="sar -o /tmp/smon/sa$day"
proc=`ps -ef | grep -v grep | grep -c "$CMD"`
if [ $proc = '0' ];
then
	$CMD 5 17280 > /dev/null 2>&1 &
	echo "run $CMD"
else
	echo "$CMD is already running"
fi
