#!/bin/bash

for i in {1..50};
do sudo powertop --csv=data/exp1-adp-ctr/powertop-$i-adp_change_time_report.csv;
sleep 5;
done