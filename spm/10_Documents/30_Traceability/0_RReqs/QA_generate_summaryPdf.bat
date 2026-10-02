@REM this script generates the Summary PDF for the QA

@ECHO OFF
set "currentDirectory=%cd%"
cd ../../20_Design
call rename_to_txt.bat
cd %currentDirectory%

call run rreqs clean
@if %errorlevel% neq 0 GOTO restoreAndExit

call run rreqs quiet Summary:SRS_SPM
@if %errorlevel% neq 0 GOTO restoreAndExit

call run rreqs quiet Summary:DDS_SPM
@if %errorlevel% neq 0 GOTO restoreAndExit

cd .\traceability_report
py %SUM_TOOLS%\merge_summaries_pdf\mergePdf.py "config.py"
call generate_report.bat

cd ..

:restoreAndExit
del *.xml /q
del *.pdf /q
cd ../../20_Design
call rename_to_md.bat
cd %currentDirectory%
exit /b %errorlevel%