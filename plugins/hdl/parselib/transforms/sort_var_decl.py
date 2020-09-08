from parselib.transforms import TopDown


class SortVarDecl(TopDown):
    """This pass sorts the variable declaration in modules based on their reference dependencies.
    The tools might require that a variable is declared before used.
    """
    def __top_sort_helper(self, u, edges, vis, res):
        vis[u] = 1  # visiting
        if u in edges:
            forbid = list(filter(lambda x: vis[x] == 1, edges[u]))
            assert len(forbid) == 0, 'Graph is not DAG'
            nxt = list(filter(lambda x: vis[x] == 0, edges[u]))
            for v in nxt:
                self.__top_sort_helper(v, edges, vis, res)
        vis[u] = 2  # done
        res.append(u)

    def __top_sort(self, edges):
        """sort nodes topologically"""
        in_deg = dict()
        nodes = set()
        for u, e in edges.items():
            nodes = nodes.union(set(e))
            nodes.add(u)
        for n in nodes: in_deg.setdefault(n, 0)
        for u, e in edges.items():
            for v in e:
                in_deg[v] += 1
        init = list(filter(lambda x: in_deg[x] == 0, nodes))
        vis = dict(map(lambda x: (x, 0), nodes))
        res = []
        assert init, 'Graph is not DAG'
        for u in init:
            if vis[u] == 0:
                self.__top_sort_helper(u, edges, vis, res)
        return res

    def modportsiglist(self, tree):
        # Detects dependency
        var_dict = dict()
        edge = dict()
        for idx, ch in enumerate(tree.children):
            if ch.data == 'vardeclinit':
                var_name = ch.children[0]
                var_dict[str(var_name)] = (idx, ch)
                if len(ch.children) == 3:
                    dep = ch.children[2]
                    if dep.data == 'hvarref':
                        prev = dep.children[0]
                        edge.setdefault(var_name, list()).append(prev)

        if len(edge) > 0:
            sorted_noes = self.__top_sort(edge)
            indices = sorted(list(map(lambda x: var_dict[x][0], sorted_noes)))
            for idx, n in zip(indices, sorted_noes):
                tree.children[idx] = var_dict[n][1]
        return tree