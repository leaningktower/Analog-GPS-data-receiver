// look in pins.pcf for all the pin names on the TinyFPGA BX board
module top (
    //input
    input wire clk,
    input wire rst,
    input wire dataclkrst,
    input wire codesign,
    input wire ifin,
    input wire codetrack,
    input wire doptrack,
    //SPI
    input wire sck, en, mosi,
    output miso,
    //output
    // output CAcode, ditherCAcode, ditherCAcode_m,
    output ditherCAcode,
    // output epoch, epoch_m,
    output epoch,
    // output dataclk, dataclk_m,
    output dataclk,
    // output dither,
    // output codeclk,
    // output codeNCOud,
    output pwm,
    // output data, data_m,
    output data,
    // output ifedge,
    //USB
    output USBPU           // USB pull-up resistor
);
    // IO switch board
    assign ditherCAcode_m = ditherCAcode;
    assign epoch_m = epoch;
    assign dataclk_m = dataclk;
    assign data_m = data;
    // drive USB pull-up resistor to '0' to disable USB
    assign USBPU = 0;
    // input interface
    wire [4:0] satset;
    wire [11:0] dopset;
    wire [9:0] pwmset;
    wire [11:0] iffreq;
    wire [9:0] pwmvalue;
    SPIslave keyboardcomm (
        .clk(clk),
        .SCK(sck),
        .SSEL(en),
        .MOSI(mosi),
        .MISO(miso),
        .iffreq(iffreq),
        .pwmvalue(pwmvalue),
        .satset(satset),
        .dopset(dopset),
        .pwmset(pwmset)
    );
    // CA code generator
    CAtracker codetracker (
        .clk(clk),
        .rst(rst),
        .codesign(codesign),
        .codetrack(codetrack),
        .dataclkrst(dataclkrst),
        .sat(satset),
        .dither(dither),
        .CAcode(CAcode),
        .ditherCAcode(ditherCAcode),
        .epoch(epoch),
        .dataclk(dataclk),
        .ci(codeclk),
        .codeNCOud(codeNCOud),
        .data(data)
      );
    dopplertracker doptracker(
        .clk(clk),
        .rst(rst),
        .dopset(dopset),
        .pwmset(pwmset),
        .ifin(ifin),
        .codetrack(codetrack),
        .doptrack(doptrack),
        .iffreq(iffreq),
        .pwmvalue(pwmvalue),
        // .ifedge(ifedge),
        .pwm(pwm)
    );
endmodule
