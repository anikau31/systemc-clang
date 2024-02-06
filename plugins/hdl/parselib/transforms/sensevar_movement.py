from lark import Tree
from parselib.transforms import TopDown
from parselib.transforms.node import TypeDefType
from ..utils import dprint, is_tree_type, get_ids_in_tree_types, get_ids_in_tree, alternate_ids, set_ids_in_tree_dfs
from pprint import pprint
from copy import deepcopy


class SensevarMovement(TopDown):
    """This pass moves sensevar declaration within a for loop to the top of the module, 
    and generate a generate block for each sensevar"""
    def __init__(self):
        super().__init__()
        self.is_in_initblock = False
        self.is_for_stmt = False
        self.only_sensevar = False
        self.current_for_stmt = None
        self.current_module = None
        self.initblock_vardecls = []

    def __decl_referred_in_sensvar(self, sensvar, vardecl):
        var_ids = get_ids_in_tree_types(vardecl, ['vardeclrn'])
        sensvar_ids = get_ids_in_tree_types(sensvar, ['hvarref'])
        if set(var_ids).intersection(set(sensvar_ids)):
            return True
        return False

    def __create_sensevar_generate_block(self, tree):
        assert is_tree_type(tree, "hmodule"), "tree should be a hmodule when creating generate blocks for sensitivity variables"
        if self.sensevar_dict == {}: return
        genblk = Tree("hgenerateblock", [ ])
        external_decl = set()
        for_list = []
        for sensvar, (process, for_stmt) in self.sensevar_dict.items():
            # create a generate block for each sensevar
            for vardecl in self.initblock_vardecls:
                if self.__decl_referred_in_sensvar(sensvar, vardecl):
                    external_decl.add(vardecl)

            process_node = self.processes.get(process, None)
            assert process_node is not None, f"sensvar {sensvar} defined for unknown process {process}"
            for_stmt_duplicate = deepcopy(for_stmt)
            proc = deepcopy(process_node)
            setattr(proc, 'force_sensevar', sensvar)
            for_stmt_duplicate.children[3].children = [proc]
            for_list.append(for_stmt_duplicate)

        genblk.children = [Tree("hgenvardecl", list(external_decl))] + for_list
        tree.children.append(genblk)

    def hmodule(self, tree):
        self.current_module = tree.children[0].value
        self.initblock_vardecls = []
        self.sensevar_dict = {}
        self.processes = dict()

        self.__push_up(tree)

        self.__create_sensevar_generate_block(tree)

        self.current_module = None
        self.sensevar_dict = {}
        self.initblock_vardecls = []
        return tree

    def hprocess(self, tree):
        # we don't need to push up the process for now.
        self.processes[tree.children[0].value] = tree
        return tree

    def hmodinitblock(self, tree):
        self.is_in_initblock = True
        self.__push_up(tree)
        tree.children = list(filter(lambda x: x is not None, tree.children))  # we might remove for loops
        self.is_in_initblock = False
        return tree

    def vardecl(self, tree):
        if self.is_in_initblock:
            self.__push_up(tree)
            self.initblock_vardecls.append(tree)
            return tree
        else:
            self.__push_up(tree)
            return tree

    def forstmt(self, tree):
        self.is_for_stmt = True
        self.only_sensevar = False
        self.current_for_stmt = tree

        for_loop_sensevar = self.is_in_initblock and self.is_for_stmt

        self.__push_up(tree)
        init, cond, step, body = tree.children
        

        self.is_for_stmt = False
        self.current_for_stmt = None
        if self.only_sensevar:
            self.only_sensevar = False
            return None  # In this case we simply remove the for loop statement
        else:
            self.only_sensevar = False
            return tree


    def stmt(self, tree):
        if self.is_for_stmt and self.is_in_initblock:
            is_sensvar = map(lambda x: is_tree_type(x, "hnamedsensvar") , tree.children)
            if all(is_sensvar):
                self.only_sensevar = True
                self.__push_up(tree)
                # In this case, all the sensevar declaration should be moved to the top of the module
                return Tree("hnoop", [])
            elif any(is_sensvar):
                raise ValueError("sensevar declaration should be the only statement in for loop init block, if it contains a sensevar declaration")
            else:
                self.__push_up(tree)
                return tree
        else:
            self.__push_up(tree)
            return tree

    def hnamedsensvar(self, tree):
        for_loop_sensevar = self.is_in_initblock and self.is_for_stmt
        assert for_loop_sensevar, "named sensevar is only allowed in for loop init block"

        process = tree.children[0].value
        sensevar = tree.children[1]
        self.sensevar_dict[sensevar] = (process, self.current_for_stmt)

        # remove the sensevar declaration from the original for loop
        return None

