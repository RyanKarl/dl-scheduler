#!/bin/bash
#Run as sudo
#Topology is the argument to dataParser
if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi
if (( $# != 2 )); then
    echo "USAGE: $ ./collect.sh output error"
    exit
fi

forkstat -e exit | tee forkout.txt | (./dataParser 4 3 2 1 > $1 2> $2)

#sudo forkstat -e exec,exit 2> forkerror.txt | tee forkout.txt | /home/jonathan/Documents/grad_os/dl-scheduler/data/dataParser 4 3 2 1 > $1 2> $2
