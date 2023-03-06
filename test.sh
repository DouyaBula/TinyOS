#!/bin/bash 
diff mos.log mos.log > temp
echo $?
diff mos.log 01-01.txt > temp
echo $?
