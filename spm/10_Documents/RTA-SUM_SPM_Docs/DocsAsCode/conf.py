# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information
import os
import sys
from pathlib import Path

project = 'RTA-SUM SPM'
copyright = '2024, ETAS GmbH'
author = 'RTA-SUM Team'
release = '8.1.0'

#The below configuration makes the architectural diagrams clickable
plantuml_output_format ='svg_obj'
# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

sys.path.insert(0, os.path.abspath('.'))
sys.path.insert(0, os.path.abspath('./filters'))

extensions = [
    'sphinx_rtd_theme',
    'sphinx_needs',
    'sphinxcontrib.plantuml'
    #'sphinxcontrib.test_reports'
]

templates_path = ['_templates']
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = [
    '_out',
    'Thumbs.db',
    '.DS_Store',
    '_doctool'
]
needs_id_regex= '(\[\$SRS_.*\]|CAR_FNC_REQ_.*|GM-SDV.*)'

#####################################################################################################
#
# PlantUML settings
#
#####################################################################################################

plantuml_cfg = Path("_static/plantuml/plantumlstyle.cfg").resolve()
plantuml_output_format ='svg_obj'

if Path("_tools/plantuml.jar").exists():
    plantuml_jar = Path("_tools/plantuml.jar").resolve()
elif "PLANTUML_JAR" in os.environ:
    plantuml_jar = os.environ["PLANTUML_JAR"]
elif os.getenv("plantuml_jar"):
    plantuml_jar = Path(os.getenv("plantuml_jar")).resolve()
elif Path('/usr/share/plantuml/plantuml.jar').exists():
    plantuml_jar = Path('/usr/share/plantuml/plantuml.jar').resolve()
elif Path("_doctool/plantuml/plantuml.jar").exists():
    plantuml_jar = Path("_doctool/plantuml/plantuml.jar").resolve()
elif Path('C:/PlantUML/plantuml.jar').exists():
    plantuml_jar = Path('C:/PlantUML/plantuml.jar').resolve()
else:
    raise RuntimeError("Could not find plantuml.jar, please check the README.md")
java = "java"
if Path("_doctool/openjdk/bin/java.exe").exists():
    java = Path("_doctool/openjdk/bin/java.exe").resolve()
    

plantuml = f'{java} -Djava.awt.headless=true -jar {plantuml_jar}'

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
html_theme = 'sphinx_rtd_theme'
html_logo = 'ETAS_Logo_White.png'

html_theme_options = {
    'style_nav_header_background': 'linear-gradient(to right, #164293, #89037A)',
}

# Enable C syntax code highlighting
pygments_style = 'sphinx'


# -- Options for Sphinx-Needs ------------------------------------------------


# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']
html_css_files = [ 'css/custom.css']

needs_role_need_template = "[${id}] : {title}"
# cross-check with SUM about used directives (currently only built-in req used)
# if setting needs_tpyes the built-in directory is overwritten -> check with SUM
needs_types = [
#     dict(directive="software_requirement", title="Software Requirement", prefix="BSW_SWCS_", color="#ac6dd1", style="artifact"),
#     dict(directive="system_requirement", title="System Requirement", prefix="sysreq_", color="#1e539e", style="node"),
#     dict(directive="sth_requirement", title="Stakeholder Requirement", prefix="sthreq_", color="#1e539e", style="node"),
     dict(directive="functional_requirement", title="Functional Requirement", prefix="", color="#71162d", style="node"),
     dict(directive="req", title="Requirement", prefix="", color="#71162d", style="node"),
     dict(directive="dds", title="Detailed Requirement", prefix="", color="#71162d", style="node"),
     dict(directive="sth_requirement", title="Stakeholder Requirement", prefix="", color="#1e539e", style="node"),
 ]

# List of additional needs attributes
needs_extra_options = [
    'rreqs_tag',
    'source',
    'category',                   
    'sub_category',                    
    'source_id',                    
    'test_technique',                  
    'comments',
    'source_doc',
    'doc_version',
    'last_update'
]


