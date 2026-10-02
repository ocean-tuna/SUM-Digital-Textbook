
cls

@echo off 

SET TEMP_PATH=%PATH%
SET PATH=C:\Python311;C:\Python311\Scripts;%PATH%

py generate_confd_info.py

cd /d "%~dp0"
sphinx-build . ..\OUT\demo\RTA-SUM_SPM clean
sphinx-build . ..\OUT\demo\RTA-SUM_SPM -E

..\OUT\demo\RTA-SUM_SPM\index.html

SET PATH=%TEMP_PATH%
