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

call DocsAsCode\build_dac_document.bat silent
@if %errorlevel% neq 0 exit /b 1

call python %SUM_TOOLS%\release_package\create_release_package.py "%~dp0/release_package.json"

@endlocal