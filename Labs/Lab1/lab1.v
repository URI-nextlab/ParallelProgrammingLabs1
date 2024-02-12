`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 08/31/2023 02:15:31 PM
// Design Name: 
// Module Name: lab1
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


module lab1(
    input [3:0] btn,
     output [3:0] led
    );
    
    assign led[0] = ~btn[0];
    assign led[1] = btn[1] & ~btn[2];
    assign led[3] = btn[2] & btn[3];
    assign led[2] = led[1] | led[3];
     
endmodule
