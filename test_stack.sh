#!/bin/bash

adb push stack.ko /data
adb shell insmod /data/stack.ko
adb shell "echo 1 > /sys/kernel/debug/stack/push"
adb shell "echo 2 > /sys/kernel/debug/stack/push"
adb shell "echo 3 > /sys/kernel/debug/stack/push"
adb shell "cat /sys/kernel/debug/stack/show"
adb shell "echo 4 > /sys/kernel/debug/stack/push"
adb shell "echo 5 > /sys/kernel/debug/stack/push"
adb shell "cat /sys/kernel/debug/stack/show"
adb shell "echo 1 > /sys/kernel/debug/stack/pop"
adb shell "echo 1 > /sys/kernel/debug/stack/pop"
adb shell "cat /sys/kernel/debug/stack/show"
adb shell "echo 1 > /sys/kernel/debug/stack/pop"
adb shell "echo 1 > /sys/kernel/debug/stack/pop"
adb shell "cat /sys/kernel/debug/stack/show"
adb shell rmmod stack
