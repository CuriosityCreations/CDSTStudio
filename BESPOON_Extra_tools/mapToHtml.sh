#!/bin/sh
#Generate RAM/ROM Summary
#@echo off
#REM arg1 is project path, arg 2, the html output name, arg4 heapsize
for f in `find $1/$2 -name "Module.map"`
do
    p=$f
done
echo "GENERATING RAM/ROM SUMMARY" 
"$1/../BESPOON_Extra_tools/mapToHtml.py" $p out="$3"  heapsize=$4
