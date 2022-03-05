using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace iVerilog_Assistant_2022a
{
    public partial class Rename : Form
    {
        public string name;
        string before_edit;
        public bool is_effective = false;
        public Rename(string before_edit)
        {
            InitializeComponent();
            this.before_edit = before_edit;
        }

        private void ok_Click(object sender, EventArgs e)
        {
            if (Regex.IsMatch(new_name.Text, @"^[A-Za-z][A-Za-z0-9_]+$"))
            {
                is_effective = true;
                name = new_name.Text + new_ext.Text;
            }
            else
            {
                MessageBox.Show(this, "Invalid file name!", "Information cue", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            this.Close();
        }

        private void Rename_Load(object sender, EventArgs e)
        {
            if (before_edit.Contains("[Active Testbench]"))
            {
                old_ext.Text = ".v";
                new_ext.Text = ".v";
                old_name.Text = Path.GetFileNameWithoutExtension(before_edit.Substring(0, before_edit.Length - before_edit.LastIndexOf(".")));
            }
            else
            {
                old_ext.Text = Path.GetExtension(before_edit);
                new_ext.Text = Path.GetExtension(before_edit);
                old_name.Text = Path.GetFileNameWithoutExtension(before_edit);
            }
        }

        private void cancle_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
