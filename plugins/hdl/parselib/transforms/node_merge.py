from parselib.transforms import TopDown


class NodeMergePass(TopDown):
    """
    This pass merges separate nodes that are created for easy recognition for grammar,
    but actually shares the same semantics
    """
    def hnsbinop(self, tree):
        self.__push_up(tree)
        tree.data = 'hbinop'
        return tree

    def __transform_arrayref_syscread(self, tree):
        if len(tree.children) == 2 and tree.children[1].data == 'hbinop' and tree.children[1].children[0] == 'ARRAYSUBSCRIPT':
            true_node = tree.children[1]
            true_node.children = true_node.children[1:]
            true_node.data = 'harrayref'
            return true_node
        return tree

    def blkassign(self, tree):
        """this handler fixes the a[b].read() case by removing read() and making it a[b]"""
        self.__push_up(tree)
        for idx, t in enumerate(tree.children):
            if t.data == 'syscread':
                tree.children[idx] = self.__transform_arrayref_syscread(t)
        # dprint(tree)
        return tree