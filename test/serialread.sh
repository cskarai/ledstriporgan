#!/bin/sh

stty -F /dev/ttyUSB0 cs8 2000000 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts
./serialread.pl /dev/ttyUSB0 data.bin
