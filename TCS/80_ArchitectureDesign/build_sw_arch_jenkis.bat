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

py %SUM_TOOLS%\parser_arxml\jenkis\update_port_intf_csv.py --out "./tables" --json "./interfaces/port_interfaces_list.json" --csv_size 9 2 3 3 8
@if %errorlevel% neq 0 exit /b 1

@REM parse xpt nd SWCD in arxml
py %SUM_TOOLS%\parser_arxml\runnable_parser.py --input "../../20_Implementation/scripts/templates/TCS_Cfg_SWCD_arxml.xpt" --out_json "./runnables/out/json" --log "./runnables/out/log" --out_latex "./generated/runnables" --cal "./parameters/in"
@if %errorlevel% neq 0 exit /b 1

py %SUM_TOOLS%\parser_arxml\container_parser.py --paramdef "../../20_Implementation/scripts/paramdef/SUM_TCS_EcuParamDef.arxml" --table "./tables" --json "./containers/out/json" --uml "./diagrams" --log "./containers/out/log" --csv_size 5 3 2 4
@if %errorlevel% neq 0 exit /b 1

py %SUM_TOOLS%\parser_arxml\parameter_parser.py --paramdef "../../20_Implementation/scripts/paramdef/SUM_TCS_EcuParamDef.arxml" --input_cal "./parameters/in/calibration_parameter.txt" --out_json "./parameters/out/json" --out_latex "./generated" --log "./parameters/out/log"
@if %errorlevel% neq 0 exit /b 1

py %SUM_TOOLS%\parser_arxml\references_parser.py --input "../../20_Implementation/scripts/paramdef/SUM_TCS_EcuParamDef.arxml" --log "./parameters/references" --out "./generated/references"
@if %errorlevel% neq 0 exit /b 1

@REM Generate LaTeX variables, tables, ...
python %SUM_TOOLS%\generate_latex\generate_latex_variables.py "config.py"
@if %errorlevel% neq 0 exit /b 1
python %SUM_TOOLS%\generate_latex\generate_latex_tables.py --escape
@if %errorlevel% neq 0 exit /b 1
powershell %SUM_TOOLS%\generate_latex\plantUML2PDF.ps1
@if %errorlevel% neq 0 exit /b

@REM Compile the document
texify.exe -I %SUM_TOOLS%\latex_styles --clean --pdf "RTA-SUM_TCS_Software_Architecture_Document.tex"
@if %errorlevel% neq 0 exit /b 1

@if "%viewer%"=="y" (
	@start %SUM_DOC_TOOLS%\SumatraPDF.exe "RTA-SUM_TCS_Software_Architecture_Document.pdf"
)
@endlocal