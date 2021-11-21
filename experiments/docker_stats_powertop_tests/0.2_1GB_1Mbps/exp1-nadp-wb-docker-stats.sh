#!/bin/bash

while true;
do { docker stats --no-stream --format "{{.ID}} {{.Name}} {{.CPUPerc}} {{.MemUsage}} {{.MemPerc}} {{.NetIO}} {{.BlockIO}} {{.PIDs}}" && date -u; } | tee --append data/exp1-nadp-wb/nadp_wb.csv;
sleep 1;
done