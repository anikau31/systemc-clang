import re
import argparse


def transform_to_glyphs(value):
    table = {'True': '✔︎ OK', 'False': '✘ Failed'}
    if value in table:
        return table[value]
    else:
        return value


def report(data, output):
    """report the status, hard-coded to what is reported"""
    with open(output, 'w') as f:
        f.writelines('## Instructions to generate the report\n')
        f.writelines('In systemc-clang build folder: ')
        f.writelines('`python -B run-verilog-tests.py -m verilog`\n')
        f.writelines('and this will generate a `log.csv` file\n')
        f.writelines('\n')
        f.writelines('Then use the `report-from-csv.py` in this folder: `python -B report-from-csv.py --input-csv path/to/log.csv --output-md status.md`')
        f.writelines('\n')
        f.writelines("| Test Name | systemc-clang/xlat | hcode2verilog.py | hdl.txt syntax | transformation |\n")
        f.writelines("|:---------:|:------------------:|:----------------:|:--------------:|:--------------:|\n")
        for k, v in data.items():
            f.writelines(f"|{k}|{v['xlat_run']}|{v['convert_run']}|{v['convert_syntax']}|{v['convert_transform']}|\n")


def load_data(filename):
    res = {}
    with open(filename, 'r') as f:
        for line in f.readlines():
            m = re.match(r'(?P<test>[a-zA-Z_0-9\[\]]*),(?P<property>[a-zA-Z_0-9]*),(?P<value>[a-zA-Z_0-9]*)', line)
            assert m, "Record should match pattern"
            r = res.setdefault(m.group('test'), {})
            r[m.group('property')] = transform_to_glyphs(m.group('value'))
    return res


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--input-csv', required=True, help='input csv file name')
    parser.add_argument('--output-md', required=True, help='output markdown file name')
    args = parser.parse_args()

    res = load_data(args.input_csv)
    report(res, args.output_md)
    



if '__main__' == __name__:
    main()
