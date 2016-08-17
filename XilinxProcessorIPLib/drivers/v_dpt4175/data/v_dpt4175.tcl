# ==============================================================
# File generated by Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC
# Version: 2016.1
# Copyright (C) 1986-2016 Xilinx, Inc. All Rights Reserved.
#
# ==============================================================

proc generate {drv_handle} {
    xdefine_include_file $drv_handle "xparameters.h" "XV_dpt4175" \
        "NUM_INSTANCES" \
        "DEVICE_ID" \
        "C_S_AXI_CTRL_BASEADDR" \
        "C_S_AXI_CTRL_HIGHADDR" \
        "DYNAMIC_BPC" \
        "MAX_BPC" \
        "SAMPLES_PER_CLOCK" \
        "M_PKT_DET" \
        "MAX_COLS" \
        "MAX_ROWS"

    xdefine_config_file $drv_handle "xv_dpt4175_g.c" "XV_dpt4175" \
        "DEVICE_ID" \
        "C_S_AXI_CTRL_BASEADDR" \
        "DYNAMIC_BPC" \
        "MAX_BPC" \
        "SAMPLES_PER_CLOCK" \
        "M_PKT_DET" \
        "MAX_COLS" \
        "MAX_ROWS"

    xdefine_canonical_xpars $drv_handle "xparameters.h" "XV_dpt4175" \
        "DEVICE_ID" \
        "C_S_AXI_CTRL_BASEADDR" \
        "C_S_AXI_CTRL_HIGHADDR" \
        "DYNAMIC_BPC" \
        "MAX_BPC" \
        "SAMPLES_PER_CLOCK" \
        "M_PKT_DET" \
        "MAX_COLS" \
        "MAX_ROWS"
}
