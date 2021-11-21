#!/bin/bash

for i in {1..50};
do sudo powertop --csv=data/exp1-adp-dm/powertop-$i-adp_disable_metrics.csv;
sleep 5;
done