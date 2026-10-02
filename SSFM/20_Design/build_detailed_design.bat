@if not defined SUM_DOC_TOOLS (
	echo SUM_DOC_TOOLS environment variable not defined!
	echo Please set it to the location of your local install of the text documentation package, containing the MiKTeX portable install and SumatraPDF.exe:
	echo e.g.:
	echo $ setx SUM_DOC_TOOLS "C:\full\path\to\sum_doc_tools"
	exit /b 1
)

@if not defined SUM_TOOLS (
	echo SUM_TOOLS environment variable not defined!
	echo Please set it to the location of your local copy of the sum_tools repository:
	echo e.g.:
	echo $ git clone https://bitbucket.etas-dev.com/scm/gmsum/sum_tools.git
	echo $ setx SUM_TOOLS "C:\full\path\to\sum_tools"
	exit /b 1
)

@setlocal
@REM Change to the script directory (necessary when invoked from another .bat file)
cd /d "%~dp0"

@REM The script will view the compiled document with SumatraPDF by default.
@REM To avoid this, you may invoke the script with the "silent" argument
@set sad_viewer=y

@REM The script will view the generated html documentation with the Default Browser.
@REM To avoid this, you may invoke the script with the "silent" argument
@set dd_viewer=y

@REM The script will view the generated html documentation with the Default Browser.
@REM To avoid this, you may invoke the script with the "silent" argument
@set path_dd=".\output\html\"

@if "%1"=="silent" (
	set sad_viewer=n
	set dd_viewer=n
)

@PATH=%SUM_DOC_TOOLS%\MiKTeX\texmfs\install\miktex\bin\x64;%PATH%

@REM Run Doxygen for updating the Detailed Design
@REM make sure you have doxygen as System Path variable
del /Q "%path_dd%\*.*"
doxygen Doxyfile
@if %errorlevel% neq 0 exit /b 1


@if "%dd_viewer%"=="y" (
	@start "" ".\output\html\index.html"
)


@endlocal