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


def alternate_ids(tree, ops):
    """Change the ids within a tree, given operations ops as an array of lambdas"""
    ids = get_ids_in_tree(tree)
    if len(ops) != len(ids):
        raise ValueError('ops should have the same length as ids')
    for idx, _ in enumerate(ids):
        ops[idx](ids[idx])

