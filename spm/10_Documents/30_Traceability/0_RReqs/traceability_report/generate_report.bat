@if not defined SUM_TOOLS (
	echo SUM_TOOLS environment variable not defined!
	echo Please set it to the location of your local copy of the sum_tools repository:
	echo e.g.:
	echo $ git clone https://bitbucket.etas-dev.com/scm/gmsum/sum_tools.git
	echo $ setx SUM_TOOLS "C:\full\path\to\sum_tools"
	exit /b 1
)

@REM Generate traceability analysis report
python %SUM_TOOLS%\test_and_traceability\hybrid_traceability_analysis.py "config.py"
@if %errorlevel% neq 0 exit /b 1

@endlocal