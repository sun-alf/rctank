#!/bin/bash
echo "Hello, Linux auto-run!"
sudo /home/pi/rctank/rctank.elf --cfg=/home/pi/rctank/res/gpiodev.cfg --profile=/home/pi/rctank/res/profile_linux.cfg --assert_gpio=27
