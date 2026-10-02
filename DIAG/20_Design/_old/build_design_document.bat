@REM just a temp. solution for plugin automatic testing
@REM porting DD in latex as soon as possible
@REM don't edit this file

@REM Change to the script directory (necessary when invoked from another .bat file)
cd /d "%~dp0"

run.bat doc2pdf RTA-SUM_DIAG_Design_Document.docm
@if %errorlevel% neq 0 exit /b 1