# Define own link types
needs_extra_links = [
    { 
        "option": "checks",
        "incoming": "is checked by",
        "outgoing": "checks" 
    },
    { 
        "option": "implements",
        "incoming": "is implemented by",
        "outgoing": "implements" 
    },
    {
        "option": "satisfies",
        "incoming": "is_satisfied_by",
        "outgoing": "satisfies"
    },
    {
        "option": "is_satisfied_by",
        "incoming": "satisfies",
        "outgoing": "is_satisfied_by"
    },
    {
        "option": "related_to",
        "incoming": "related from",
        "outgoing": "related to"
    },
    {
        "option": "derived_from",
        "incoming": "derives",
        "outgoing": "derived from"
    }
]

needs_id_required = True
#####################################################################################################
#
# DocLink service configuration
#
#####################################################################################################


try:
    from dotenv import load_dotenv
    if Path(".env").exists():
       print("Loading env")
       load_dotenv()
    
except:
    print("Library python-dotenv not available in your toolchain - please upgrade the toolchain")

auth_tenant_id = os.getenv("AUTH_TENANT_ID")
auth_client_id = os.getenv("AUTH_CLIENT_ID")
auth_client_secret = os.getenv("AUTH_CLIENT_SECRET")
print(auth_tenant_id)
if auth_tenant_id and auth_client_id and auth_client_secret:
    
    print(auth_tenant_id)
    # DocLink Setup (Expose ARCANA Data to other release trains)
    from sne_doclink_service import DoclinkService
    from sne_doclink_service.resources.helpers import Mode

    doclink_options = {}
    if len(os.getenv("DOCLINK_WRITE_CONTAINER", "")) > 0:
        doclink_options["write_container"] = os.getenv("DOCLINK_WRITE_CONTAINER")

    if len(os.getenv("DOCLINK_WRITE_CONFIGURATION", "")) > 0:
        doclink_options["write_configuration"] = os.getenv("DOCLINK_WRITE_CONFIGURATION")

    if len(os.getenv("DOCLINK_BASE_URL", "")) > 0:
        doclink_options["write_artifact_base_url"] = os.getenv("DOCLINK_BASE_URL")

    doclink_options["global_filters"] = {
        "container_name": [
            "arc-system-requirements",
            "ar-spec"
        ]
    }

    if len(os.getenv("DOCLINK_READ_GLOBAL_CONFIGURATION", "")) > 0:
        doclink_options["global_filters"]["global_configuration"] = os.getenv(
            "DOCLINK_READ_GLOBAL_CONFIGURATION"
        )

    needs_services = {
        # To fix warning described here: https://github.com/useblocks/sphinxcontrib-needs/issues/168
        # Cloud GitHub configuration
        "github-issues": {},
        "github-prs": {},
        "github-commits": {},
        "doclink": {
            "class": DoclinkService,
            "class_init": {
                "api_url": "https://xac.etas.com/doclink/boost/",
                "type_mapping": {
                    # Requirements
                    "req": "req-sw",
                    "functional_requirement": "req-sys",
                    "sth_requirement": "req-stake",
                },
                "read_mode": Mode.always_activated,
                "write_mode": Mode.on_ci,
                "read_page_size": 2000, 
                "write_page_size": 10000,
                "read_timeout": 120,
                # links option value in sphinx-needs: link attribute name in Doclink service
                "link_mapping": {
                #    "links": "implements",
                #    "verified_by": "verified_by",
            	    "is_satisfied_by": "satisfied_by", # source Doors
                  	"satisfies": "satisfies", # source SWCS
                #    "implemented_by": "implemented_by",
                },
                # TODO XAC-607
                "auth": {
                    "tenant_id": auth_tenant_id,
                    "client_id": auth_client_id,
                    "client_secret": auth_client_secret,
                #    "refresh_token": os.getenv("AUTH_REFRESH_TOKEN")
                },
                "proxies": {
                    "http": os.getenv("HTTP_PROXY"),
                    "https": os.getenv("HTTPS_PROXY"),
                },
                **doclink_options,
            },
        },

    }