@echo off
REM cvtToBspDfu.bat PROJECT_PATH OUTPUTNAME WITHOUT EXTENSION
for /f %%F in ('dir %1\%2\Module.hex /b /s') do ( 
set p=%%F
)
echo "bs_converter_app.exe" -f %p% -o "%3.bspdfu"
cd %1
"%1\..\BESPOON_Extra_tools\bs_converter_app.exe" -f %p% -o "%3.bspdfu"
