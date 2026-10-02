@REM Generate the Doxygen documentation
doxygen "Doxyfile"
@if %errorlevel% neq 0 exit /b 1
