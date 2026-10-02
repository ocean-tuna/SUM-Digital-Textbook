document_type = r"User Guide"
project = r"RTA-SUM"
module = r"Diagnostic Application Service (DIAG)"
version = r"8.0.1\_Pr24"
reviewer = "Ippazio Martella (ETAS/ERS-Tn)"
reviewer_title = "ETAS Hub Project Manager"
status = "released"
is_plugin = False

#########################################################################################################
#if is_plugin=="true":doc_group=1 else:doc_group=0
latex_variables = {
    "DocumentTitle": "{project} - {product} {version}".format(project=project, product=module, version=version),
    "DocumentSubtitle": document_type,
    "DocumentStatus": status,
    "DocumentReviewer": reviewer,
    "DocumentReviewerTitle": reviewer_title,
    "DocumentGroup": is_plugin,
}