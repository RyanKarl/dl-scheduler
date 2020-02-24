#!/bin/bash
#First argument is the column to grab, second is the filename to output to
ps axu | cut -f $1 -d ' ' > $2
