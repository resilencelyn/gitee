namespace iVerilog_Assistant_2022a
{
    partial class MainForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.tree = new System.Windows.Forms.TreeView();
            this.menu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addExistFilesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.addToProjectToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.addToSourceToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.addToTestbenchToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.addToDocumentToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.renameToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.setToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.setAsActiveTestbenchToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.deleteToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.status = new System.Windows.Forms.StatusStrip();
            this.version_lab = new System.Windows.Forms.ToolStripStatusLabel();
            this.lb_name = new System.Windows.Forms.ToolStripStatusLabel();
            this.is_top_most = new System.Windows.Forms.CheckBox();
            this.Open_Project_Btn = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.Add_Src_Btn = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.Compile_Btn = new System.Windows.Forms.Button();
            this.Simulate_Btn = new System.Windows.Forms.Button();
            this.Config_Btn = new System.Windows.Forms.Button();
            this.label5 = new System.Windows.Forms.Label();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.label2 = new System.Windows.Forms.Label();
            this.tools_btn = new System.Windows.Forms.Button();
            this.file_info = new System.Windows.Forms.ToolStripStatusLabel();
            this.compiler = new System.Windows.Forms.ToolStripStatusLabel();
            this.menu.SuspendLayout();
            this.status.SuspendLayout();
            this.SuspendLayout();
            // 
            // tree
            // 
            this.tree.ContextMenuStrip = this.menu;
            this.tree.Enabled = false;
            this.tree.Location = new System.Drawing.Point(12, 12);
            this.tree.Name = "tree";
            this.tree.Size = new System.Drawing.Size(352, 524);
            this.tree.TabIndex = 0;
            this.tree.AfterLabelEdit += new System.Windows.Forms.NodeLabelEditEventHandler(this.tree_AfterLabelEdit);
            this.tree.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView1_AfterSelect);
            this.tree.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.tree_MouseDoubleClick);
            // 
            // menu
            // 
            this.menu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addExistFilesToolStripMenuItem,
            this.addToProjectToolStripMenuItem,
            this.renameToolStripMenuItem,
            this.setToolStripMenuItem,
            this.setAsActiveTestbenchToolStripMenuItem,
            this.deleteToolStripMenuItem});
            this.menu.Name = "menu";
            this.menu.Size = new System.Drawing.Size(199, 136);
            this.menu.Opening += new System.ComponentModel.CancelEventHandler(this.menu_Opening);
            // 
            // addExistFilesToolStripMenuItem
            // 
            this.addExistFilesToolStripMenuItem.Name = "addExistFilesToolStripMenuItem";
            this.addExistFilesToolStripMenuItem.Size = new System.Drawing.Size(198, 22);
            this.addExistFilesToolStripMenuItem.Text = "Add Exist Files";
            this.addExistFilesToolStripMenuItem.Click += new System.EventHandler(this.addExistFilesToolStripMenuItem_Click);
            // 
            // addToProjectToolStripMenuItem
            // 
            this.addToProjectToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addToSourceToolStripMenuItem,
            this.addToTestbenchToolStripMenuItem,
            this.addToDocumentToolStripMenuItem});
            this.addToProjectToolStripMenuItem.Name = "addToProjectToolStripMenuItem";
            this.addToProjectToolStripMenuItem.Size = new System.Drawing.Size(198, 22);
            this.addToProjectToolStripMenuItem.Text = "Add To Project";
            this.addToProjectToolStripMenuItem.Visible = false;
            // 
            // addToSourceToolStripMenuItem
            // 
            this.addToSourceToolStripMenuItem.Name = "addToSourceToolStripMenuItem";
            this.addToSourceToolStripMenuItem.Size = new System.Drawing.Size(170, 22);
            this.addToSourceToolStripMenuItem.Text = "Add To Source";
            // 
            // addToTestbenchToolStripMenuItem
            // 
            this.addToTestbenchToolStripMenuItem.Name = "addToTestbenchToolStripMenuItem";
            this.addToTestbenchToolStripMenuItem.Size = new System.Drawing.Size(170, 22);
            this.addToTestbenchToolStripMenuItem.Text = "Add To Testbench";
            // 
            // addToDocumentToolStripMenuItem
            // 
            this.addToDocumentToolStripMenuItem.Name = "addToDocumentToolStripMenuItem";
            this.addToDocumentToolStripMenuItem.Size = new System.Drawing.Size(170, 22);
            this.addToDocumentToolStripMenuItem.Text = "Add To Document";
            // 
            // renameToolStripMenuItem
            // 
            this.renameToolStripMenuItem.Name = "renameToolStripMenuItem";
            this.renameToolStripMenuItem.Size = new System.Drawing.Size(198, 22);
            this.renameToolStripMenuItem.Text = "Rename";
            this.renameToolStripMenuItem.Click += new System.EventHandler(this.renameToolStripMenuItem_Click);
            // 
            // setToolStripMenuItem
            // 
            this.setToolStripMenuItem.Name = "setToolStripMenuItem";
            this.setToolStripMenuItem.Size = new System.Drawing.Size(198, 22);
            this.setToolStripMenuItem.Text = "Set As Top Level";
            this.setToolStripMenuItem.Click += new System.EventHandler(this.setToolStripMenuItem_Click);
            // 
            // setAsActiveTestbenchToolStripMenuItem
            // 
            this.setAsActiveTestbenchToolStripMenuItem.Name = "setAsActiveTestbenchToolStripMenuItem";
            this.setAsActiveTestbenchToolStripMenuItem.Size = new System.Drawing.Size(198, 22);
            this.setAsActiveTestbenchToolStripMenuItem.Text = "Set As Active Testbench";
            this.setAsActiveTestbenchToolStripMenuItem.Click += new System.EventHandler(this.setAsActiveTestbenchToolStripMenuItem_Click);
            // 
            // deleteToolStripMenuItem
            // 
            this.deleteToolStripMenuItem.Name = "deleteToolStripMenuItem";
            this.deleteToolStripMenuItem.Size = new System.Drawing.Size(198, 22);
            this.deleteToolStripMenuItem.Text = "Remove";
            this.deleteToolStripMenuItem.Click += new System.EventHandler(this.deleteToolStripMenuItem_Click);
            // 
            // status
            // 
            this.status.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.version_lab,
            this.compiler,
            this.lb_name,
            this.file_info});
            this.status.Location = new System.Drawing.Point(0, 539);
            this.status.Name = "status";
            this.status.Size = new System.Drawing.Size(495, 22);
            this.status.TabIndex = 1;
            this.status.Text = "statusStrip1";
            // 
            // version_lab
            // 
            this.version_lab.Name = "version_lab";
            this.version_lab.Size = new System.Drawing.Size(129, 17);
            this.version_lab.Text = "iVerilog Assistant 2022a";
            // 
            // lb_name
            // 
            this.lb_name.ForeColor = System.Drawing.Color.Blue;
            this.lb_name.Name = "lb_name";
            this.lb_name.Size = new System.Drawing.Size(0, 17);
            // 
            // is_top_most
            // 
            this.is_top_most.AutoSize = true;
            this.is_top_most.Location = new System.Drawing.Point(383, 519);
            this.is_top_most.Name = "is_top_most";
            this.is_top_most.Size = new System.Drawing.Size(92, 17);
            this.is_top_most.TabIndex = 2;
            this.is_top_most.Text = "Always on top";
            this.is_top_most.UseVisualStyleBackColor = true;
            // 
            // Open_Project_Btn
            // 
            this.Open_Project_Btn.Location = new System.Drawing.Point(380, 12);
            this.Open_Project_Btn.Name = "Open_Project_Btn";
            this.Open_Project_Btn.Size = new System.Drawing.Size(103, 30);
            this.Open_Project_Btn.TabIndex = 3;
            this.Open_Project_Btn.Text = "Open/New";
            this.Open_Project_Btn.UseVisualStyleBackColor = true;
            this.Open_Project_Btn.Click += new System.EventHandler(this.Open_Project_Btn_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(380, 60);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(50, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "Add Files";
            // 
            // Add_Src_Btn
            // 
            this.Add_Src_Btn.Enabled = false;
            this.Add_Src_Btn.Location = new System.Drawing.Point(430, 80);
            this.Add_Src_Btn.Name = "Add_Src_Btn";
            this.Add_Src_Btn.Size = new System.Drawing.Size(50, 23);
            this.Add_Src_Btn.TabIndex = 6;
            this.Add_Src_Btn.Text = ">>";
            this.Add_Src_Btn.UseVisualStyleBackColor = true;
            this.Add_Src_Btn.Click += new System.EventHandler(this.Add_Src_Btn_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(380, 106);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(44, 13);
            this.label3.TabIndex = 8;
            this.label3.Text = "Compile";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(380, 152);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(47, 13);
            this.label4.TabIndex = 9;
            this.label4.Text = "Simulate";
            // 
            // Compile_Btn
            // 
            this.Compile_Btn.Enabled = false;
            this.Compile_Btn.Location = new System.Drawing.Point(430, 126);
            this.Compile_Btn.Name = "Compile_Btn";
            this.Compile_Btn.Size = new System.Drawing.Size(50, 23);
            this.Compile_Btn.TabIndex = 10;
            this.Compile_Btn.Text = ">>";
            this.Compile_Btn.UseVisualStyleBackColor = true;
            this.Compile_Btn.Click += new System.EventHandler(this.Compile_Btn_Click);
            // 
            // Simulate_Btn
            // 
            this.Simulate_Btn.Enabled = false;
            this.Simulate_Btn.Location = new System.Drawing.Point(430, 172);
            this.Simulate_Btn.Name = "Simulate_Btn";
            this.Simulate_Btn.Size = new System.Drawing.Size(50, 23);
            this.Simulate_Btn.TabIndex = 11;
            this.Simulate_Btn.Text = ">>";
            this.Simulate_Btn.UseVisualStyleBackColor = true;
            this.Simulate_Btn.Click += new System.EventHandler(this.Simulate_Btn_Click);
            // 
            // Config_Btn
            // 
            this.Config_Btn.Location = new System.Drawing.Point(430, 450);
            this.Config_Btn.Name = "Config_Btn";
            this.Config_Btn.Size = new System.Drawing.Size(49, 23);
            this.Config_Btn.TabIndex = 12;
            this.Config_Btn.Text = ">>";
            this.Config_Btn.UseVisualStyleBackColor = true;
            this.Config_Btn.Click += new System.EventHandler(this.Config_Btn_Click);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(380, 430);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(52, 13);
            this.label5.TabIndex = 13;
            this.label5.Text = "Configure";
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Interval = 1000;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(380, 384);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(33, 13);
            this.label2.TabIndex = 15;
            this.label2.Text = "Tools";
            this.label2.Visible = false;
            // 
            // tools_btn
            // 
            this.tools_btn.Location = new System.Drawing.Point(430, 404);
            this.tools_btn.Name = "tools_btn";
            this.tools_btn.Size = new System.Drawing.Size(49, 23);
            this.tools_btn.TabIndex = 14;
            this.tools_btn.Text = ">>";
            this.tools_btn.UseVisualStyleBackColor = true;
            this.tools_btn.Visible = false;
            this.tools_btn.Click += new System.EventHandler(this.tools_btn_Click);
            // 
            // file_info
            // 
            this.file_info.ActiveLinkColor = System.Drawing.Color.BlueViolet;
            this.file_info.ForeColor = System.Drawing.Color.Blue;
            this.file_info.Name = "file_info";
            this.file_info.Size = new System.Drawing.Size(0, 17);
            // 
            // compiler
            // 
            this.compiler.Name = "compiler";
            this.compiler.Size = new System.Drawing.Size(0, 17);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(495, 561);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.tools_btn);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.Config_Btn);
            this.Controls.Add(this.Simulate_Btn);
            this.Controls.Add(this.Compile_Btn);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.Add_Src_Btn);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.Open_Project_Btn);
            this.Controls.Add(this.is_top_most);
            this.Controls.Add(this.status);
            this.Controls.Add(this.tree);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "MainForm";
            this.Text = "iVerilog Assistant 2022a";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.menu.ResumeLayout(false);
            this.status.ResumeLayout(false);
            this.status.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TreeView tree;
        private System.Windows.Forms.StatusStrip status;
        private System.Windows.Forms.ToolStripStatusLabel version_lab;
        private System.Windows.Forms.CheckBox is_top_most;
        private System.Windows.Forms.Button Open_Project_Btn;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button Add_Src_Btn;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button Compile_Btn;
        private System.Windows.Forms.Button Simulate_Btn;
        private System.Windows.Forms.Button Config_Btn;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button tools_btn;
        private System.Windows.Forms.ToolStripStatusLabel lb_name;
        private System.Windows.Forms.ContextMenuStrip menu;
        private System.Windows.Forms.ToolStripMenuItem addExistFilesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem addToProjectToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem addToSourceToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem addToTestbenchToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem addToDocumentToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem setToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem setAsActiveTestbenchToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem deleteToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem renameToolStripMenuItem;
        private System.Windows.Forms.ToolStripStatusLabel file_info;
        private System.Windows.Forms.ToolStripStatusLabel compiler;
    }
}

