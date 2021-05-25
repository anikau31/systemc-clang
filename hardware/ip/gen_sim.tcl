export_simulation \
  -of_objects [get_files $ip_name.xci] \
  -simulator xsim \
  -ip_user_files_dir "${IP_USER_FILES}" \
  -ipstatic_source_dir "${IP_STATIC}" \
  -use_ip_compiled_libs \
  -force


# -of_objects [get_files $ip_name.xci] \
