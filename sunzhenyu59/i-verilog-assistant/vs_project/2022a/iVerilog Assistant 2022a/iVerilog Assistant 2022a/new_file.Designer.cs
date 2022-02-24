namespace iVerilog_Assistant_2022a
{
    partial class new_file
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(new_file));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label10 = new System.Windows.Forms.Label();
            this.doc_file_type = new System.Windows.Forms.ComboBox();
            this.add_doc_temp_btn = new System.Windows.Forms.Button();
            this.label9 = new System.Windows.Forms.Label();
            this.doc_temp = new System.Windows.Forms.ComboBox();
            this.add_doc = new System.Windows.Forms.RadioButton();
            this.more_tools_btn = new System.Windows.Forms.Button();
            this.is_load_vcd = new System.Windows.Forms.CheckBox();
            this.is_nrst = new System.Windows.Forms.CheckBox();
            this.stop_cyc = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.rst_cyc = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.clk_freq = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.tb_module_name = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.src_module_name_tb = new System.Windows.Forms.TextBox();
            this.add_tb_temp_btn = new System.Windows.Forms.Button();
            this.add_src_temp_btn = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.file_type = new System.Windows.Forms.Label();
            this.src_port = new System.Windows.Forms.DataGridView();
            this.port_name = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dir = new System.Windows.Forms.DataGridViewComboBoxColumn();
            this.is_bus = new System.Windows.Forms.DataGridViewCheckBoxColumn();
            this.MSB = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.lsb = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.add_src_menu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deleteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tb_temp = new System.Windows.Forms.ComboBox();
            this.src_temp = new System.Windows.Forms.ComboBox();
            this.add_tb = new System.Windows.Forms.RadioButton();
            this.add_src = new System.Windows.Forms.RadioButton();
            this.label1 = new System.Windows.Forms.Label();
            this.file_name_tb = new System.Windows.Forms.TextBox();
            this.view_box = new System.Windows.Forms.RichTextBox();
            this.ok_btn = new System.Windows.Forms.Button();
            this.cancel_btn = new System.Windows.Forms.Button();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.src_port)).BeginInit();
            this.add_src_menu.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label10);
            this.groupBox1.Controls.Add(this.doc_file_type);
            this.groupBox1.Controls.Add(this.add_doc_temp_btn);
            this.groupBox1.Controls.Add(this.label9);
            this.groupBox1.Controls.Add(this.doc_temp);
            this.groupBox1.Controls.Add(this.add_doc);
            this.groupBox1.Controls.Add(this.more_tools_btn);
            this.groupBox1.Controls.Add(this.is_load_vcd);
            this.groupBox1.Controls.Add(this.is_nrst);
            this.groupBox1.Controls.Add(this.stop_cyc);
            this.groupBox1.Controls.Add(this.label8);
            this.groupBox1.Controls.Add(this.rst_cyc);
            this.groupBox1.Controls.Add(this.label7);
            this.groupBox1.Controls.Add(this.clk_freq);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Controls.Add(this.label5);
            this.groupBox1.Controls.Add(this.tb_module_name);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.src_module_name_tb);
            this.groupBox1.Controls.Add(this.add_tb_temp_btn);
            this.groupBox1.Controls.Add(this.add_src_temp_btn);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.file_type);
            this.groupBox1.Controls.Add(this.src_port);
            this.groupBox1.Controls.Add(this.tb_temp);
            this.groupBox1.Controls.Add(this.src_temp);
            this.groupBox1.Controls.Add(this.add_tb);
            this.groupBox1.Controls.Add(this.add_src);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.file_name_tb);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(576, 498);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "New Files";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(34, 469);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(50, 13);
            this.label10.TabIndex = 30;
            this.label10.Text = "File Type";
            // 
            // doc_file_type
            // 
            this.doc_file_type.Enabled = false;
            this.doc_file_type.FormattingEnabled = true;
            this.doc_file_type.Items.AddRange(new object[] {
            "Text File (.txt)",
            "Markdown File (.md)",
            "LaTex File (.tex)"});
            this.doc_file_type.Location = new System.Drawing.Point(91, 466);
            this.doc_file_type.Name = "doc_file_type";
            this.doc_file_type.Size = new System.Drawing.Size(156, 21);
            this.doc_file_type.TabIndex = 29;
            this.doc_file_type.SelectedIndexChanged += new System.EventHandler(this.doc_file_type_SelectedIndexChanged);
            // 
            // add_doc_temp_btn
            // 
            this.add_doc_temp_btn.Enabled = false;
            this.add_doc_temp_btn.Location = new System.Drawing.Point(499, 464);
            this.add_doc_temp_btn.Name = "add_doc_temp_btn";
            this.add_doc_temp_btn.Size = new System.Drawing.Size(51, 23);
            this.add_doc_temp_btn.TabIndex = 28;
            this.add_doc_temp_btn.Text = "Edit";
            this.add_doc_temp_btn.UseVisualStyleBackColor = true;
            this.add_doc_temp_btn.Click += new System.EventHandler(this.add_doc_temp_btn_Click);
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(280, 469);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(51, 13);
            this.label9.TabIndex = 27;
            this.label9.Text = "Template";
            // 
            // doc_temp
            // 
            this.doc_temp.Enabled = false;
            this.doc_temp.FormattingEnabled = true;
            this.doc_temp.Location = new System.Drawing.Point(337, 466);
            this.doc_temp.Name = "doc_temp";
            this.doc_temp.Size = new System.Drawing.Size(156, 21);
            this.doc_temp.TabIndex = 26;
            this.doc_temp.SelectedIndexChanged += new System.EventHandler(this.doc_temp_SelectedIndexChanged);
            // 
            // add_doc
            // 
            this.add_doc.AutoSize = true;
            this.add_doc.Location = new System.Drawing.Point(28, 440);
            this.add_doc.Name = "add_doc";
            this.add_doc.Size = new System.Drawing.Size(96, 17);
            this.add_doc.TabIndex = 25;
            this.add_doc.TabStop = true;
            this.add_doc.Text = "Add Document";
            this.add_doc.UseVisualStyleBackColor = true;
            this.add_doc.CheckedChanged += new System.EventHandler(this.add_doc_CheckedChanged);
            // 
            // more_tools_btn
            // 
            this.more_tools_btn.Enabled = false;
            this.more_tools_btn.Location = new System.Drawing.Point(464, 403);
            this.more_tools_btn.Name = "more_tools_btn";
            this.more_tools_btn.Size = new System.Drawing.Size(86, 23);
            this.more_tools_btn.TabIndex = 24;
            this.more_tools_btn.Text = "More Tools";
            this.more_tools_btn.UseVisualStyleBackColor = true;
            // 
            // is_load_vcd
            // 
            this.is_load_vcd.AutoSize = true;
            this.is_load_vcd.Checked = true;
            this.is_load_vcd.CheckState = System.Windows.Forms.CheckState.Checked;
            this.is_load_vcd.Enabled = false;
            this.is_load_vcd.Location = new System.Drawing.Point(305, 383);
            this.is_load_vcd.Name = "is_load_vcd";
            this.is_load_vcd.Size = new System.Drawing.Size(137, 17);
            this.is_load_vcd.TabIndex = 23;
            this.is_load_vcd.Text = "Load VCD File (Default)";
            this.is_load_vcd.UseVisualStyleBackColor = true;
            // 
            // is_nrst
            // 
            this.is_nrst.AutoSize = true;
            this.is_nrst.Checked = true;
            this.is_nrst.CheckState = System.Windows.Forms.CheckState.Checked;
            this.is_nrst.Enabled = false;
            this.is_nrst.Location = new System.Drawing.Point(305, 361);
            this.is_nrst.Name = "is_nrst";
            this.is_nrst.Size = new System.Drawing.Size(153, 17);
            this.is_nrst.TabIndex = 22;
            this.is_nrst.Text = "Reset Active Low (Default)";
            this.is_nrst.UseVisualStyleBackColor = true;
            // 
            // stop_cyc
            // 
            this.stop_cyc.Enabled = false;
            this.stop_cyc.Location = new System.Drawing.Point(129, 410);
            this.stop_cyc.Name = "stop_cyc";
            this.stop_cyc.Size = new System.Drawing.Size(72, 20);
            this.stop_cyc.TabIndex = 21;
            this.stop_cyc.Text = "100";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(25, 413);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(88, 13);
            this.label8.TabIndex = 20;
            this.label8.Text = "Stop After Cycles";
            // 
            // rst_cyc
            // 
            this.rst_cyc.Enabled = false;
            this.rst_cyc.Location = new System.Drawing.Point(129, 384);
            this.rst_cyc.Name = "rst_cyc";
            this.rst_cyc.Size = new System.Drawing.Size(72, 20);
            this.rst_cyc.TabIndex = 19;
            this.rst_cyc.Text = "2";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(25, 387);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(69, 13);
            this.label7.TabIndex = 18;
            this.label7.Text = "Reset Cycles";
            // 
            // clk_freq
            // 
            this.clk_freq.Enabled = false;
            this.clk_freq.Location = new System.Drawing.Point(129, 358);
            this.clk_freq.Name = "clk_freq";
            this.clk_freq.Size = new System.Drawing.Size(72, 20);
            this.clk_freq.TabIndex = 17;
            this.clk_freq.Text = "50";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(25, 361);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(89, 13);
            this.label6.TabIndex = 16;
            this.label6.Text = "Clock Freq (MHz)";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(25, 324);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(73, 13);
            this.label5.TabIndex = 15;
            this.label5.Text = "Module Name";
            // 
            // tb_module_name
            // 
            this.tb_module_name.Enabled = false;
            this.tb_module_name.Location = new System.Drawing.Point(104, 321);
            this.tb_module_name.Name = "tb_module_name";
            this.tb_module_name.ReadOnly = true;
            this.tb_module_name.Size = new System.Drawing.Size(143, 20);
            this.tb_module_name.TabIndex = 14;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(25, 115);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(73, 13);
            this.label4.TabIndex = 13;
            this.label4.Text = "Module Name";
            // 
            // src_module_name_tb
            // 
            this.src_module_name_tb.Location = new System.Drawing.Point(104, 112);
            this.src_module_name_tb.Name = "src_module_name_tb";
            this.src_module_name_tb.Size = new System.Drawing.Size(143, 20);
            this.src_module_name_tb.TabIndex = 12;
            // 
            // add_tb_temp_btn
            // 
            this.add_tb_temp_btn.Enabled = false;
            this.add_tb_temp_btn.Location = new System.Drawing.Point(499, 318);
            this.add_tb_temp_btn.Name = "add_tb_temp_btn";
            this.add_tb_temp_btn.Size = new System.Drawing.Size(51, 23);
            this.add_tb_temp_btn.TabIndex = 11;
            this.add_tb_temp_btn.Text = "Edit";
            this.add_tb_temp_btn.UseVisualStyleBackColor = true;
            this.add_tb_temp_btn.Click += new System.EventHandler(this.add_tb_temp_btn_Click);
            // 
            // add_src_temp_btn
            // 
            this.add_src_temp_btn.Location = new System.Drawing.Point(499, 109);
            this.add_src_temp_btn.Name = "add_src_temp_btn";
            this.add_src_temp_btn.Size = new System.Drawing.Size(51, 23);
            this.add_src_temp_btn.TabIndex = 10;
            this.add_src_temp_btn.Text = "Edit";
            this.add_src_temp_btn.UseVisualStyleBackColor = true;
            this.add_src_temp_btn.Click += new System.EventHandler(this.add_src_temp_btn_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(280, 323);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(51, 13);
            this.label3.TabIndex = 9;
            this.label3.Text = "Template";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(280, 114);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(51, 13);
            this.label2.TabIndex = 8;
            this.label2.Text = "Template";
            // 
            // file_type
            // 
            this.file_type.AutoSize = true;
            this.file_type.Location = new System.Drawing.Point(464, 51);
            this.file_type.Name = "file_type";
            this.file_type.Size = new System.Drawing.Size(95, 13);
            this.file_type.TabIndex = 7;
            this.file_type.Text = ".V (Verilog Source)";
            // 
            // src_port
            // 
            this.src_port.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.src_port.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.src_port.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.port_name,
            this.dir,
            this.is_bus,
            this.MSB,
            this.lsb});
            this.src_port.ContextMenuStrip = this.add_src_menu;
            this.src_port.Location = new System.Drawing.Point(28, 138);
            this.src_port.Name = "src_port";
            this.src_port.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.src_port.Size = new System.Drawing.Size(522, 143);
            this.src_port.TabIndex = 6;
            // 
            // port_name
            // 
            this.port_name.HeaderText = "Port Name";
            this.port_name.Name = "port_name";
            this.port_name.Width = 250;
            // 
            // dir
            // 
            this.dir.DisplayStyle = System.Windows.Forms.DataGridViewComboBoxDisplayStyle.ComboBox;
            this.dir.HeaderText = "Direction";
            this.dir.Items.AddRange(new object[] {
            "input",
            "output",
            "inout"});
            this.dir.Name = "dir";
            this.dir.Width = 80;
            // 
            // is_bus
            // 
            this.is_bus.FalseValue = "0";
            this.is_bus.HeaderText = "Bus";
            this.is_bus.IndeterminateValue = "0";
            this.is_bus.Name = "is_bus";
            this.is_bus.TrueValue = "1";
            this.is_bus.Width = 40;
            // 
            // MSB
            // 
            this.MSB.HeaderText = "MSB";
            this.MSB.Name = "MSB";
            this.MSB.Width = 40;
            // 
            // lsb
            // 
            this.lsb.HeaderText = "LSB";
            this.lsb.Name = "lsb";
            this.lsb.Width = 40;
            // 
            // add_src_menu
            // 
            this.add_src_menu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteToolStripMenuItem});
            this.add_src_menu.Name = "add_src_menu";
            this.add_src_menu.Size = new System.Drawing.Size(108, 26);
            // 
            // deleteToolStripMenuItem
            // 
            this.deleteToolStripMenuItem.Name = "deleteToolStripMenuItem";
            this.deleteToolStripMenuItem.Size = new System.Drawing.Size(107, 22);
            this.deleteToolStripMenuItem.Text = "Delete";
            this.deleteToolStripMenuItem.Click += new System.EventHandler(this.deleteToolStripMenuItem_Click);
            // 
            // tb_temp
            // 
            this.tb_temp.Enabled = false;
            this.tb_temp.FormattingEnabled = true;
            this.tb_temp.Location = new System.Drawing.Point(337, 320);
            this.tb_temp.Name = "tb_temp";
            this.tb_temp.Size = new System.Drawing.Size(156, 21);
            this.tb_temp.TabIndex = 5;
            // 
            // src_temp
            // 
            this.src_temp.FormattingEnabled = true;
            this.src_temp.Location = new System.Drawing.Point(337, 111);
            this.src_temp.Name = "src_temp";
            this.src_temp.Size = new System.Drawing.Size(156, 21);
            this.src_temp.TabIndex = 4;
            this.src_temp.SelectedIndexChanged += new System.EventHandler(this.src_temp_SelectedIndexChanged);
            // 
            // add_tb
            // 
            this.add_tb.AutoSize = true;
            this.add_tb.Location = new System.Drawing.Point(28, 296);
            this.add_tb.Name = "add_tb";
            this.add_tb.Size = new System.Drawing.Size(98, 17);
            this.add_tb.TabIndex = 3;
            this.add_tb.Text = "Add Testbench";
            this.add_tb.UseVisualStyleBackColor = true;
            this.add_tb.CheckedChanged += new System.EventHandler(this.add_tb_CheckedChanged);
            // 
            // add_src
            // 
            this.add_src.AutoSize = true;
            this.add_src.Checked = true;
            this.add_src.Location = new System.Drawing.Point(28, 87);
            this.add_src.Name = "add_src";
            this.add_src.Size = new System.Drawing.Size(81, 17);
            this.add_src.TabIndex = 2;
            this.add_src.TabStop = true;
            this.add_src.Text = "Add Source";
            this.add_src.UseVisualStyleBackColor = true;
            this.add_src.CheckedChanged += new System.EventHandler(this.add_src_CheckedChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(25, 32);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(54, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "File Name";
            // 
            // file_name_tb
            // 
            this.file_name_tb.Location = new System.Drawing.Point(28, 48);
            this.file_name_tb.Name = "file_name_tb";
            this.file_name_tb.Size = new System.Drawing.Size(430, 20);
            this.file_name_tb.TabIndex = 0;
            this.file_name_tb.TextChanged += new System.EventHandler(this.file_name_tb_TextChanged);
            // 
            // view_box
            // 
            this.view_box.Enabled = false;
            this.view_box.Font = new System.Drawing.Font("Consolas", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.view_box.Location = new System.Drawing.Point(594, 12);
            this.view_box.Name = "view_box";
            this.view_box.Size = new System.Drawing.Size(490, 498);
            this.view_box.TabIndex = 1;
            this.view_box.Text = "";
            // 
            // ok_btn
            // 
            this.ok_btn.Location = new System.Drawing.Point(853, 516);
            this.ok_btn.Name = "ok_btn";
            this.ok_btn.Size = new System.Drawing.Size(86, 33);
            this.ok_btn.TabIndex = 2;
            this.ok_btn.Text = "OK";
            this.ok_btn.UseVisualStyleBackColor = true;
            this.ok_btn.Click += new System.EventHandler(this.ok_btn_Click);
            // 
            // cancel_btn
            // 
            this.cancel_btn.Location = new System.Drawing.Point(992, 516);
            this.cancel_btn.Name = "cancel_btn";
            this.cancel_btn.Size = new System.Drawing.Size(86, 33);
            this.cancel_btn.TabIndex = 3;
            this.cancel_btn.Text = "Cancel";
            this.cancel_btn.UseVisualStyleBackColor = true;
            this.cancel_btn.Click += new System.EventHandler(this.cancel_btn_Click);
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Interval = 1000;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // new_file
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1096, 558);
            this.Controls.Add(this.cancel_btn);
            this.Controls.Add(this.ok_btn);
            this.Controls.Add(this.view_box);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "new_file";
            this.Text = "New Files";
            this.Load += new System.EventHandler(this.new_file_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.src_port)).EndInit();
            this.add_src_menu.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RichTextBox view_box;
        private System.Windows.Forms.Button ok_btn;
        private System.Windows.Forms.Button cancel_btn;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox file_name_tb;
        private System.Windows.Forms.RadioButton add_tb;
        private System.Windows.Forms.RadioButton add_src;
        private System.Windows.Forms.ComboBox tb_temp;
        private System.Windows.Forms.ComboBox src_temp;
        private System.Windows.Forms.DataGridView src_port;
        private System.Windows.Forms.Label file_type;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button add_src_temp_btn;
        private System.Windows.Forms.Button add_tb_temp_btn;
        private System.Windows.Forms.ContextMenuStrip add_src_menu;
        private System.Windows.Forms.ToolStripMenuItem deleteToolStripMenuItem;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox src_module_name_tb;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox tb_module_name;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox clk_freq;
        private System.Windows.Forms.TextBox rst_cyc;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox stop_cyc;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.CheckBox is_nrst;
        private System.Windows.Forms.CheckBox is_load_vcd;
        private System.Windows.Forms.Button more_tools_btn;
        private System.Windows.Forms.RadioButton add_doc;
        private System.Windows.Forms.Button add_doc_temp_btn;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.ComboBox doc_temp;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.ComboBox doc_file_type;
        private System.Windows.Forms.DataGridViewTextBoxColumn port_name;
        private System.Windows.Forms.DataGridViewComboBoxColumn dir;
        private System.Windows.Forms.DataGridViewCheckBoxColumn is_bus;
        private System.Windows.Forms.DataGridViewTextBoxColumn MSB;
        private System.Windows.Forms.DataGridViewTextBoxColumn lsb;
        private System.Windows.Forms.Timer timer1;
    }
}