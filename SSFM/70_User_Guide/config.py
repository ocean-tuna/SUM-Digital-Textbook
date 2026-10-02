document_type = r"User Guide"
project = r"RTA-SUM"
module = r"Signal Status and Fault Monitor (SSFM)"
version = r"8.0.1\_rc"
reviewer = "Ippazio Martella (ETAS/ERS-Tn)"
reviewer_title = "ETAS Hub Project Manager"
status = "Released"
is_plugin = True

#########################################################################################################
latex_variables = {
    "DocumentTitle": "{project} - {product} {version}".format(project=project, product=module, version=version),
    "DocumentSubtitle": document_type,
    "DocumentStatus": status,
    "DocumentReviewer": reviewer,
    "DocumentReviewerTitle": reviewer_title,
    "DocumentGroup": is_plugin,
}