`timescale 1ns/1ps
/*
	@ File		: test.v
	@ Author 	: User
	@ Data 		: 2022-02-10
	@ Version	:
	@ Brief		:
*/
module test_tb();
//************************Do Not Remove************************//
	initial begin
		$dumpfile("test_tb.vcd");
		$dumpvars();
	end
//*************************************************************//

reg[8:0] i;
reg [11:0] bcd;
wire [11:0] test_output;
reg [3:0] error_count;

bin2bcd bin2bcd_inst(
	.bin(i[7:0]),
	.bcd(test_output)
);

initial begin
	error_count = 0;
	for(i = 0;i <= 255; i = i + 1) begin
		bcd[3:0] = i % 10;
		bcd[7:4] = (i / 10) % 10;
		bcd[11:8] = (i / 100) % 10;
		#10;
		if(test_output != bcd) begin
			error_count = error_count + 1;
			$display("integer %d : hdl output [%d %d %d], correct [%d %d %d]\n", 
											i,
											test_output[11:8], test_output[7:4], test_output[3:0],
											bcd[11:8], bcd[7:4], bcd[3:0]
					);
		end		
	end
	if(error_count == 0)
		$display("Test pass!\n");
	else
		$display("%d error occurred while testing hdl module\b");
	$finish();
end

endmodule
