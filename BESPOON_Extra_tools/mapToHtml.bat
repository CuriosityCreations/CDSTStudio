@echo off
REM arg1 is project path, arg 2, the html output name, arg4 heapsize
for /f %%F in ('dir %1\%2\Module.map /b /s') do ( 
set p=%%F
)
echo "GENERATING RAM/ROM SUMMARY"
cd %1
"%1\..\BESPOON_Extra_tools\mapToHtml.exe" %p% out="%3"  heapsize=%4%
