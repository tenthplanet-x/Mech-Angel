#!/bin/sh
cd ..
#num=`cat /proc/cpuinfo | grep cores | uniq | awk -F":" '{print $2}'`
num=`cat /proc/cpuinfo | grep processor | wc -l`
make -j$num