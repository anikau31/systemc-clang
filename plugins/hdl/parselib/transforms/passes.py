"""Different pass of the translation"""
from .alias_translation import AliasTranslation
from .literal_expansion import LiteralExpansion
from .node_merge import NodeMergePass
from .sort_var_decl import SortVarDecl
from .typedef_expansion import TypedefExpansion
from .typedef_filter import TypeDefFilter, TypeDefCleanup
from .verilog_tranlation import VerilogTranslationPass
from .port_expansion import PortExpansion
from .slice_merge import SliceMerge
from .node_movement import NodeMovement
from .function_param_marker import FunctionParamMarker
from .reorder_mod_init_block import ReorderModInitBlock
from .function_info_pass import FunctionInfoPass, FunctionInfoPass2
from .function_transformation_pass import FunctionTransformationPass
from .comma_transformation import CommaTransformation
from .structure_collector import StructureCollector


class VerilogTranslator:
    """Translate hcode to verilog"""
    @staticmethod
    def translate(tree):
        # we need some form of `level' of the tree, lower level meaning it is loosing more information, but more lenient
        # to hardware language
        prev = tree
        prev = ReorderModInitBlock().visit(prev)
        prev = NodeMovement().visit(prev)
        prev = SortVarDecl().visit(prev)
        prev = AliasTranslation().visit(prev)
        sc = StructureCollector()
        sc.visit(prev)
        prev = LiteralExpansion(structure=sc.hier).visit(prev)
        prev = SliceMerge().visit(prev)
        prev = CommaTransformation().visit(prev)
        f = TypeDefFilter()
        prev = f.visit(prev)
        prev = NodeMergePass().visit(prev)
        prev = PortExpansion().visit(prev)
        # note typedef should be after port expansion to prevent duplicate valid/ready
        prev = TypedefExpansion(f.types).visit(prev)
        prev = FunctionInfoPass().visit(prev)
        prev = FunctionInfoPass2().visit(prev)
        prev = FunctionParamMarker().visit(prev)
        prev = FunctionTransformationPass().visit(prev)
        prev = TypeDefCleanup().visit(prev)
        prev = VerilogTranslationPass().visit(prev)
        return prev

