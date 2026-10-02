@REM this script generates the requirement view not in quiet mode 
@REM and updates the traceability report

@ECHO OFF
call run rreqs clean
@if %errorlevel% neq 0 GOTO restoreAndExit

call run rreqs view
@if %errorlevel% neq 0 GOTO restoreAndExit

cd ./traceability_report
call ./generate_report.bat
cd ..

:restoreAndExit
cd %currentDirectory%
exit /b %errorlevel%