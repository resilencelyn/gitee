@echo off
color 0a
"C:\iverilog\iverilog\bin\iverilog.exe" -o "test.o" "tb\test.v" "src\bin2bcd.v" "src\add3.v" 
"C:\iverilog\iverilog\bin\vvp.exe" "test.o" 
"C:\iverilog\iverilog\gtkwave\bin\gtkwave.exe" "test_tb.vcd" 
