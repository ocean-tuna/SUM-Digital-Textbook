
# Edit only this parameter
component = "SPM"

name = f"SUM_{component}"

traceability_report_folder = r"..\trace_generated"
traceability_out_file = f"{component}_Traceability_report.txt"
reqs = ["DDS"]

config_data = {
    "input_path" : "./../",
    "input_pdfs" : [rf'SRS_{component} Trace Summary.pdf', rf'DDS_{component} Trace Summary.pdf', rf'ADS_{component} Trace Summary.pdf'],
    "output_path": '.',
    "output_pdf" : f'RTA-SUM_{component}_trace_summary.pdf'
}