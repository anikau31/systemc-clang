"""Compound type"""
# aggregated type?
# Aggregated types better created within the class


class aggregate(object):
    def __init__(self, params, fields):
        self.fields = fields
        self.params = params
        pass

    def to_str(self, var_name, context=None):
        stmts = []
        idx = 0
        for x, t in self.fields:
            pref = var_name + '_' + x
            if idx == len(self.fields) - 1:
                if context and context.suffix:
                    context.suffix = ''
            stmts.append(t.to_str(pref, context=context))
            idx = idx + 1
        # assert context is not None
        # TODO: need some context for input/output
        return '\n'.join(stmts)


