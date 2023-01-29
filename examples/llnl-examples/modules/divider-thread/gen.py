from jinja2 import Template, StrictUndefined
from pathlib import Path

j2_template = Template(Path("divider-pipeline.h.tpl").read_text())


print(j2_template.render())
