![iv](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/iv.png)

## 😁介绍

iVerilog Assistant是一个**轻量级工具**，用于辅助各位朋友使用iVerilog进行仿真。本程序的定位是一个适合初学者、爱好者使用，适合小型项目构建和仿真验证的小助手，而不是做一套IDE或者一个功能复杂的Editor。小助手初版写于2019年，本科二年级的时候，是实验室FPGA课程的培训计划的一部分。这个软件使用两个学期下来发现不少问题，第二个版本在2020年底开发完，然后丢到这个仓库里面。期间也有很多朋友使用过这个软件，大部分都对操作逻辑有一些意见，在使用的过程中也发现一些功能上的Bug。2021年因为个人身体问题和学习安排，项目停更一年，直到2022年才重新开始对小助手收到的反馈做进一步修改。在2022版中我重构了大部分代码，修正了一些操作逻辑，以及对小助手作为一个工具本身的定位做了一些规划。由于我不是专门从事软件开发的，只学习过一些简单的C#编程，以及玩过一点点ACM的底子，小助手在编写的过程中基本是现学现卖，很多设想功能因为自己能力没办法实现，也挖个坑给自己以后填吧。

## 🎈说明

### 版本说明

小助手的版本是[年份]+一个[字母]，小助手若有重大更新（添加了什么喜大普奔的功能、Bug太多了推倒重构之类的），则会使用当年的年份作为版本号，否则将版本字母按照ASCII序号加一。

### 🎉开发动态

小助手目前正在尝试增加插件库，支持从iVerilog Assistant工程导出至intel Quartus、Vivado工程，支持TCL脚本管理和生成。在下一个版本中将加入对FPGA开发板管理，包括图形化和自动化的管脚分配、约束文件或TCL的生成。针对目前反馈的问题对模板和文档管理功能做出一定调整。

我与@alainchen大神在图形化界面和软件理念上做了一些讨论，其团队开发的“基于QT开发的可视化FPGA-EDA软件”拥有比iVerilog Assistant更为先进的设计思想。我也在向alainchen不断学习、请教，期望在易用性和理念上更进一步。

