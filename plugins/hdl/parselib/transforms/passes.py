"""Different pass of the translation"""
from .alias_translation import AliasTranslation
from .literal_expansion import LiteralExpansion, LiteralExpansion2
from .node_merge import NodeMergePass
from .sort_var_decl import SortVarDecl
from .typedef_expansion import TypedefExpansion #, BlkAssignTypeDefExpansion
from .typedef_filter import TypeDefFilter, TypeDefCleanup
from .verilog_tranlation import VerilogTranslationPass
from .port_expansion import PortExpansion
from .slice_merge import SliceMerge
from .node_movement import NodeMovement, ArrayPortMovement
from .function_param_marker import FunctionParamMarker
from .reorder_mod_init_block import ReorderModInitBlock
from .function_info_pass import FunctionInfoPass, FunctionInfoPass2
from .function_transformation_pass import FunctionTransformationPass
from .comma_transformation import CommaTransformation
from .structure_collector import StructureCollector
from .sensevar_movement import SensevarMovement
from .portbinding_recollect import PortbindingRecollect, PortbindingPrecheck, PortDirectionCollector, LowerComplexPort
from .interface_generation import InterfaceGeneration, InterfaceReplacement

from ..utils import dprint, terminate_with_no_trace


from parselib.transforms import TopDown

class PrettyPrintModule(TopDown):
    def __init__(self, name):
        self.name = name

    def hmodule(self, tree):
        if tree.children[0].value == self.name:
            dprint(tree.pretty())
            assert False
            return
        return tree


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
        # prev = CommaTransformation().visit(prev)
        # dprint(prev.pretty())
        f = TypeDefFilter()
        prev = f.visit(prev)
        prev = NodeMergePass().visit(prev)
        # prev = ArrayPortMovement().visit(prev)
        
        prev = PortExpansion().visit(prev)
        prev = TypedefExpansion(f.types).visit(prev)

        ig = InterfaceGeneration()
        prev = ig.visit(prev)
        # terminate_with_no_trace()
        # PrettyPrintModule('fifo_cc_sc_module_11').visit(prev)
        # terminate_with_no_trace()
        # prev = BlkAssignTypeDefExpansion(f.types).visit(prev)
        prev = SensevarMovement().visit(prev)
        prev = FunctionInfoPass().visit(prev)
        prev = FunctionInfoPass2().visit(prev)
        prev = FunctionParamMarker().visit(prev)
        prev = FunctionTransformationPass().visit(prev)
        prev = TypeDefCleanup().visit(prev)

        # PrettyPrintModule('fifo_cc_sc_module_11').visit(prev)
        # terminate_with_no_trace()

        port_directions = PortDirectionCollector()
        port_directions.visit(prev)
        # PrettyPrintModule('encode_sc_module_1').visit(prev)
        # terminate_with_no_trace()



        prev = PortbindingPrecheck().visit(prev)
        prev = PortbindingRecollect(ports=port_directions.ports).visit(prev)  # this pass should only work when PreCheck passes
        prev = LowerComplexPort(ig.interface_meta_data).visit(prev)

        prev=  LiteralExpansion2().visit(prev)
        prev = InterfaceReplacement(ig.interface_meta_data).visit(prev)
        # PrettyPrintModule('decode_sc_module_1').visit(prev)
        # terminate_with_no_trace()
        # PrettyPrintModule('encode_block_sc_module_5').visit(prev)
        # terminate_with_no_trace()

        # PrettyPrintModule('rvfifo_cc_sc_module_9').visit(prev)
        prev = VerilogTranslationPass(itf_meta=ig.interface_meta_data).visit(prev)
        # terminate_with_no_trace()
        return prev

