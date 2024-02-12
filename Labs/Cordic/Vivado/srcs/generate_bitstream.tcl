create_project CORDIC ./prj -part xc7z020clg400-1
set_property IP_REPO_PATHS {../Vitis_HLS/IP} [get_filesets sources_1]
source ./srcs/system_block_design.tcl
make_wrapper -files [get_files ./prj/cordic/cordic.bd] -top -import -force
launch_runs synth_1
wait_on_run synth_1

launch_runs impl_1 -to_step write_bitstream
wait_on_run impl_1
exit

