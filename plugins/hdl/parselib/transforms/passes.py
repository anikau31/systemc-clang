"""Different pass of the translation"""
from .alias_translation import AliasTranslation
from .literal_expansion import LiteralExpansion
from .node_merge import NodeMergePass
from .sort_var_decl import SortVarDecl
from .typedef_expansion import TypedefExpansion
from .typedef_filter import TypeDefFilter
from .verilog_tranlation import VerilogTranslationPass
from .port_expansion import PortExpansion
from .slice_merge import SliceMerge
from .node_movement import NodeMovement


class VerilogTranslator:
    """Translate hcode to verilog"""
    @staticmethod
    def translate(tree):
        prev = tree
        prev = NodeMovement().visit(prev)
        prev = SortVarDecl().visit(prev)
        prev = AliasTranslation().visit(prev)
        prev = LiteralExpansion().visit(prev)
        prev = SliceMerge().visit(prev)
        f = TypeDefFilter()
        prev = f.visit(prev)
        prev = NodeMergePass().visit(prev)
        prev = PortExpansion().visit(prev)
        # note typedef should be after port expansion to prevent duplicate valid/ready
        prev = TypedefExpansion(f.types).visit(prev)
        prev = VerilogTranslationPass().visit(prev)
        return prev

