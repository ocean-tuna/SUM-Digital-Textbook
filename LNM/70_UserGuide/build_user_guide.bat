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
@set viewer=y

@if "%1"=="silent" (
	set viewer=n
)

@PATH=%SUM_DOC_TOOLS%\MiKTeX\texmfs\install\miktex\bin\x64;%PATH%
rm .\generated\config.tex
@REM Generate LaTeX variables, tables, ...
python %SUM_TOOLS%\generate_latex\generate_latex_variables.py "config.py"
@if %errorlevel% neq 0 exit /b 1
python %SUM_TOOLS%\generate_latex\generate_latex_tables.py --escape
@if %errorlevel% neq 0 exit /b 1

@REM Compile the document
texify.exe -I %SUM_TOOLS%\latex_styles --clean --pdf "RTA-SUM_LNM_User_Guide.tex"
@if %errorlevel% neq 0 exit /b 1

@if "%viewer%"=="y" (
	@start %SUM_DOC_TOOLS%\SumatraPDF.exe "RTA-SUM_LNM_User_Guide.pdf"
)
@endlocal