import os
from jinja2 import Template
import yaml

rst_f = """
SPM Configuration
------------------

.. csv-table:: The description of configuration parameters are the following:
    :header: "Container Name", "Description"
    :file: containers.csv
    :widths: 10, 25

{%- for container in data.Containers %}
    {% if 'Parameters' in container %}
{{ container.Name }} parameter definition
{{ '^' * (container.Name|length + ' parameter definition '|length) }}

.. csv-table:: The description of configuration parameters for {{ container.name }}:
    :header: "Parameter", "Description", "Type", "Multiplicity"
{% for val in create_uml(container)[1] %}
    {{ val }}
{%- endfor %}
    {%- endif %}
{%- endfor %}

"""

def create_rst( name: str, dst: str ) :
    with open(os.path.join(dst, name), 'w', encoding='iso-8859-1') as fw :
        fw.write(rst_f)

def create_uml( container : dict ) -> str:
    desc_name = []
    new_yaml = []
    val = {}

    for param in container['Parameters'] :
        print("{} --> {}".format(container["Name"], param["Name"]))
        desc_name.append("\"{}\", \"{}\", \"{}\", \"{}\"".format(param['Name'], param['Description'] if 'Description' in param else "NULL", param["Type"], param['Multiplicity']))
        for k in param :
            if k != 'Description' and k != 'ReqDef' and k != 'ReqSat' :
                val[k] = param[k]
        new_yaml.append(val)
    
    return [yaml.dump(new_yaml), desc_name]

        


def main() :

    with open('../../../design_driven/Spm_paramConf.yaml', 'r', encoding='iso-8859-1') as fr:
        data = yaml.safe_load(fr)

    print(data)
    
    templ = Template(rst_f)

    out = templ.render({'data' : data, 'create_uml' : create_uml})

    with open('./csv/spm_container_view.rst', 'w', encoding='iso-8859-1') as fw:
        fw.write(out)

if __name__ == "__main__" :
    main()