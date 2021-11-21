#!/bin/bash

NC='\033[0m' 			# No Color
PURPLE='\033[0;35m'		# Purple	

docker-compose -f docker-compose-exp1-adp-dm-ctr.yml up -d;
{ timeout 30m bash ./exp1-adp-dm-ctr-docker-stats.sh; printf "${PURPLE}Docker-stats finished${NC}.\n"; } & 
{ bash ./exp1-adp-dm-ctr-powertop.sh; printf "${PURPLE}Powertop finished${NC}.\n"; } &
wait
docker-compose -f docker-compose-exp1-adp-dm-ctr.yml down;