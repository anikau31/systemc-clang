"""Utility library"""
from inspect import currentframe, getframeinfo, stack
import os
from lark import Tree

def tidify(verilog, current_indent = 0, indent_width = 2):
    """makes the generated verilog looks a bit better, may be subject to changes later"""
    add_indent_pattern = re.compile(r'(^(module|if|always|for|case)|(\: begin))')
    sub_indent_pattern = re.compile(r'^(endmodule|end)')
    sub_add_indent_pattern = re.compile(r'^(\)\;|end else begin)')
    res = []
    for l in verilog.splitlines():
        if add_indent_pattern.search(l):
            s = ' ' * (current_indent) + l
            current_indent += indent_width
        elif sub_add_indent_pattern.search(l):
            s = ' ' * (current_indent - indent_width) + l
        elif sub_indent_pattern.search(l):
            current_indent -= indent_width
            s = ' ' * (current_indent) + l
        else:
            s = ' ' * current_indent + l
        # print(s)
        res.append(s)

    return '\n'.join(res)


debug = True
def p(decorated):
    """a decorator that helps printing out the transformation results"""
    if debug:
        def wrapper(self, args):
            print(f'[DBG] {decorated.__name__}: \n{args} \n\n')
            res = decorated(self, args)
            print(f'[DBG] returns: {res}\n')
            return res
        return wrapper
    else:
        return decorated


def dprint(*arg, **kwargs):
    """debug utility for printing, prints line number"""
    frameinfo = currentframe()
    caller = getframeinfo(stack()[1][0])
    print(os.path.basename(caller.filename), ': L', frameinfo.f_back.f_lineno, ":", "\u001b[31m", *arg, "\u001b[0m", **kwargs)

def terminate_with_no_trace():
    assert False

def is_tree_type(t, name):
    """Check whether t is lark Tree and whether the tree type is name"""
    return isinstance(t, Tree) and t.data == name


def is_tree_types(t, names):
    """Check whether t is lark Tree and whether the tree type is name"""
    if not isinstance(names, list):
        raise ValueError('name argument should be list')
    return isinstance(t, Tree) and t.data in names


def get_ids_in_tree(tree):
    """get all ids"""
    __id_types = ['hvarref']
    if not isinstance(tree, Tree):
        raise ValueError('Only Tree type is accepted')
    res = []
    for t in tree.iter_subtrees():
        if is_tree_types(t, __id_types):
            assert t.children[0], 'hvarref should only contain one children'
            res.append(t.children[0])
    return res

def get_ids_in_tree_types(tree, types=['hvarref']):
    """get all ids"""
    __id_types = types
    if not isinstance(tree, Tree):
        raise ValueError('Only Tree type is accepted')
    res = []
    for t in tree.iter_subtrees():
        if is_tree_types(t, __id_types):
            assert t.children[0], 'hvarref should only contain one children'
            res.append(t.children[0])
    return res


def get_tree_types(tree, types=['hvarref']):
    """get all ids"""
    __id_types = types
    if not isinstance(tree, Tree):
        raise ValueError('Only Tree type is accepted')
    res = []
    for t in tree.iter_subtrees():
        if is_tree_types(t, __id_types):
            res.append(t)
    return res

def get_ids_in_tree_dfs(tree):
    """get all ids"""
    # __id_types = ['hvarref']
    # if not isinstance(tree, Tree):
    #     raise ValueError('Only Tree type is accepted')
    # res = []
    # for t in tree.iter_subtrees():
    #     if is_tree_types(t, __id_types):
    #         assert t.children[0], 'hvarref should only contain one children'
    #         res.append(t.children[0])
    # return res

    res = []
    dfs_stack = list()
    dfs_stack.append(tree)
    i = 0
    while len(dfs_stack) != 0:
        t = dfs_stack.pop(0)
        for idx in range(len(t.children)):
            nxt = t.children[idx]
            if isinstance(nxt, Tree):
                dfs_stack.append(nxt)
            elif is_tree_types(t, __id_types):
                assert t.children[0], 'hvarref should only contain one children'
                res.append(t.children[0])
    return res


def set_ids_in_tree_dfs(tree, ids):
    __id_types = ['hvarref']
    dfs_stack = list()
    dfs_stack.append(tree)
    i = 0
    while len(dfs_stack) != 0:
        t = dfs_stack.pop(0)
        for idx in range(len(t.children)):
            nxt = t.children[idx]
            if isinstance(nxt, Tree):
                dfs_stack.append(nxt)
            elif is_tree_types(t, __id_types):
                t.children[idx] = ids[i](t.children[idx])
                i += 1


def alternate_ids(tree, ops):
    """Change the ids within a tree, given operations ops as an array of lambdas"""
    ids = get_ids_in_tree(tree)
    if len(ops) != len(ids):
        raise ValueError('ops should have the same length as ids')
    for idx, _ in enumerate(ids):
        ops[idx](ids[idx])

def map_hvarref_ids(tree, ops):
    """get all and apply mapping function"""
    __id_types = ['hvarref']
    if not isinstance(tree, Tree):
        raise ValueError('Only Tree type is accepted')
    # res = []
    idx = 0
    for t in tree.iter_subtrees():
        if is_tree_types(t, __id_types):
            assert len(t.children) == 1, 'hvarref should only contain one children'
            mapped_token = ops[idx](t.children[0])
            assert mapped_token, 'mapping function should return a token'
            t.children[0] = mapped_token
            idx += 1
            # res.append(t.children[0])


class ContextManager(object):
    def __init__(self):
        self.stack = []

    def __getattr__(self, key):
        if key in self.__dict__:
            return self.__dict__[key]
        for d in reversed(self.stack):
            if key in d:
                return d[key]
        return None

    def search_key_in_outer_context(self, key):
        if len(self.stack) <= 1:
            return None

        for d in reversed(self.stack[:-1]):
            if key in d:
                return d[key]
        return None

    def add_values(self, **kwargs):
        self.stack.append(kwargs)
        return self

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.stack.pop()