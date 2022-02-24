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
    public partial class new_file : Form
    {
        List<string> list_of_src_temp = new List<string>();
        List<string> list_of_tb_temp = new List<string>();
        List<string> list_of_latex_temp = new List<string>();
        List<string> list_of_md_temp = new List<string>();
        List<string> list_of_txt_temp = new List<string>();
        string author;
        public bool is_effctive = false;
        public string file_name;
        public string file_dir;
        public int type;
        string project_dir;
        public new_file(string author, string dir)
        {
            InitializeComponent();
            this.author = author;
            project_dir = dir;
        }


        private bool Check_Port_Table()// 检查表格里面的内容是不是合法的
        {
            bool flag = true;
            int rows = src_port.Rows.Count;
            if (rows == 1)
                return true;
            for (int i = 0; i < rows - 1; i++)
            {
                bool err_flag = true;
                // 检查是否符合变量名标准
                if (src_port.Rows[i].Cells[0].Value != null && Regex.IsMatch(src_port.Rows[i].Cells[0].Value.ToString(), @"^[_a-zA-Z0-9]+$"))
                {
                    DataGridViewComboBoxCell cmb = (DataGridViewComboBoxCell)src_port.Rows[i].Cells[1];
                    DataGridViewCheckBoxCell chb = (DataGridViewCheckBoxCell)src_port.Rows[i].Cells[2];
                    if (cmb.Value == null)
                    {
                        err_flag = false;
                    }
                    if (chb.Value != null && chb.Value.ToString() == "1") // 是一个bus
                    {
                        if (src_port.Rows[i].Cells[3].Value == null || src_port.Rows[i].Cells[4].Value == null)
                        {
                            err_flag = false;
                        }
                    }
                }
                else
                {
                    err_flag = false;
                }
                if (err_flag)
                    src_port.Rows[i].DefaultCellStyle.BackColor = Color.Empty;
                else
                {
                    src_port.Rows[i].DefaultCellStyle.BackColor = Color.Red;
                    flag = false;
                }
            }
            return flag;
        }

        private void Compete_Mask()// 补全富文本框里的掩码
        {
            string[] keywords = { "module", "begin", "end", "endmodule", "wire", "reg", "input", "output", "inout", "always", "case", "endcase", "default", "for", "generate", "repeat", "if", "else", "else if", "initial", "task", "repeat", "forever", "$stop()", "$finish()", "`timescale", "parameter", "localparam" };
            //string[] masks = { "&file&", "&author&", "&date&", "&params&", "&ports&", "&module_name&", "&tb_cfg&" };
            string[] comments = { @"\/\*(\s|.)*?\*\/", @"//[\s\S]*?\n" };
            Regex r;
            if (add_src.Checked)// 填ports 
            {
                if (Check_Port_Table()) // 检查一下ports对不对
                {
                    // 填写ports, 现在格式对齐还没想好要怎么做, 先这样吧~
                    string str="";
                    int rows = src_port.Rows.Count;
                    if (rows == 1)
                        str = "";
                    for (int i = 0; i < rows - 1; i++)
                    {
                        str += "\t"+ src_port.Rows[i].Cells[1].Value.ToString()+"\t";
                        if (src_port.Rows[i].Cells[2].Value != null && src_port.Rows[i].Cells[2].Value.ToString() == "1") //是一个总线
                        {
                            str += "[" + src_port.Rows[i].Cells[3].Value.ToString() + ":" + src_port.Rows[i].Cells[4].Value.ToString() + "]\t\t\t";
                        }
                        else
                        {
                            str += "\t\t\t\t";
                        }
                        str += src_port.Rows[i].Cells[0].Value.ToString();
                        if (i != rows - 2)
                            str += ",\r\n";
                        else
                            str += "\r\n";
                    }
                    r = new Regex("&ports&", System.Text.RegularExpressions.RegexOptions.IgnoreCase);
                    foreach (Match m in r.Matches(view_box.Text))
                    {
                        view_box.Select(m.Index, m.Length);
                        view_box.SelectedText = str;
                    }
                }
            }
            else if(add_tb.Checked)// 填一些testbech的配置
            {
                
                string str = "//************************Do Not Remove************************//\r\n";
                int clk, rst, stop;
                if(is_load_vcd.Checked)
                {
                    str += "\tinitial begin\r\n\t\t" + @"$dumpfile(""" + tb_module_name.Text + @".vcd"");" + "\r\n\t\t$dumpvars();\r\n\tend";
                }
                if(Regex.IsMatch(clk_freq.Text+ rst_cyc.Text+ stop_cyc.Text, @"^[0-9]*$"))
                {
                    clk = 1000 / int.Parse(clk_freq.Text); // convert to ns
                    rst = int.Parse(rst_cyc.Text) * clk;
                    stop = int.Parse(stop_cyc.Text) * clk;
                    str += "\r\n\treg clk, rst;\r\n\tinitial begin\r\n\t\tclk = 0;\r\n\t\t";
                    if (is_nrst.Checked) {
                        str += "rst = 1'b1; \t// rst is active low\r\n";
                        str += "\t\t#" + rst.ToString() + " rst = 1'b0;\r\n\t\t#" + clk.ToString() + " rst = 1'b1;\r\n";
                    }
                    else {
                        str += "rst = 1'b0; // rst is active high\r\n";
                        str += "\t\t#" + rst.ToString() + " rst = 1'b1;\r\n\t\t#" + clk.ToString() + " rst = 1'b0;\r\n";
                    }
                    str += "\t\t#" + stop.ToString() + " $finish();\r\n\tend";
                    str += "\r\n\talways begin\r\n\t\t#" + clk / 2 + " clk = ~clk; \t// clk freq : " + clk_freq.Text + " MHz\r\n\tend";
                }
                str += "\r\n//*************************************************************//";
                r = new Regex("&tb_cfg&", System.Text.RegularExpressions.RegexOptions.IgnoreCase);
                foreach (Match m in r.Matches(view_box.Text))
                {
                    view_box.Select(m.Index, m.Length);
                    view_box.SelectedText = str;
                }
            }
            // 填写其他的一些东西
            // &file&
            r = new Regex("&file&", System.Text.RegularExpressions.RegexOptions.IgnoreCase);
            foreach (Match m in r.Matches(view_box.Text))
            {
                view_box.Select(m.Index, m.Length);
                view_box.SelectedText = file_name_tb.Text+".v";
            }
            // &author&
            r = new Regex("&author&", System.Text.RegularExpressions.RegexOptions.IgnoreCase);
            foreach (Match m in r.Matches(view_box.Text))
            {
                view_box.Select(m.Index, m.Length);
                view_box.SelectedText = author;
            }
            // &data&
            r = new Regex("&date&", System.Text.RegularExpressions.RegexOptions.IgnoreCase);
            foreach (Match m in r.Matches(view_box.Text))
            {
                view_box.Select(m.Index, m.Length);
                view_box.SelectedText = DateTime.Now.ToString("yyyy - MM - dd");
            }
            // &module_name&
            r = new Regex("&module_name&", System.Text.RegularExpressions.RegexOptions.IgnoreCase);
            foreach (Match m in r.Matches(view_box.Text))
            {
                view_box.Select(m.Index, m.Length);
                if (add_src.Checked)
                    view_box.SelectedText = src_module_name_tb.Text;
                else if(add_tb.Checked)
                    view_box.SelectedText = tb_module_name.Text;
            }

            // 其他的还有待支持

            // 刷颜色
            if (!add_doc.Checked)
            {
                // 加粗关键字, 注释
                Highlight_Keywords(keywords, Color.OrangeRed); //关键字
                Highlight_Keywords(comments, Color.ForestGreen); //注释
            }
        }

        private void Highlight_Keywords(string[] keywords, Color color)// 给列表的词加高亮
        {
            foreach (string str in keywords)
            {
                Regex r = new Regex(str, System.Text.RegularExpressions.RegexOptions.IgnoreCase);

                foreach (Match m in r.Matches(view_box.Text))
                {
                    view_box.Select(m.Index, m.Length);
                    view_box.SelectionColor = color;
                }
            }
        }

        private void updata_RichTextBox(string dir)// 更新右边富文本框的内容
        {
             view_box.Clear();
            if (File.Exists(dir) == false)
            {
                MessageBox.Show(this, "Could not find files", "Information cue", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            StreamReader sr = new StreamReader(dir, Encoding.Default);
            string line;
            while ((line = sr.ReadLine()) != null)
            {
                view_box.AppendText(line);
                view_box.AppendText(Environment.NewLine);
            }
            sr.Close();
        }

        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)// 源码端口右键菜单删除
        {
            if (src_port.CurrentRow.Index != src_port.Rows.Count - 1) // 未提交的新行不会给删除
                src_port.Rows.Remove(src_port.CurrentRow);
        }

        private void new_file_Load(object sender, EventArgs e)
        {
            // 读取模板信息
            if(File.Exists("templates\\templates.ini") == false)
            {
                MessageBox.Show(this, "Could not find templates.ini!", "Information cue", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            // 全清了,不然重复添加啊
            src_temp.Items.Clear();
            tb_temp.Items.Clear();
            doc_temp.Items.Clear();
            list_of_src_temp.Clear();
            list_of_tb_temp.Clear();
            list_of_md_temp.Clear();
            list_of_txt_temp.Clear();
            list_of_latex_temp.Clear();
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
                    case "latex": file_type = 2; break;
                    case "md": file_type = 3; break;
                    case "txt": file_type = 4; break;
                    default: file_type = -1; break;
                }
                if (file_type != -1)
                {
                    line = sr.ReadLine();
                    file_counts = int.Parse(line);
                    for (int i = 0; i < file_counts; i++)
                    {
                        line = sr.ReadLine();
                        if (file_type == 0)
                        {
                            src_temp.Items.Add(line);
                            list_of_src_temp.Add(line);
                        }
                        else if (file_type == 1)
                        {
                            tb_temp.Items.Add(line);
                            list_of_tb_temp.Add(line);
                        }
                        else if (file_type == 2)
                        {
                            list_of_latex_temp.Add(line);
                        }
                        else if (file_type == 3)
                        {
                            list_of_md_temp.Add(line);
                        }
                        else if (file_type == 4)
                        {
                            list_of_txt_temp.Add(line);
                        }

                    }
                }

            }
            sr.Close();
            doc_file_type.SelectedIndex = 0;
            src_temp.SelectedIndex = 0;
            tb_temp.SelectedIndex = 0;
            doc_file_type_SelectedIndexChanged(sender, e);
            this.file_type.Text = ".v (Verilog Source)";
        }

        private void add_src_CheckedChanged(object sender, EventArgs e)
        {
            // 相关的enable, 不相关的disable掉
            if (add_src.Checked)
            {
                src_module_name_tb.Enabled = true;
                src_port.Enabled = true;
                src_temp.Enabled = true;
                add_src_temp_btn.Enabled = true;

                tb_temp.Enabled = false;
                tb_module_name.Enabled = false;
                clk_freq.Enabled = false;
                rst_cyc.Enabled = false;
                stop_cyc.Enabled = false;
                is_load_vcd.Enabled = false;
                is_nrst.Enabled = false;
                add_tb_temp_btn.Enabled = false; ;

                doc_temp.Enabled = false;
                add_doc_temp_btn.Enabled = false;
                doc_file_type.Enabled = false;

                file_type.Text = ".v (Verilog Source)";
                updata_RichTextBox("templates\\src\\" + src_temp.SelectedItem.ToString());
                Compete_Mask();
            }
        }

        private void add_tb_CheckedChanged(object sender, EventArgs e)
        {
            src_module_name_tb.Enabled = false;
            src_port.Enabled = false;
            src_temp.Enabled = false;
            add_src_temp_btn.Enabled = false;

            tb_temp.Enabled = true;
            //tb_module_name.Enabled = true;
            clk_freq.Enabled = true;
            rst_cyc.Enabled = true;
            stop_cyc.Enabled = true;
            is_load_vcd.Enabled = true;
            is_nrst.Enabled = true;
            add_tb_temp_btn.Enabled = true;

            doc_temp.Enabled = false;
            add_doc_temp_btn.Enabled = false;
            doc_file_type.Enabled = false;

            file_type.Text = ".v (Testbench)";
            updata_RichTextBox("templates\\tb\\" + tb_temp.SelectedItem.ToString());
            Compete_Mask();
        }

        private void add_doc_CheckedChanged(object sender, EventArgs e)
        {
            src_module_name_tb.Enabled = false;
            src_port.Enabled = false;
            src_temp.Enabled = false;
            add_src_temp_btn.Enabled = false;

            tb_temp.Enabled = false;
            tb_module_name.Enabled = false;
            clk_freq.Enabled = false;
            rst_cyc.Enabled = false;
            stop_cyc.Enabled = false;
            is_load_vcd.Enabled = false;
            is_nrst.Enabled = false;
            add_tb_temp_btn.Enabled = false;

            doc_temp.Enabled = true;
            add_doc_temp_btn.Enabled = true;
            doc_file_type.Enabled = true;
            file_type.Text = "(Document)";
            doc_file_type.SelectedIndex = 0;
        }

        private void file_name_tb_TextChanged(object sender, EventArgs e)
        {
            // 同步到另外的textbox里面
            tb_module_name.Text = file_name_tb.Text + "_tb";
            src_module_name_tb.Text = file_name_tb.Text;
        }

        private void doc_file_type_SelectedIndexChanged(object sender, EventArgs e)
        {
            doc_temp.Items.Clear();
            if(doc_file_type.SelectedIndex == 0) // txt
            {
                foreach(string str in list_of_txt_temp)
                    doc_temp.Items.Add(str);
                file_type.Text = ".txt (Text File)";
            }
            else if(doc_file_type.SelectedIndex == 1) // md
            {
                foreach (string str in list_of_md_temp)
                    doc_temp.Items.Add(str);
                file_type.Text = ".md (Markdown File)";
            }
            else if(doc_file_type.SelectedIndex == 2) // tex
            {
                foreach (string str in list_of_latex_temp)
                    doc_temp.Items.Add(str);
                file_type.Text = ".tex (LaTex File)";
            }
            //doc_temp.SelectedIndex = 0;
        }

        private void doc_temp_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (doc_file_type.SelectedIndex == 0) // txt
            {
                updata_RichTextBox("templates\\doc\\txt\\" + doc_temp.SelectedItem.ToString());
            }
            else if (doc_file_type.SelectedIndex == 1) // md
            {
                updata_RichTextBox("templates\\doc\\md\\" + doc_temp.SelectedItem.ToString());
            }
            else if (doc_file_type.SelectedIndex == 2) // tex
            {
                updata_RichTextBox("templates\\doc\\latex\\" + doc_temp.SelectedItem.ToString());
            }
        }

        private void ok_btn_Click(object sender, EventArgs e)
        {
            if (Regex.IsMatch(file_name_tb.Text, @"^[A-Za-z][A-Za-z0-9_]+$"))
            {
                if (add_src.Checked)
                {
                    file_name = file_name_tb.Text + ".v";
                    file_dir = project_dir + "\\src\\";
                    type = 0;
                }
                else if (add_tb.Checked)
                {
                    file_name = file_name_tb.Text + ".v";
                    file_dir = project_dir + "\\tb\\";
                    type = 1;
                }
                else if (add_doc.Checked)
                {
                    file_name = file_name_tb.Text;
                    if (doc_file_type.SelectedIndex == 0) // txt
                        file_name += ".txt";
                    else if (doc_file_type.SelectedIndex == 1) // md
                        file_name += ".md";
                    else if (doc_file_type.SelectedIndex == 2) // tex
                        file_name += ".tex";
                    file_dir = project_dir + "\\doc\\";
                    type = 2;
                }
                if (File.Exists(file_dir + file_name) == true)
                {
                    MessageBox.Show(this, file_name + " already exists!", "Information cue", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return;
                }
                is_effctive = true;
                view_box.SaveFile(file_dir + file_name, RichTextBoxStreamType.PlainText);
                this.Close();
            }
            else
            {
                MessageBox.Show(this, "Invalid file name!", "Information cue", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                is_effctive = false;
            }
        }

        private void cancel_btn_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void timer1_Tick(object sender, EventArgs e) // 放个小闹钟,每过一秒刷一次
        {
            
            if(add_src.Checked)
            {
                updata_RichTextBox("templates\\src\\" + src_temp.SelectedItem.ToString());
                Compete_Mask();
            }
            else if(add_tb.Checked)
            {
                updata_RichTextBox("templates\\tb\\" + tb_temp.SelectedItem.ToString());
                Compete_Mask();
            }
            else
            {

            }
            // add doc就不用刷了
        }

        private void add_src_temp_btn_Click(object sender, EventArgs e)
        {
            edit_template edit = new edit_template(0, src_temp.Text); //src
            edit.ShowDialog();
            new_file_Load(sender, e);
        }

        private void add_tb_temp_btn_Click(object sender, EventArgs e)
        {
            edit_template edit = new edit_template(1, tb_temp.Text); //src
            edit.ShowDialog();
            new_file_Load(sender, e);
        }

        private void add_doc_temp_btn_Click(object sender, EventArgs e)
        {
            edit_template edit;
            if (doc_file_type.SelectedIndex == 0) // txt
            {
                edit = new edit_template(2, doc_temp.Text);edit.ShowDialog();
            }
            else if (doc_file_type.SelectedIndex == 1) // md
            {
                edit = new edit_template(3, doc_temp.Text); edit.ShowDialog();
            }
            else if (doc_file_type.SelectedIndex == 2) // tex
            {
                edit = new edit_template(4, doc_temp.Text); edit.ShowDialog();
            }
            new_file_Load(sender, e);
        }

        private void src_temp_SelectedIndexChanged(object sender, EventArgs e)
        {

        }
    }
}
