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

copy .\interfaces\port_interfaces_list.json .\interfaces\port_interfaces_list_bk.json

py %SUM_TOOLS%\parser_arxml\jenkis\port_interface_parser.py --input "../../20_Implementation/scripts/templates/TCS_Cfg_SWCD_arxml.xpt" --log "./interfaces/log" --descr "./interfaces"
@if %errorlevel% neq 0 exit /b 1