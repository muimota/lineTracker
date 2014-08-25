#!/bin/bash
v4l2-ctl -d $1 -c auto_exposure=1
v4l2-ctl -d $1 -c gain_automatic=0
v4l2-ctl -d $1 -c gain=0
v4l2-ctl -d $1 -c brightness=0
v4l2-ctl -d $1 -c contrast=64
