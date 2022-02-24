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
    public partial class create_open : Form
    {
        public string project_dir; // 工程路径
        public string project_name; // 工程名
        public bool is_open = true; // 打开一个工程还是关闭工程
        public bool is_effective = false;
        public bool is_create_dscp = false; // 是否建立一个描述文档
        public create_open()
        {
            InitializeComponent();
        }

        private void open_prj_CheckedChanged(object sender, EventArgs e)
        {
            if (open_prj.Checked)
            {
                open_tb.Enabled = true;
                open_btn.Enabled = true;
            }
            else
            {
                open_tb.Enabled = false;
                open_btn.Enabled = false;
            }
        }

        private void new_prj_CheckedChanged(object sender, EventArgs e)
        {
            if (new_prj.Checked)
            {
                new_tb.Enabled = true;
                new_btn.Enabled = true;
                name_tb.Enabled = true;
                create_dscp.Enabled = true;
            }
            else
            {
                new_tb.Enabled = false;
                new_btn.Enabled = false;
                name_tb.Enabled = false;
                create_dscp.Enabled = false;
            }
        }

        private void create_open_Load(object sender, EventArgs e)
        {
            new_tb.Enabled = false;
            new_btn.Enabled = false;
            name_tb.Enabled = false;
            create_dscp.Enabled = false;
        }

        private void open_btn_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "iVerilog Assistant Project|*.ivprj";
            openFileDialog.RestoreDirectory = true;
            openFileDialog.FilterIndex = 1;
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                open_tb.Text = openFileDialog.FileName;
            }
        }

        private void new_btn_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog();
            if (folderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                new_tb.Text = folderBrowserDialog.SelectedPath;
            }


        }

        private void ok_btn_Click(object sender, EventArgs e)
        {
            // 检查是不是符合要求
            if ((new_prj.Checked && (name_tb.Text == ""
                || new_tb.Text == ""
                || !(Regex.IsMatch(name_tb.Text, @"(?!((^(con)$)|^(con)\\..*|(^(prn)$)|^(prn)\\..*|(^(aux)$)|^(aux)\\..*|(^(nul)$)|^(nul)\\..*|(^(com)[1-9]$)|^(com)[1-9]\\..*|(^(lpt)[1-9]$)|^(lpt)[1-9]\\..*)|^\\s+|.*\\s$)(^[^\\\\\\/\\:\\<\\>\\*\\?\\\\\\""\\\\|]{1,255}$)"))))
                || (open_tb.Text == "" && open_prj.Checked)
                )
            {
                MessageBox.Show(this, "Invalid project name or path!", "Information cue", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            // 名字是符合要求的
            if (new_prj.Checked)
            {
                is_open = false;
                project_name = name_tb.Text;
                project_dir = new_tb.Text;
                if (create_dscp.Checked)
                    is_create_dscp = true;
                if (!Directory.Exists(project_dir))
                {
                    MessageBox.Show(this, project_dir + " not found!", "Information cue", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return;
                }
                if (File.Exists(project_dir +"\\"+ project_name+".ivprj") == true)
                {
                    MessageBox.Show(this,"Project " + project_name + " Exist!", "Information cue", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return;
                }
                // 工程目录下面的文件夹
                if (!Directory.Exists(project_dir + "\\src"))
                {
                    Directory.CreateDirectory(project_dir + "\\src");
                }
                if (!Directory.Exists(project_dir + "\\tb"))
                {
                    Directory.CreateDirectory(project_dir + "\\tb");
                }
                if (!Directory.Exists(project_dir + "\\doc"))
                {
                    Directory.CreateDirectory(project_dir + "\\doc");
                }
                FileStream fs = new FileStream(project_dir + "\\doc\\default_doc.txt", FileMode.Create);
                fs.Close();
                // 写入工程文件
                fs = new FileStream(project_dir+"\\"+ project_name + ".ivprj", FileMode.Create); //这个工程名没有扩展名
                StreamWriter sw = new StreamWriter(fs);
                sw.Write(project_name+"\r\nsrc\r\n0\r\ntb\r\n0\r\ndoc\r\n0\r\ntop\r\n!\r\nactive_tb\r\n!");
                sw.Close();
                fs.Close();
                
            }
            else
            {
                is_open = true;
                project_name = System.IO.Path.GetFileNameWithoutExtension(open_tb.Text); //去掉扩展名, 纯工程名
                project_dir = System.IO.Path.GetDirectoryName(open_tb.Text);
            }
            is_effective = true;
            this.Close();
        }

        private void cancel_btn_Click(object sender, EventArgs e)
        {
            is_effective = false;
            this.Close();
        }
    }
}
