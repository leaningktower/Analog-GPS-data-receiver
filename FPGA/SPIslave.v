module SPIslave (
    input wire clk,
    // SPI interface
    input wire SCK, SSEL, MOSI,
    output MISO,
    // Keyboard data IO
    input wire [11:0] iffreq,
    input wire [9:0]  pwmvalue,
    output [4:0]  satset,
    output reg [11:0] dopset,
    output reg [9:0]  pwmset
    // SPI interface
);
initial begin
  satset_r <= 6'd0;
  dopset   <= 12'd0;
  pwmset   <= 10'd0;
end
// SPI signal
reg [2:0] SCKr;  always @(posedge clk) SCKr <= {SCKr[1:0], SCK};
wire SCK_risingedge = (SCKr[2:1]==2'b01);
wire SCK_fallingedge = (SCKr[2:1]==2'b10);
reg [2:0] SSELr;  always @(posedge clk) SSELr <= {SSELr[1:0], SSEL};
wire SSEL_active = ~SSELr[1];
wire SSEL_startmessage = (SSELr[2:1] == 2'b10);
wire SSEL_endmessage = (SSELr[2:1] == 2'b01);
reg [1:0] MOSIr;  always @(posedge clk) MOSIr <= {MOSIr[0], MOSI};
wire MOSI_data = MOSIr[1];
// SPI rcv logic
reg [4:0] bitcnt;
reg byte_received;
reg [23:0] byte_data_received;
always @(posedge clk) begin
    if(~SSEL_active) begin
        bitcnt <= 0;
    end
    else if(SCK_risingedge) begin
        bitcnt <= bitcnt + 1;
        byte_data_received <= {byte_data_received[22:0], MOSI_data};
    end
end
always @(posedge clk) byte_received <= SSEL_active && SCK_risingedge && (bitcnt == 5'd23);
// SPI trs logic
reg [23:0] byte_data_sent;
always @(posedge clk) begin
    if(SSEL_active) begin
        if(SSEL_startmessage) begin
            if (last_byte_data_received[23:16] == 8'd4) begin
                byte_data_sent <= {8'd4, 6'd0, pwmvalue};
            end
            else if(last_byte_data_received[23:16] == 8'd5) begin
                byte_data_sent <= {8'd5, 4'd0, iffreq};
            end
        end
        else if(SCK_fallingedge) begin
            if (bitcnt > 0) begin
                byte_data_sent <= {byte_data_sent[22:0], 1'b0};
            end
        end
    end
end
assign MISO = byte_data_sent[23];
// Data management
reg[5:0] satset_r;
reg[23:0] last_byte_data_received;
always @(posedge clk) begin
    if(byte_received) begin
        last_byte_data_received <= byte_data_received;
        if (byte_data_received[23:16] == 8'd1) begin
            satset_r <= byte_data_received[5:0] - 5'd1;
        end
        else if(byte_data_received[23:16] == 8'd2) begin
            dopset <= byte_data_received[11:0];
        end
        else if(byte_data_received[23:16] == 8'd3) begin
            pwmset <= byte_data_received[9:0];
        end
    end
end
assign satset = satset_r[4:0];
endmodule
