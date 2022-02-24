namespace iVerilog_Assistant_2022a
{
    partial class Rename
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
            this.ok = new System.Windows.Forms.Button();
            this.cancle = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.old_name = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.new_name = new System.Windows.Forms.TextBox();
            this.old_ext = new System.Windows.Forms.Label();
            this.new_ext = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // ok
            // 
            this.ok.Location = new System.Drawing.Point(185, 117);
            this.ok.Name = "ok";
            this.ok.Size = new System.Drawing.Size(75, 23);
            this.ok.TabIndex = 0;
            this.ok.Text = "OK";
            this.ok.UseVisualStyleBackColor = true;
            this.ok.Click += new System.EventHandler(this.ok_Click);
            // 
            // cancle
            // 
            this.cancle.Location = new System.Drawing.Point(321, 117);
            this.cancle.Name = "cancle";
            this.cancle.Size = new System.Drawing.Size(75, 23);
            this.cancle.TabIndex = 1;
            this.cancle.Text = "Cancle";
            this.cancle.UseVisualStyleBackColor = true;
            this.cancle.Click += new System.EventHandler(this.cancle_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(24, 21);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(54, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Old Name";
            // 
            // old_name
            // 
            this.old_name.Location = new System.Drawing.Point(27, 37);
            this.old_name.Name = "old_name";
            this.old_name.ReadOnly = true;
            this.old_name.Size = new System.Drawing.Size(323, 20);
            this.old_name.TabIndex = 3;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(24, 66);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(60, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "New Name";
            // 
            // new_name
            // 
            this.new_name.Location = new System.Drawing.Point(27, 82);
            this.new_name.Name = "new_name";
            this.new_name.Size = new System.Drawing.Size(323, 20);
            this.new_name.TabIndex = 5;
            // 
            // old_ext
            // 
            this.old_ext.AutoSize = true;
            this.old_ext.Location = new System.Drawing.Point(368, 40);
            this.old_ext.Name = "old_ext";
            this.old_ext.Size = new System.Drawing.Size(0, 13);
            this.old_ext.TabIndex = 6;
            // 
            // new_ext
            // 
            this.new_ext.AutoSize = true;
            this.new_ext.Location = new System.Drawing.Point(368, 85);
            this.new_ext.Name = "new_ext";
            this.new_ext.Size = new System.Drawing.Size(0, 13);
            this.new_ext.TabIndex = 7;
            // 
            // Rename
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(422, 154);
            this.Controls.Add(this.new_ext);
            this.Controls.Add(this.old_ext);
            this.Controls.Add(this.new_name);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.old_name);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.cancle);
            this.Controls.Add(this.ok);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "Rename";
            this.Text = "Rename";
            this.Load += new System.EventHandler(this.Rename_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button ok;
        private System.Windows.Forms.Button cancle;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox old_name;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox new_name;
        private System.Windows.Forms.Label old_ext;
        private System.Windows.Forms.Label new_ext;
    }
}