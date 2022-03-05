`timescale 1ns/1ps
/*
	@ File		: bin2bcd.v
	@ Author 	: User
	@ Data 		: 2022-02-10
	@ Version	:
	@ Brief		:
*/
module bin2bcd(
	input	[7:0]			bin,
	output	[11:0]			bcd

);
	wire 	[3:0] 			add3_inst0_out;
	wire 	[3:0] 			add3_inst1_out;
	wire 	[3:0] 			add3_inst2_out;
	wire 	[3:0] 			add3_inst3_out;
	wire 	[3:0] 			add3_inst4_out;
	wire 	[3:0] 			add3_inst5_out;
	wire 	[3:0] 			add3_inst6_out;
	
	assign bcd[11:8] = {2'b0,add3_inst5_out[3],add3_inst6_out[3]};
	assign bcd[7:4] = {add3_inst6_out[2:0],add3_inst4_out[3]};
	assign bcd[3:0] = {add3_inst4_out[2:0], bin[0]};
	add3 add3_inst0
	(
		.in({1'b0,bin[7:5]}),
		.out(add3_inst0_out)
	);
	add3 add3_inst1
	(
		.in({add3_inst0_out[2:0],bin[4]}),
		.out(add3_inst1_out)
	);
	add3 add3_inst2
	(
		.in({add3_inst1_out[2:0],bin[3]}),
		.out(add3_inst2_out)
	);
	add3 add3_inst3
	(
		.in({add3_inst2_out[2:0],bin[2]}),
		.out(add3_inst3_out)
	);
	add3 add3_inst4
	(
		.in({add3_inst3_out[2:0],bin[1]}),
		.out(add3_inst4_out)
	);
	add3 add3_inst5
	(
		.in({1'b0,add3_inst0_out[3],add3_inst1_out[3],add3_inst2_out[3]}),
		.out(add3_inst5_out)
	);
	add3 add3_inst6
	(
		.in({add3_inst5_out[2:0],add3_inst3_out[3]}),
		.out(add3_inst6_out)
	);
endmodule
