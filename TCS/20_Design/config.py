document_type = r"Design Document"
project = r"RTA-SUM"
module = r"Timing Client Service (TCS)"
project_number = r"0841-0004"
author = "SUM Development Team(ETAS/ERS-Tn)"
status = "Released"
version = r"8.0.1"
module_shortname = r"TCS"

#########################################################################################################
latex_variables = {
    "DocumentTitle": "{project} - {module}".format(project=project, module=module),
    "DocumentSubtitle": document_type,
    "ModuleShortname": module_shortname,
    "ProjectName": project,
    "ProjectNumber": project_number,
    "DocumentAuthor": author,
    "DocumentStatus": status,
    "DocumentVersion": version,
}