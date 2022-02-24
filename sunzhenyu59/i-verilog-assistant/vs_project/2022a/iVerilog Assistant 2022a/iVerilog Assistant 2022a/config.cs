using iVerilog_Assistant_2022a.Properties;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace iVerilog_Assistant_2022a
{
    public partial class config : Form
    {
        string username, editor_path, iv_path, version;

        private void editor_brose_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "(exe)|*.exe";
            openFileDialog.RestoreDirectory = true;
            openFileDialog.FilterIndex = 1;
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                editor_path = openFileDialog.FileName;
                editor_tb.Text = editor_path;
            }
        }

        private void iv_brose_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog();
            if (folderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                iv_path = folderBrowserDialog.SelectedPath;
                iv_tb.Text = iv_path;
            }
        }

        private void close_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void save_Click(object sender, EventArgs e)
        {
            FileStream fs = new FileStream("config.ini", FileMode.Create);
            StreamWriter sw = new StreamWriter(fs);
            sw.WriteLine("username");
            sw.WriteLine(user_name_tb.Text);
            sw.WriteLine("editor_path");
            sw.WriteLine(editor_tb.Text);
            sw.WriteLine("iv_path");
            sw.WriteLine(iv_tb.Text);
            sw.WriteLine("version");
            sw.WriteLine(version);
            sw.Close();
            fs.Close();
            save.Enabled = false;
        }

        private void update_Click(object sender, EventArgs e)
        {
            Process.Start("update.exe");
            Process.GetCurrentProcess().Kill();
        }

        public config()
        {
            InitializeComponent();
        }

        private void config_Load(object sender, EventArgs e)
        {
            pictureBox1.Image = (Resources.logo);
            StreamReader sr = new StreamReader("config.ini", Encoding.Default);
            string line;
            while ((line = sr.ReadLine()) != null)
            {
                switch (line)
                {
                    case "username":
                        username = sr.ReadLine(); break;
                    case "editor_path": editor_path = sr.ReadLine(); break;
                    case "iv_path": iv_path = sr.ReadLine(); break;
                    case "version": version = sr.ReadLine(); break;
                    default: break;
                }
            }
            user_name_tb.Text = username;
            editor_tb.Text = editor_path;
            iv_tb.Text = iv_path;
            sr.Close();
        }
    }
}
