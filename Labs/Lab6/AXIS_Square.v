`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/08/2023 10:46:47 AM
// Design Name: 
// Module Name: AXIS_Square
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module AXIS_Square(			
// Declare the attributes above the port declaration
(* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 axis_reset_n RST" *)
// Supported parameter: POLARITY {ACTIVE_LOW, ACTIVE_HIGH}
// Normally active low is assumed.  Use this parameter to force the level
(* X_INTERFACE_PARAMETER = "POLARITY ACTIVE_LOW" *)
input axis_reset_n, //  (required)

// Declare the attributes above the port declaration
(* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 axis_clk CLK" *)
// from the top BD.
(* X_INTERFACE_PARAMETER = "ASSOCIATED_BUSIF S_AXIS_Square, ASSOCIATED_RESET axis_reset_n, FREQ_HZ 125000000, FREQ_TOLERANCE_HZ 0" *)
input s_axis_clk, //  (required)


// Declare the attributes above the port declaration
(* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 axis_clk CLK" *)
// from the top BD.
(* X_INTERFACE_PARAMETER = "ASSOCIATED_BUSIF M_AXIS_Square, ASSOCIATED_RESET axis_reset_n, FREQ_HZ 125000000, FREQ_TOLERANCE_HZ 0" *)
input m_axis_clk, //  (required)
			
		

//  input [<left_bound>:0] <s_tdest>,
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 S_AXIS_Square TDATA" *)
  input [31:0] S_AXIS_Square_TDATA, // Transfer Data (optional)
//  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 <interface_name> TSTRB" *)
//  input [31:0] <s_tstrb>,)
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 S_AXIS_Square TKEEP" *)
  input [3:0] S_AXIS_Square_TKEEP, 
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 S_AXIS_Square TLAST" *)
  input S_AXIS_Square_TLAST, 
//  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 <interface_name> TUSER" *)
//  input [31:0] <s_tuser>, 
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 S_AXIS_Square TVALID" *)
  input S_AXIS_Square_TVALID, 
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 S_AXIS_Square TREADY" *)
  output S_AXIS_Square_TREADY, 
  
  //MAXIS
    (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 M_AXIS_Square TDATA" *)
  output [31:0] M_AXIS_Square_TDATA, 
//  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 <interface_name> TSTRB" *)
//  input [31:0] <s_tstrb>
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 M_AXIS_Square TKEEP" *)
  output [3:0] M_AXIS_Square_TKEEP, 
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 M_AXIS_Square TLAST" *)
  output M_AXIS_Square_TLAST, // Packet Boundary Indicator (optional)
//  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 <interface_name> TUSER" *)
//  input [31:0] <s_tuser>
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 M_AXIS_Square TVALID" *)
  output M_AXIS_Square_TVALID, 
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 M_AXIS_Square TREADY" *)
  input M_AXIS_Square_TREADY 
//  additional ports here
);

//  user logic here
assign M_AXIS_Square_TDATA = S_AXIS_Square_TDATA << 1;
assign M_AXIS_Square_TKEEP = S_AXIS_Square_TKEEP;
assign M_AXIS_Square_TLAST = S_AXIS_Square_TLAST;
assign M_AXIS_Square_TVALID = S_AXIS_Square_TVALID;
assign S_AXIS_Square_TREADY = M_AXIS_Square_TREADY;
endmodule
			
		
