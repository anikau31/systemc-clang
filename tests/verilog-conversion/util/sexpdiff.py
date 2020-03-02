import difflib
from itertools import tee

def sexpdiff(this_filename, that_filename):
    # Test the equality of golden versus the generated
    with open(this_filename, 'r') as f:
        sexp_lines = f.readlines()
    with open(that_filename, 'r') as f:
        golden_lines = f.readlines()

    diff_res, diff_res_str = tee(difflib.ndiff(golden_lines, sexp_lines))
    diff_only = list(filter(lambda x: x[0] != ' ', diff_res))
    if diff_only:
        diff_str = ''.join(list(diff_res_str))
    else:
        diff_str = None
    return len(diff_only) != 0, diff_str

