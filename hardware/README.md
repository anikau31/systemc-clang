# SystemC-Clang hardware flow

This folder contains scripts and code that help to push the systemc-clang through the hardware flow in Vivado

## Directory structure

- `app`: software
- `bd`: block designs
- `rtl`: synthesizable rtl design
- `verif`: verification related code
- `ip`: ips to initiate in the design
- `constr`: constraints such as IO, floor-planning and debugging

## Related targets

The hardware flow creates cmake targets that generate stuff such as design checkpoints and simulation.

