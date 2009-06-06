#!/bin/bash

echo Sendmail hook script v0.1
#echo Variables are: $1, $2, $3, $4

echo "$2 downloaded from feed $1" | mail -s "Tuxcast: $1" your.address@example.com
