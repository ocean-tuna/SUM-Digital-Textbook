# SRS DOC AS CODE

## Needs

- configparser (NOT_NEED FOR SPM)
- openpyxl (NOT_NEED FOR SPM)
- openpyxl_image_loader (NOT_NEED FOR SPM)
- sphinx

To install all together:

```py
pip install openpyxl openpyxl_image_loader configparser sphinx sphinx_rtd_theme sphinx_needs sphinxcontrib.plantuml sphinx-test-reports
```

## How to use

1) copy the excel SRS in this folder
2) modify the config.txt accordingly
3) run the main.py
4) run the build_srs.bat 
5) look in build/html , is index.html looking good ?
6) copy all except the main.py in the SRS folder of the project and try the traceability
7) good luck if you find more srs files, maybe just merge the index.rst content in one only file
8) from now on, just use rst