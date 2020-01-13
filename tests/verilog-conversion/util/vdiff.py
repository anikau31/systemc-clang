"""Testing utilities"""
from io import StringIO

class DiffInfo(object):
    def __init__(self, lineno=None, diff_tuples=None, diff_nodes=None):
        info = [diff_tuples, diff_nodes]
        info = list(filter(lambda x: x is not None, info))
        assert len(info) <= 1, 'At most one type of diff is accepted'
        self.diff_tuples = diff_tuples
        self.diff_nodes = diff_nodes
        self.lineno = lineno

    def __str__(self):
        if self.diff_tuples:
            res = "# Different Attributes at line {}: ".format(self.lineno)
            res = res + '\n'.join("-   {}: {}\n+   {}: {}".format(str(k1), str(v1), str(k2), str(v2)) 
                    for k1, v1, k2, v2 in self.diff_tuples)
            return res
        if self.diff_nodes:
            def _get_str(node):
                buf = StringIO()
                node.show(buf=buf)
                return buf.getvalue()
            res = "# Different Children at line {}: \n".format(self.lineno)
            res += "# In the first file\n"
            res += ''.join(map(_get_str, self.diff_nodes[0])) + '\n'
            res += "# In the next file\n"
            res += ''.join(map(_get_str, self.diff_nodes[1]))
            return res
        return ""

class VerilogASTDiff(object):
    """interpret the diff of AST of verilog"""

    @staticmethod
    def _diff_traverse(this_node, that_node):
        """traverses two asts using dfs, stops when different node encountered"""
        if type(this_node) != type(that_node):
            return False

        # assuming each attr_name appear once in each node
        this_names = set(this_node.attr_names)
        that_names = set(that_node.attr_names)
        diff = []
        for a in this_names:
            this_attr = getattr(this_node, a)
            if a in that_names:
                that_attr = getattr(that_node, a)
                if this_attr != that_attr:
                    diff.append((a, this_attr, a, that_attr))
            else:
                diff.append((a, this_attr, '', ''))
        for a in that_names:
            that_attr = getattr(that_node, a)
            if a not in this_names:
                diff.append(('', '', a, that_attr))

        if diff:
            return DiffInfo(this_node.lineno, diff_tuples=diff)

        other_children = that_node.children()

        if len(this_node.children()) != len(other_children):
            return DiffInfo(this_node.lineno,
                    diff_nodes = [this_node.children(),
                    other_children])

        for i, c in enumerate(this_node.children()):
            res = VerilogASTDiff._diff_traverse(c, other_children[i])
            if res:
                return res

        return None


    @staticmethod
    def diff_info(this_verilog_ast, that_verilog_ast):
        """shows the diff information of two ast, including line number and node name"""
        return VerilogASTDiff._diff_traverse(this_verilog_ast, that_verilog_ast)

