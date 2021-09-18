import warnings

from lark import Token

from parselib.transforms import TopDown
from ..utils import dprint, is_tree_type
from ..grammar import UnexpectedHCodeStructureError


class StructureCollector(TopDown):
    """Collect structural information, such as:
    module_name: list of port name

    We could leverage this to facilitate generation of assign of ports
    """
    def __init__(self):
        super().__init__()