**[👉基于QT开发的可视化FPGA-EDA软件](https://gitee.com/eda-development/eda_fpga)**

### 仓库目录说明

目前仓库的目录结构如下：

```
-iVerilogAssistant\
    |-packed\
    |    |-...
    |-latest\
    |    |-assistant.exe
    |    |-verison
    |-demo_prj\
    |    |-...
    |-vs_project\
    |    |-...
```

|    目录    |                             说明                             |
| :--------: | :----------------------------------------------------------: |
|   packed   |                  打包好的小助手，安装就能用                  |
|   latest   | 最新的小助手主程序和版本说明（仅供自动更新使用，请勿手动下载） |
|  demo_prj  |               一个测试的demo，实现二进制转BCD                |
| vs_project |       小助手的VS工程，包含源代码（写的很垃圾求安慰🥺）        |

## 🚀使用

### 安装

从2022a开始，小助手发布的都会用一个7z自解压文件。各位使用的朋友需要注意🎉🎉🎉🎉：

**小助手的安装路径和工程路径内不应包含中文和空格！**

**小助手的安装路径和工程路径内不应包含中文和空格！**

**小助手的安装路径和工程路径内不应包含中文和空格！**

如果在使用过程中出现没有办法编译或者是仿真的情况，请检查安装路径是否有问题😡😡😡。这里推荐大家解压路径选择**C盘根目录**，解压完成后将assistant.exe添加一个快捷方式到桌面或者是开始菜单都可以~😀

小助手能正常运行，那么打开的主界面如下图所示。如果不一样那就凉了啊😔

![image-20220210005907543](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/image-20220210005907543.png)

**😍😍😍😍在小助手中，双击文件树可以打开相应的文件，双击project可以打开工程路径，双击source、testbech、document同样可以打开相应路径。**

### 配置

要让小助手能够正常工作，首先需要进行必要的配置。

![image-20220210035714059](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/image-20220210035714059.png)

首先设置用户名可以让生成的文件包含尊贵大名。小助手默认的编辑器是notepad，就是系统自带的记事本（因为总得有一个东西要打开代码）。小助手不集成编辑器的原因是每一个人都有自己写代码的习惯，相应的编辑器也有足够丰富的插件支撑快速编码。无论使用任何编辑器，都需要在此处配置路径。小助手内集成一套预编译的iverilog、GTKWave程序，默认参数**Internal**将使用内部所带的编译器和波形查看器，否则请指定外部编译器（例如自己编译的最新iverilog程序），并且指定的外部仿真器必须包含iverilog、vvp和GTKWave。

### 创建工程

点击Open/New按钮，选择New Project选项，填上工程名和路径，然后OK就可以了。

![image-20220210010329826](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/image-20220210010329826.png)

### 添加源代码和测试代码

点击New Files按钮，弹出添加文件对话框。

![image-20220210011026448](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/image-20220210011026448.png)

在该对话框内可以添加三种文件：verilog源代码、verilog测试脚本、说明文档（支持text、markdown、latex）。该对话框包含一个端口编辑器，可以在新建文件的时候就加上一些端口之类的，同时可以指定文件模板。右侧是一个代码预览，可以看看自动生成了什么东西。

此处添加一个名字为“haha.v"的源码文件。

![image-20220210011752461](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/image-20220210011752461.png)

这里有几个设置的参数，对照右边生成的代码就明白了。

此处添加一个叫”test.v"的测试文件**。注意测试文件的模块名程序不允许修改，默认是文件名加"_tb"。另外请不要更改vcd文件的名字，否则会找不到vcd文件弹不出波形！**（高手随意😀）

### 仿真

仿真之前请先指定【顶层模块】，和【测试代码】

![image-20220210013121463](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/image-20220210013121463.png)

😡😡😡**注意，一个工程里面至少需要有一个源代码和一个测试代码！！**

![image-20220210013525092](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/image-20220210013525092.png)

点击编译按钮，不出意外可以看到弹出绿字大黑窗（控制台），如果有报错信息，那么需要一条一条耐心修正，不断重新编译。如果编译成功会产生vcd文件，同时也会打印testbench中所有通过$display，$write的内容。

点击仿真按钮，如果选择产生vcd文件，可以看到弹出GTKWave，在GTKWave中可以观察相应信号的波形。

![image-20220210033236321](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/image-20220210033236321.png)

### 自定义模板

![image-20220210034600882](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/image-20220210034600882.png)

使用的朋友可以自定义模板，包括指定一些注释信息，时间、日期、文件名、作者等。小助手提供的模板编辑器目前支持以下几种MASK，在完成文件添加的时候，MASK会被替换为具有实际意义的文字。

|     MASK      |                说明                |      示例      |
| :-----------: | :--------------------------------: | :------------: |
|    &file&     |         将被替换为此文件名         |     led.v      |
|   &author&    | 将被替换为作者名（作者名可以设置） |   Sun Zhenyu   |
|    &date&     |          将被替换为年月日          | 2022 - 02 - 05 |
|    &port&     |   将被替换为端口编辑器生成的端口   |       /        |
| &module_name& |          将被替换为模块名          |      led       |
|   &tb_cfg&    |     将被替换为testbech配置文件     |       /        |

建立模板的时候可以在任意位置插入MASK，这意味着，如果MASK对应的字符与实际意义存在冲突（例如&date&在上下文中是女盆友的名字👩），程序将无法按照设想保留这些字符（会被替换为当天时间日期）。

## 😨灾难恢复

小助手不可避免的会出现抽风问题，可能会导致工程文件损坏无法打开，因此下面详细阐述小助手所依赖的文件构成。

### 配置文件config.ini

配置文件位于assistant.exe同目录下。默认的内容如下

```
username
User
editor_path
notepad.exe
iv_path
Internal
version
2022a
```

中第一行username是一个关键字，对应的用户名在第二行，在程序中小助手将读取第二行的字符串作为用户名。第三行为编辑器关键字，小助手将读取第四行作为双击打开的编辑器，iv_path是仿真器iverilog的路径，version是当前小助手的版本（版本号将影响升级检测）依此类推。目前配置相对比较简单，日后会逐渐增加配置项。

### 模板文件

模板文件位于templates文件夹内。当调用模板时，会首先读取配置**templates.ini**，其默认内容如下

```
src
1
Default Source Template.v
tb
1
Default Testbench Template.v
latex
1
Default Document Template.tex
ss1.tex
md
1
Default Document Template.md
txt
1
Default Document Template.txt
```

配置文件首行是表明模板类型，第二行表明该类模板数量，此后每一行会放置一个模板文件名，相应每一类模板会放置在对应的文件夹中。如果希望手动增加模板或者重命名模板，需要修改这个文件。

### 升级器update.exe

升级器为另一个使用C#开发的程序，点击小助手的升级按钮，将唤起升级器并杀死小助手进程。升级器的工作流程如下：

1. 检查config.ini文件的version字段，获取版本号
2. 读取url.ini中version字段后对应的http连接，获取远端version版本号（这个文件对应gitee仓库latest\verison）
3. 若版本号满足大于关系，读取url.ini中file字段后的内容，下载assistant.exe，并替换旧版本的assistant（对应这个文件对应仓库gitee仓库latest\assistant.exe）

目前仓库的目录对应的url.ini内容如下

```
version
https://gitee.com/sunzhenyu59/i-verilog-assistant/raw/master/latest/version
file
https://gitee.com/sunzhenyu59/i-verilog-assistant/raw/master/latest/assistant.exe
file_name
assistant.exe
```

这个应该不会变动。

### 工程文件

工程文件的后缀名为ivprj，这个文件包含了当前工程下的源码、测试代码、文档、顶层、活动测试代码。

例如某个工程文件的内容如下：

```
test_project
src
4
a.v
b.v
c.v
d.v
tb
3
q.v
w.v
t.v
doc
1
hello.txt
top
c.v
active_tb
w.v
```

那么表明a.v、b.v、c.v、d.v是该工程的源码，且c.v是顶层。q.v、w.v、t.v是测试文件，其中活动的测试文件是w.v，工程包含一个hello.txt。由于小助手与编辑器是独立的，小助手崩溃不影响编辑器工作，因此可以继续完成代码，但是工程文件可能需要手动修复。注意，如果工程未指定顶层和活动测试文件，那么用英文的!代替文件名，例如

```
top
!
active_tb
！
```

### 其他奇怪的灾难性崩溃

**劳烦在issue或者评论指出崩溃的情况🙏🙏🙏🙏。大部分时候重新下一个新的小助手覆盖原先配置就可以工作了。**

## DEMO工程：二进制转BCD

该工程位于demo_prj目录下，可以直接下载下来测一下看看。这里演示一下工程是怎么搭建的。

**✨STEP 1**

选择一个合适的位置放工程。

![在这里插入图片描述](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/bcb1bb2998fa484c8d761a1e82e5403a.png)

**✨STEP 2**

这里我们需要实现一个二进制转BCD码，博主思来想去辗转反侧寝食难安，决定使用加三法，因此需要建立一个加法器。加法器是一个纯组合逻辑电路，点击新建文件建立一个源码文件add3.v，填写输入和输出端口。

![在这里插入图片描述](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/2367ba1140db48a0be4301ea3d58f6d4.png)

有了加法器，我们需要对加法器进行组合才能输出一个完成转BCD码的操作，因此新建一个文件bin2bcd.v。

![在这里插入图片描述](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/ed011f5977044b028ee5305a8b56a50c.png)

注意这里的位宽输入是8位二进制，输出最多BCD码位宽是12位。添加完源码文件以后，目录树应该是这个样子的。

![在这里插入图片描述](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/c7967f80de4c49f18640f46bb8ae8b23.png)

**✨STEP 3**

双击里面的add3.v，填上以下代码：

```verilog
`timescale 1ns/1ps
/*
	@ File		: add3.v
	@ Author 	: User
	@ Data 		: 2022-02 -10
	@ Version	:
	@ Brief		:
*/
module add3(
	input	[3:0]			in,
	output	[3:0]			out

);
	reg [3:0] 				out;
	always @(*) begin
			case(in)
				4'b0000: out = 4'b0000;  // 0
				4'b0001: out = 4'b0001;  // 1
				4'b0010: out = 4'b0010;  // 2
				4'b0011: out = 4'b0011;  // 3
				4'b0100: out = 4'b0100;  // 4
				4'b0101: out = 4'b1000;  // 5
				4'b0110: out = 4'b1001;  //6
				4'b0111: out = 4'b1010;  //7
				4'b1000: out = 4'b1011;  //8
				4'b1001: out = 4'b1100;  //9
				default: out = 4'b0000;  //others, impossible
			endcase
		end
endmodule
```

双击bin2bcd.v，填上以下代码

```verilog
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
```

**✨STEP 4**

保存以后接下来新建测试文件。

![在这里插入图片描述](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/c598cc6309254f47b1d4974efdec4416.png)

默认测试文件会生成一些时钟和复位信号，因为我们测试的是纯组合逻辑，在生成的代码中我们把这些东西删掉。

```verilog
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
```

**✨STEP 5**

现在在bin2bcd.v上右键，设置bin2bcd为顶层，在test.v上右键设置test.v为活动测试代码。

![在这里插入图片描述](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/bbad464a207345c389ecc53cc338abcd.png)

**✨STEP 6**

点击编译，如果安装路径、编译器路径设置正常的话就应该能够输出结果。否则请检查一下安装路径或者是工程路径中是否有空格、中文字符。

![在这里插入图片描述](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/3b2fd43797b2441eb8754d9bd234ce74.png)


  点击仿真按钮可以看到弹出GTKWave，说明仿真可以工作。

  ![在这里插入图片描述](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/bde8b7d1ff984b1abf8538df04d7a570.png)

  ## 版本历史

  #### 2022a

  1. 修复了点击compile没有反应；
  2. 修复了点击simulate闪退，调不出GTKWave；
  3. 修复了生成的vcd文件不知所踪，仿真波形没法刷新；
  4. 2021a版本没有办法添加已经写好的源码，同时也没有办法重命名已有源码，今年重写了一些相关的代码，实现这两个功能；
  5. 调整了一个工程里只能添加一个testbench的逻辑，但是新版本在编译过程中同样还是遵循一个testbench的原则，用“Active Testbench”标记当前调用的testbench，实现灵活切换；
  6. 添加了对项目说明文档的支持（虽然这个功能真的很鸡肋哈哈）；
  7. 允许用户自定义source code、testbench、document的模板，可以在源文件创建的时候自动加上用户名、日期、代码文件名之类的；
  8. 新建源码的允许添加port，这个功能是模仿vivado的；
  9. 尝试在程序中添加检查更新、升级软件的功能；
  10. 尝试在程序中支持一些插件，让小助手的功能更加丰富。

  #### 2020c

  - 部分朋友点击Run Simulation无法正常弹出GTKWave，需要进入工程目录，手动运行simulate.cmd。
  - 拷贝至工程目录下的文件无法正常添加到工程中，需要手动修改Project.ivprj文件。

  ## 其他一些说明

  - 2021/3/30 仓库里的压缩文件iVerilogAssistant.7z总是包含最新的iVerilogAssistant，目前版本的iVerilogAssistant中包含iverilog v11-20201123-x64版本程序。
  - 2021/3/30 程序源码尚待补充。

## One More Thing

哈哈哈个人公众号有兴趣可以看看，不恰饭。

![gzh](https://gitee.com/Sunzhenyu59/drawing-bed/raw/master/drawing-bed/tree/gzh.png)