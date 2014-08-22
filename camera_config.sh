#!/bin/bash
v4l2-ctl -d $1 -c exposure_auto=1
v4l2-ctl -d $1 -c brightness=15
v4l2-ctl -d $1 -c contrast=160
v4l2-ctl -d $1 -c exposure_absolute=200
v4l2-ctl -d $1 -c contrast=160
v4l2-ctl -d $1 -c brightness=128