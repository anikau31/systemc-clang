# sed -i "s/^.*module divider_sc_module_2 (.*$/  \(\* use_dsp = \"yes\" \*\) &/"  $1
sed -i "s/^.*module moving_average_sc_module_1 (.*$/  \(\* use_dsp = \"yes\" \*\) &/"  $1

sed -i "s/^.*module divider_sc_module_1 (.*$/ \(\* use_dsp = \"yes\" \*\) &/" $1
