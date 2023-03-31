import re
import sys, os
from pathlib import Path
from argparse import ArgumentParser as AP

parser = AP()
parser.add_argument("-i")
parser.add_argument("-o")
parser.add_argument("-s", default="&")

pattern = re.compile(r"\|\s+(?P<instance>[a-zA-Z0-9_]+)\s+\|\s+(?P<module>[a-zA-Z0-9_]+)\s+\|\s+(?P<total_luts>\d+)\s+\|\s+(?P<logic_luts>\d+)\s+\|\s+(?P<lutrams>\d+)\s+\|\s+(?P<srls>\d+)\s+\|\s+(?P<ffs>\d+)\s+\|\s+(?P<ramb36>\d+)\s+\|\s+(?P<ramb18>\d+)\s+\|\s+(?P<uram>\d+)\s+\|\s+(?P<dsp_blocks>\d+)\s+\|")

def table_head(sep):
    # table_head = "| Instance |                     Module                     | Total LUTs | Logic LUTs | LUTRAMs | SRLs | FFs | RAMB36 | RAMB18 | URAM | DSP Blocks |"
    table_head = "| Instance | LUTs | FFs | RAMB36 | RAMB18 | DSP Blocks |"

    # Split the table head string on the "|" character.
    # This will give us a list of strings, each containing one
    # of the table head names.
    names = table_head.split("|")

    # Loop through the names and remove any whitespace at the
    # beginning and end of each string.
    names = [name.strip() for name in names]

    # Filter out any empty strings (i.e. the vertical bars
    # that were used to separate the table head names).
    names = [name for name in names if name]

    # Join the names into a single string, separated by commas.
    names_string = "{} ".format(sep).join(names)

    return names_string


def main(args):
    if args.o:
        out = Path(args.o).open('w')
    else:
        out = sys.stdout
    f = Path(args.i).read_text()
    matched = pattern.search(f)  # we only need the first matching instance

    out.writelines("% % " + table_head(args.s) + " \\\\")
    out.writelines("\n")
    out.writelines(
        "{} ".format(args.s).join(
            list(map(lambda x: matched.group(x), ['instance', 'total_luts', 'ffs', 'ramb36', 'ramb18', 'dsp_blocks']))
        ) + " \\\\\n"
    )

    if args.o:
        out.close()


if __name__ == '__main__':
    args = parser.parse_args()
    main(args)
