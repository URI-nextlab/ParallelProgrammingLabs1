`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/01/2023 01:47:50 PM
// Design Name: 
// Module Name: led_ctl
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

module led_ctl (
  // Write side inputs
  input            clk_rx,       // Clock input
  input            rst_clk_rx,   // Active HIGH reset - synchronous to clk_rx

  input            btn_clk_rx,   // Button to swap low and high pins

  input      [7:0] rx_data,      // 8 bit data output
                                 //  - valid when rx_data_rdy is asserted
  input            rx_data_rdy,  // Ready signal for rx_data

  output reg [3:0] led_o         // The LED outputs
);


//***************************************************************************
// Parameter definitions
//***************************************************************************

//***************************************************************************
// Reg declarations
//***************************************************************************

  reg             old_rx_data_rdy;
  reg  [7:0]      char_data;
   reg  [3:0]     led_pipeline_reg;

//***************************************************************************
// Wire declarations
//***************************************************************************

//***************************************************************************
// Code
//***************************************************************************

  always @(posedge clk_rx)
  begin
    if (rst_clk_rx)
    begin
      old_rx_data_rdy <= 1'b0;
      char_data       <= 8'b0;
      led_o           <= 8'b0;
    end
    else
    begin
      // Capture the value of rx_data_rdy for edge detection
      old_rx_data_rdy <= rx_data_rdy;

      // If rising edge of rx_data_rdy, capture rx_data
      if (rx_data_rdy && !old_rx_data_rdy)
      begin
        char_data <= rx_data;
      end

      // Output the normal data or the data with high and low swapped
      if (btn_clk_rx)
        led_pipeline_reg <= char_data[3:0]^char_data[7:4];
      else
        led_pipeline_reg <= char_data[3:0];
    end // if !rst
        
        led_o <= led_pipeline_reg;
  end // always


endmodule
