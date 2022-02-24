namespace iVerilog_Assistant_2022a
{
    partial class create_open
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(create_open));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.create_dscp = new System.Windows.Forms.CheckBox();
            this.name_tb = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.open_btn = new System.Windows.Forms.Button();
            this.new_btn = new System.Windows.Forms.Button();
            this.new_tb = new System.Windows.Forms.TextBox();
            this.open_tb = new System.Windows.Forms.TextBox();
            this.new_prj = new System.Windows.Forms.RadioButton();
            this.open_prj = new System.Windows.Forms.RadioButton();
            this.ok_btn = new System.Windows.Forms.Button();
            this.cancel_btn = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.create_dscp);
            this.groupBox1.Controls.Add(this.name_tb);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.open_btn);
            this.groupBox1.Controls.Add(this.new_btn);
            this.groupBox1.Controls.Add(this.new_tb);
            this.groupBox1.Controls.Add(this.open_tb);
            this.groupBox1.Controls.Add(this.new_prj);
            this.groupBox1.Controls.Add(this.open_prj);
            this.groupBox1.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(643, 255);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Open/New";
            // 
            // create_dscp
            // 
            this.create_dscp.AutoSize = true;
            this.create_dscp.Location = new System.Drawing.Point(444, 223);
            this.create_dscp.Name = "create_dscp";
            this.create_dscp.Size = new System.Drawing.Size(107, 17);
            this.create_dscp.TabIndex = 10;
            this.create_dscp.Text = "Create Dscription";
            this.create_dscp.UseVisualStyleBackColor = true;
            this.create_dscp.Visible = false;
            // 
            // name_tb
            // 
            this.name_tb.Location = new System.Drawing.Point(49, 146);
            this.name_tb.Name = "name_tb";
            this.name_tb.Size = new System.Drawing.Size(502, 20);
            this.name_tb.TabIndex = 9;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(32, 126);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(71, 13);
            this.label2.TabIndex = 8;
            this.label2.Text = "Project Name";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(32, 177);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(85, 13);
            this.label1.TabIndex = 7;
            this.label1.Text = "Project Directory";
            // 
            // open_btn
            // 
            this.open_btn.Location = new System.Drawing.Point(565, 60);
            this.open_btn.Name = "open_btn";
            this.open_btn.Size = new System.Drawing.Size(57, 23);
            this.open_btn.TabIndex = 6;
            this.open_btn.Text = ">>";
            this.open_btn.UseVisualStyleBackColor = true;
            this.open_btn.Click += new System.EventHandler(this.open_btn_Click);
            // 
            // new_btn
            // 
            this.new_btn.Location = new System.Drawing.Point(565, 195);
            this.new_btn.Name = "new_btn";
            this.new_btn.Size = new System.Drawing.Size(57, 23);
            this.new_btn.TabIndex = 5;
            this.new_btn.Text = ">>";
            this.new_btn.UseVisualStyleBackColor = true;
            this.new_btn.Click += new System.EventHandler(this.new_btn_Click);
            // 
            // new_tb
            // 
            this.new_tb.Location = new System.Drawing.Point(49, 197);
            this.new_tb.Name = "new_tb";
            this.new_tb.ReadOnly = true;
            this.new_tb.Size = new System.Drawing.Size(502, 20);
            this.new_tb.TabIndex = 4;
            // 
            // open_tb
            // 
            this.open_tb.Location = new System.Drawing.Point(49, 60);
            this.open_tb.Name = "open_tb";
            this.open_tb.ReadOnly = true;
            this.open_tb.Size = new System.Drawing.Size(502, 20);
            this.open_tb.TabIndex = 2;
            // 
            // new_prj
            // 
            this.new_prj.AutoSize = true;
            this.new_prj.Location = new System.Drawing.Point(17, 98);
            this.new_prj.Name = "new_prj";
            this.new_prj.Size = new System.Drawing.Size(83, 17);
            this.new_prj.TabIndex = 1;
            this.new_prj.Text = "New Project";
            this.new_prj.UseVisualStyleBackColor = true;
            this.new_prj.CheckedChanged += new System.EventHandler(this.new_prj_CheckedChanged);
            // 
            // open_prj
            // 
            this.open_prj.AutoSize = true;
            this.open_prj.Checked = true;
            this.open_prj.Location = new System.Drawing.Point(17, 37);
            this.open_prj.Name = "open_prj";
            this.open_prj.Size = new System.Drawing.Size(87, 17);
            this.open_prj.TabIndex = 0;
            this.open_prj.TabStop = true;
            this.open_prj.Text = "Open Project";
            this.open_prj.UseVisualStyleBackColor = true;
            this.open_prj.CheckedChanged += new System.EventHandler(this.open_prj_CheckedChanged);
            // 
            // ok_btn
            // 
            this.ok_btn.Location = new System.Drawing.Point(381, 273);
            this.ok_btn.Name = "ok_btn";
            this.ok_btn.Size = new System.Drawing.Size(86, 33);
            this.ok_btn.TabIndex = 1;
            this.ok_btn.Text = "OK";
            this.ok_btn.UseVisualStyleBackColor = true;
            this.ok_btn.Click += new System.EventHandler(this.ok_btn_Click);
            // 
            // cancel_btn
            // 
            this.cancel_btn.Location = new System.Drawing.Point(550, 273);
            this.cancel_btn.Name = "cancel_btn";
            this.cancel_btn.Size = new System.Drawing.Size(86, 33);
            this.cancel_btn.TabIndex = 2;
            this.cancel_btn.Text = "Cancel";
            this.cancel_btn.UseVisualStyleBackColor = true;
            this.cancel_btn.Click += new System.EventHandler(this.cancel_btn_Click);
            // 
            // create_open
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(667, 328);
            this.Controls.Add(this.cancel_btn);
            this.Controls.Add(this.ok_btn);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "create_open";
            this.Text = "Open/New";
            this.Load += new System.EventHandler(this.create_open_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton open_prj;
        private System.Windows.Forms.RadioButton new_prj;
        private System.Windows.Forms.Button ok_btn;
        private System.Windows.Forms.Button cancel_btn;
        private System.Windows.Forms.TextBox open_tb;
        private System.Windows.Forms.Button new_btn;
        private System.Windows.Forms.TextBox new_tb;
        private System.Windows.Forms.Button open_btn;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox name_tb;
        private System.Windows.Forms.CheckBox create_dscp;
    }
}