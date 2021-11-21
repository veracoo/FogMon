#!/bin/bash

for i in {1..50};
do sudo powertop --csv=data/exp1-nadp/powertop-$i-nadp.csv;
sleep 5;
done