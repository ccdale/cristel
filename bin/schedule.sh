#!/bin/bash

/home/chris/src/cristel/bin/startstopdvbstreamer.py -s
sleep 20
/usr/local/bin/getepg -c
/home/chris/src/cristel/bin/cristelfilldb.py
