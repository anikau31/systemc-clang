import warnings
from .top_down import TopDown
from ..utils import dprint
from lark import Tree, Token
import copy


class FunctionParamMarker(TopDown):
    def __init__(self):
        """
        This pass marks the function parameters as input/output or inout, so that the synthesis tool can handle
        function parameters correctly
        """
        super().__init__()
        self.expanded = list()
        self.__is_in_function = False
        self.__driven_signals = set()

    def __clear_driven_signals(self):
        self.__driven_signals = set()

    def __record_driven_signal(self, sig_name):
        """This function records which signal is driven in the process"""
        self.__driven_signals.add(sig_name)

    def __get_driven_sig_name(self, tree: Tree):
        """
        determines which signal is the signal that is being driven
        currently, it returns the first hvarref node, which is true.
        But this might be changed at a later point
        """
        for t in tree.iter_subtrees_topdown():
            if type(t) == Tree and t.data == 'hvarref':
                return t.children[0]

    def hbinop(self, tree):
        """binary op"""
        if self.__is_in_function:
            if tree.children[0] == '=':
                sig = self.__get_driven_sig_name(tree)
                self.__record_driven_signal(sig)
        return tree

    def blkassign(self, tree):
        """block assignment"""
        if self.__is_in_function:
            sig = self.__get_driven_sig_name(tree)
            self.__record_driven_signal(sig)
        return tree

    def hfunction(self, tree):
        self.__is_in_function = True
        self.__clear_driven_signals()
        self.__push_up(tree)
        self.__is_in_function = False

        # mark the input/output direction of the function parameters
        funct_param = tree.children[2]
        for param in funct_param.children:
            # param = vardeclinit
            par_name = param.children[0]
            param_type = 'sc_in'
            if par_name in self.__driven_signals:  # determines whether such signal is driven within the function body
                param_type = '__func_inout'

            param = param.children[1]
            # param = htypeinfo
            original_param_children = copy.deepcopy(param.children)
            param.children = [Tree('htype', children=[Token('TYPESTR', param_type), *original_param_children])]

        return tree

    def hfunctionparams(self, tree):
        """identify the input/output direction of the function params"""
        return tree


