import yaml
import sys
import os
from argparse import ArgumentParser as AP
from pathlib import Path

parser = AP("package_xo - command generation")
parser.add_argument("yaml", type=Path, help="YAML description of the interface")
# parser.add_argument("--xo", required=True, help="Location of xo")
parser.add_argument("-o", type=Path, help="Location of the output tcl script")
parser.add_argument("-d", "--dir", type=Path, help="Location of the working root", default=Path(os.getcwd()))

def generate_xo_script(desc, path_to_packaged, path_to_tmp_project, xo, tcl=None):
    if tcl:
        out = tcl.open("w+")
    else:
        out = sys.stdout
    # print(desc)
    # print("path_to_packaged: {}".format(path_to_packaged))
    lines = []


    lines.append("create_project -part xczu3eg-sbva484-1-i -force kernel_pack \"{}\"".format(path_to_tmp_project))
    files = map(lambda x: str(Path(x).resolve()), desc['files'])
    lines.append("add_files -norecurse [list {}]".format(" ".join(files)))

    if 'xdc' in desc:
        xdcs = map(lambda x: str(Path(x).resolve()), desc['xdc'])
        lines.append("add_files -norecurse [list {}]".format(" ".join(xdcs)))

    lines.append("set_property top {} [current_fileset]".format(desc["kernel_name"]))
    lines.append("update_compile_order -fileset sources_1")
    lines.append("update_compile_order -fileset sim_1")
    # lines.append("synth_design -top {} -mode out_of_context -retiming".format(desc["kernel_name"]))
    lines.append("ipx::package_project -root_dir {} -vendor xilinx.com -library RTLKernel -taxonomy /KernelIP -import_files -set_current false".format(path_to_packaged))
    lines.append("ipx::unload_core {}/component.xml".format(path_to_packaged))
    lines.append("ipx::edit_ip_in_project -upgrade true -name tmp_edit_project -directory {}  {}/component.xml".format(path_to_packaged, path_to_packaged))

    lines.append("set core [ipx::current_core]")

    lines.append("set_property core_revision 2 $core")
    lines.append("foreach up [ipx::get_user_parameters] {")
    lines.append("  ipx::remove_user_parameter [get_property NAME $up] $core")
    lines.append("}")

    lines.append("set_property sdx_kernel true [ipx::current_core]")
    lines.append("set_property sdx_kernel_type rtl [ipx::current_core]")
    lines.append("set_property vitis_drc {ctrl_protocol ap_ctrl_none} [ipx::current_core]")
    lines.append("set_property ipi_drc {ignore_freq_hz true} [ipx::current_core]")

    default_clk = desc["clock"]
    rc = desc["reset"].get("clock", default_clk)
    lines.append("ipx::associate_bus_interfaces -clock {} -reset {} [ipx::current_core]".format(rc, desc["reset"]["name"]))
    # default_rst_level = "ACTIVE_LOW"
    # rl = desc["reset"].get("polarity", default_rst_level)
    # lines.append("set_property value {} [ipx::get_bus_parameters POLARITY -of_objects [ipx::get_bus_interfaces {} -of_objects [ipx::current_core]]]".format(rl, desc["reset"]["name"]))
    for intf_name, intf_info in desc['bus_interfaces'].items():
        lines.append("ipx::add_bus_interface {} [ipx::current_core]".format(intf_name))
        if intf_info['bus_type'] == 'stream':
            lines.append("set_property abstraction_type_vlnv xilinx.com:interface:axis_rtl:1.0 [ipx::get_bus_interfaces {} -of_objects [ipx::current_core]]".format(intf_name))
        else:
            raise RuntimeError("bus_type: {} not supported".format(intf_info['bus_type']))
        if_mode = intf_info['interface_mode']
        assert if_mode in ['slave', 'master'], "interface_mode can only be slave or master"
        lines.append("set_property bus_type_vlnv xilinx.com:interface:axis:1.0 [ipx::get_bus_interfaces {} -of_objects [ipx::current_core]]".format(intf_name))
        lines.append("set_property interface_mode {} [ipx::get_bus_interfaces {} -of_objects [ipx::current_core]]".format(if_mode, intf_name))
        for bus_part, signal_name in intf_info['port_map'].items():
            lines.append("ipx::add_port_map {} [ipx::get_bus_interfaces {} -of_objects [ipx::current_core]]".format(bus_part, intf_name)) 
            lines.append("set_property physical_name {} [ipx::get_port_maps {} -of_objects [ipx::get_bus_interfaces {} -of_objects [ipx::current_core]]]".format(signal_name, bus_part, intf_name))
        c = intf_info.get('clock', default_clk)
        lines.append("ipx::associate_bus_interfaces -busif {} -clock {} [ipx::current_core]".format(intf_name, c))


    lines.append("set_property supported_families { } $core")
    lines.append("set_property auto_family_support_level level_2 $core")
    lines.append("ipx::create_xgui_files $core")
    lines.append("ipx::update_checksums $core")
    lines.append("ipx::check_integrity -kernel $core")
    lines.append("ipx::save_core $core")
    lines.append("close_project -delete")

    lines.append("if {[file exists \"" + xo + "\"]} {")
    lines.append("    file delete -force \"{}\"".format(xo))
    lines.append("}")

    lines.append("package_xo -ctrl_protocol user_managed -xo_path {} -kernel_name {} -ip_directory {}".format(xo, desc['kernel_name'], path_to_packaged))

    out.writelines('\n'.join(lines))

    if tcl:
        out.close()


def main(args):
    d = args.dir.resolve()
    name = args.yaml.resolve().stem
    p = name
    dn = d / ("tmp_" + p)
    xo = d / (p + ".xo")
    if args.o:
        tcl = Path(args.o)
    else:
        tcl = args.yaml.resolve().with_suffix(".tcl")
    with args.yaml.open() as f:
        desc = yaml.safe_load(f)
    generate_xo_script(desc, path_to_packaged=str(d / "package"), path_to_tmp_project=str(dn), xo=str(xo), tcl=tcl)

if __name__ == '__main__':
    args = parser.parse_args()
    main(args)
