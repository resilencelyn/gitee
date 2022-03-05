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
    public partial class edit_template : Form
    {
        int type;
        string temp_dir, edit_file_name;
        List<string> list_of_src_temp = new List<string>();
        List<string> list_of_tb_temp = new List<string>();
        List<string> list_of_latex_temp = new List<string>();
        List<string> list_of_md_temp = new List<string>();
        List<string> list_of_txt_temp = new List<string>();
        private void Highlight_Keywords()// 给列表的词加高亮
        {
            string[] keywords = { "module", "begin", "end", "endmodule", "wire", "reg", "input", "output", "inout", "always", "case", "endcase", "default", "for", "generate", "repeat", "if", "else", "else if", "initial", "task", "repeat", "forever", "$stop()", "$finish()", "`timescale", "parameter", "localparam" };
            string[] comments = { @"\/\*(\s|.)*?\*\/", @"//[\s\S]*?\n" };
            string[] masks = { "&file&", "&author&", "&date&", "&params&", "&ports&", "&module_name&", "&tb_cfg&" };
            foreach (string str in keywords)
            {
                Regex r = new Regex(str, System.Text.RegularExpressions.RegexOptions.IgnoreCase);

                foreach (Match m in r.Matches(view_box.Text))
                {
                    view_box.Select(m.Index, m.Length);
                    view_box.SelectionColor = Color.OrangeRed;
                }
            }
            foreach (string str in comments)
            {
                Regex r = new Regex(str, System.Text.RegularExpressions.RegexOptions.IgnoreCase);

                foreach (Match m in r.Matches(view_box.Text))
                {
                    view_box.Select(m.Index, m.Length);
                    view_box.SelectionColor = Color.ForestGreen;
                }
            }
            foreach (string str in masks)
            {
                Regex r = new Regex(str, System.Text.RegularExpressions.RegexOptions.IgnoreCase);

                foreach (Match m in r.Matches(view_box.Text))
                {
                    view_box.Select(m.Index, m.Length);
                    view_box.SelectionColor = Color.Purple;
                }
            }
        }
        void disable_edit()
        {
            view_box.Enabled = false;
            Highlight_Keywords();
            save.Enabled = false;
            edit.Enabled = true;
            list.Enabled = true;
        }
        void enable_edit()
        {
            view_box.SelectAll();
            view_box.SelectionColor = Color.Black;
            view_box.DeselectAll();
            view_box.Enabled = true;
            save.Enabled = true;
            edit.Enabled = false;
            list.Enabled = false;
        }
        void save_ini()
        {
            FileStream fs = new FileStream("templates\\templates.ini", FileMode.Create); //这个工程名没有扩展名
            StreamWriter sw = new StreamWriter(fs);
            sw.Write("src\r\n"+list_of_src_temp.Count.ToString()+"\r\n");
            foreach(string str in list_of_src_temp)
                sw.WriteLine(str);
            sw.Write("tb\r\n" + list_of_tb_temp.Count.ToString() + "\r\n");
            foreach (string str in list_of_tb_temp)
                sw.WriteLine(str);
            sw.Write("latex\r\n" + list_of_latex_temp.Count.ToString() + "\r\n");
            foreach (string str in list_of_latex_temp)
                sw.WriteLine(str);
            sw.Write("md\r\n" + list_of_md_temp.Count.ToString() + "\r\n");
            foreach (string str in list_of_md_temp)
                sw.WriteLine(str);
            sw.Write("txt\r\n" + list_of_txt_temp.Count.ToString() + "\r\n");
            foreach (string str in list_of_txt_temp)
                sw.WriteLine(str);
            sw.Close();
            fs.Close();
        }
        public edit_template(int file_type, string edit_file_name)
        {
            InitializeComponent();
            this.type = file_type;
            this.edit_file_name = edit_file_name;
        }
        void view_file(string dir, string file_name) //文件预览
        {

            view_box.Clear();
            if (File.Exists(dir + file_name) == false)
            {
                MessageBox.Show(this, "Could not find files", "Information cue", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            StreamReader sr = new StreamReader(dir + file_name, Encoding.Default);
            string line;
            while ((line = sr.ReadLine()) != null)
            {
                view_box.AppendText(line);
                view_box.AppendText(Environment.NewLine);
            }
            sr.Close();
        }
        private void list_SelectedIndexChanged(object sender, EventArgs e)
        {
            this.Text = "Edit : " + list.Text;
            edit_file_name = list.Text;
            view_file(temp_dir, edit_file_name);
            disable_edit();
        }

        private void edit_Click(object sender, EventArgs e)
        {
            enable_edit();

        }

        private void save_Click(object sender, EventArgs e)
        {
            view_box.SaveFile(temp_dir + edit_file_name, RichTextBoxStreamType.PlainText);
            disable_edit();

        }

        private void add_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            switch(type)
            {
                case 0: case 1: openFileDialog.Filter = "Verilog Source File|*.v";break;
                case 2: openFileDialog.Filter = "Text File|*.txt"; break;
                case 3: openFileDialog.Filter = "Markdown File|*.md"; break;
                case 4: openFileDialog.Filter = "LaTex File|*.tex"; break;
            }
            openFileDialog.RestoreDirectory = true;
            openFileDialog.Multiselect = true;
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                foreach (string file in openFileDialog.FileNames)
                {
                    bool flag = true;
                    string add_name = Path.GetFileName(file);
                    foreach(string str in list.Items)
                    {
                        if(str == add_name || File.Exists(temp_dir + Path.GetFileName(file)))
                        {
                            flag = false;
                        }
                    }
                    if(flag)
                    {
                        list.Items.Add(add_name);
                        System.IO.File.Copy(file, temp_dir + Path.GetFileName(file));
                        switch (type)
                        {
                            case 0: list_of_src_temp.Add(add_name); break;
                            case 1: list_of_tb_temp.Add(add_name); break;
                            case 2: list_of_txt_temp.Add(add_name); break;
                            case 3: list_of_md_temp.Add(add_name); break;
                            case 4: list_of_latex_temp.Add(add_name); break;
                        }
                    }
                }
                save_ini();
            }
            list.SelectedIndex = 0;
        }

        private void del_Click(object sender, EventArgs e)
        {
            if(list.Text == "Default Source Template.v" 
                || list.Text == "Default Testbench Template.v" 
                || list.Text == "Default Document Template.txt"
                || list.Text == "Default Document Template.md"
                || list.Text == "Default Document Template.tex")
            {
                MessageBox.Show(this, "Could not delete default template!", "Information cue", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            else
            {
                DialogResult msg = MessageBox.Show("Are you sure you want to delete this template? (including files on disk)?", "Confirm", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                if (msg == DialogResult.Yes)
                {
                    switch (type)
                    {
                        case 0: list_of_src_temp.Remove(list.Text); break;
                        case 1: list_of_tb_temp.Remove(list.Text); break;
                        case 2: list_of_txt_temp.Remove(list.Text); break;
                        case 3: list_of_md_temp.Remove(list.Text); break;
                        case 4: list_of_latex_temp.Remove(list.Text); break;
                    }
                    File.Delete(temp_dir + list.Text);
                    list.Items.Remove(list.SelectedItem);
                    save_ini();
                }
                else
                {
                    return;
                }
            }
            list.SelectedIndex = 0;
        }

        private void add_template_Load(object sender, EventArgs e)
        {
            int index = 0;
            if (File.Exists("templates\\templates.ini") == false)
            {
                MessageBox.Show(this, "Could not find templates.ini!", "Information cue", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            StreamReader sr = new StreamReader("templates\\templates.ini", Encoding.Default);
            string line;
            int file_type;
            int file_counts = 0;
            while ((line = sr.ReadLine()) != null)
            {
                switch (line)
                {
                    case "src": file_type = 0; break;
                    case "tb": file_type = 1; break;
                    case "txt": file_type = 2; break;
                    case "md": file_type = 3; break;
                    case "latex": file_type = 4; break;
                    default: file_type = -1; break;
                }
                if (file_type != -1)
                {
                    line = sr.ReadLine();
                    file_counts = int.Parse(line);
                    for (int i = 0; i < file_counts; i++)
                    {
                        line = sr.ReadLine();
                        switch (file_type)
                        {
                            case 0: list_of_src_temp.Add(line);break;
                            case 1: list_of_tb_temp.Add(line); break;
                            case 2: list_of_txt_temp.Add(line); break;
                            case 3: list_of_md_temp.Add(line); break;
                            case 4: list_of_latex_temp.Add(line); break;
                        }
                        if (file_type == type)
                        {
                            switch (type)
                            {
                                case 0: temp_dir = "templates\\src\\"; break;
                                case 1: temp_dir = "templates\\tb\\"; break;
                                case 2: temp_dir = "templates\\doc\\txt\\"; break;
                                case 3: temp_dir = "templates\\doc\\md\\"; break;
                                case 4: temp_dir = "templates\\doc\\latex\\"; break;
                            }
                            list.Items.Add(line);
                            if (line == edit_file_name)
                            {
                                list.SelectedIndex = index;
                                edit_file_name = line;
                                view_file(temp_dir, edit_file_name);
                            }
                            ++index;
                        }
                    }
                }
            }
            disable_edit();
            list.SelectedIndex = 0;
            sr.Close();
        }
    }
}
