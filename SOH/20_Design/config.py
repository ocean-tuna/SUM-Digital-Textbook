document_type = r"Design Document"
project = r"RTA-SUM"
project_number = r"10884"
module = r"State of Health Manager (SOH)"
module_shortname = r"SOH"
version = r"8.0.1"
author = "Development Team"
status = "Released"

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