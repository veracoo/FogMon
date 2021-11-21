#!/bin/bash

while true;
do { docker stats --no-stream --format "{{.ID}} {{.Name}} {{.CPUPerc}} {{.MemUsage}} {{.MemPerc}} {{.NetIO}} {{.BlockIO}} {{.PIDs}}" && date -u; } | tee --append data/exp1-adp-wb-ctr/adp_change_time_report_wb.csv;
sleep 1;
done