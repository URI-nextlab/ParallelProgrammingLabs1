`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 09/01/2023 01:47:02 PM
// Design Name: 
// Module Name: uart_led
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
module uart_led (
  // Write side inputs
  input            clk_pin,      // Clock input (from pin)
  input            rst_pin,      // Active HIGH reset (from pin)
  input            btn_pin,      // Button to swap high and low bits
  output     [3:0] led_pins      // 8 LED outputs
);

//***************************************************************************
// Parameter definitions
//***************************************************************************
  parameter BAUD_RATE           = 115_200;   
  parameter CLOCK_RATE          = 125_000_000;

//***************************************************************************
// Reg declarations
//***************************************************************************

//***************************************************************************
// Wire declarations
//***************************************************************************

  // Synchronized reset
  wire             rst_clk_rx;

  wire            rxd_pin;      // RS232 RXD pin - directly from pin
  // Synchronized button
  wire             btn_clk_rx;

  // Between uart_rx and led_ctl
  wire [7:0]       rx_data;      // Data output of uart_rx
  wire             rx_data_rdy;  // Data ready output of uart_rx
  
//***************************************************************************
// Code
//***************************************************************************
assign rxd_pin = 1;
  // Metastability harden the rst - this is an asynchronous input to the
  // system (from a pushbutton), and is used in synchronous logic. Therefore
  // it must first be synchronized to the clock domain (clk_pin in this case)
  // prior to being used. A simple metastability hardener is appropriate here.
  meta_harden meta_harden_rst_i0 (
    .clk_dst      (clk_pin),
    .rst_dst      (1'b0),    // No reset on the hardener for reset!
    .signal_src   (rst_pin),
    .signal_dst   (rst_clk_rx)
  );

  // And the button input
  meta_harden meta_harden_btn_i0 (
    .clk_dst      (clk_pin),
    .rst_dst      (rst_clk_rx),
    .signal_src   (btn_pin),
    .signal_dst   (btn_clk_rx)
  );

  uart_rx #(
    .CLOCK_RATE   (CLOCK_RATE),
    .BAUD_RATE    (BAUD_RATE) 
  ) uart_rx_i0 (
    .clk_rx      (clk_pin),
    .rst_clk_rx  (rst_clk_rx),

    .rxd_i       (rxd_pin),
    .rxd_clk_rx  (),
    
    .rx_data_rdy (rx_data_rdy),
    .rx_data     (rx_data),
    .frm_err     ()
  );

  led_ctl led_ctl_i0 (
    .clk_rx      (clk_pin),
    .rst_clk_rx  (rst_clk_rx),
    .btn_clk_rx  (btn_clk_rx),
    .rx_data     (rx_data),
    .rx_data_rdy (rx_data_rdy),
    .led_o       (led_pins)
  );

    ila_led ila_led_i0 (
        .clk(clk_pin), // input wire clk
    
    
        .probe0(rx_data_rdy), // input wire [0:0]  probe0  
        .probe1(led_pins) // input wire [3:0]  probe1
    );
endmodule
