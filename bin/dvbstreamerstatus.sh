#!/bin/bash
for adaptor in 0 1; do
  dvbctrl -a $adaptor -u tvc -p tvc current
  dvbctrl -a $adaptor -u tvc -p tvc getmrl
done
