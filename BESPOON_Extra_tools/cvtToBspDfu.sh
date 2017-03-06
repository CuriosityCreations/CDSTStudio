#!/bin/sh
#REM Generate bsp dfu.
#@echo off
#REM REM cvtToBspDfu.bat PROJECT_PATH OUTPUTNAME WITHOUT EXTENSION
for f in `find $1/$2 -name "Module.hex"`
do
    p=$f
done
echo "bs_converter_app" -f "$p" -o "$3.bspdfu"
"$1/../BESPOON_Extra_tools/bs_converter_app" -f "$p" -o "$3.bspdfu"
