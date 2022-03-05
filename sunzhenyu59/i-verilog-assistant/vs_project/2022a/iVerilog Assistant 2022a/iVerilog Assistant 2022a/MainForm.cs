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
    public partial class MainForm : Form
    {
        string project_dir, project_name, username, editor_path, iv_path;
        TreeNode prj = new TreeNode();
        TreeNode src = new TreeNode();
        TreeNode src_top = new TreeNode();
        TreeNode tb = new TreeNode();
        TreeNode doc = new TreeNode();
        TreeNode outside_prj = new TreeNode();
        string active_tb = "";
        public MainForm()
        {
            InitializeComponent();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            string info_path = "";
            if (is_top_most.Checked == true)
            {
                this.TopMost = true;
            }
            else
            {
                this.TopMost = false;
            }
            if (tree.Nodes.Count !=0 && tree.SelectedNode != null && tree.SelectedNode.Level >=2)
            {
                Console.WriteLine("in");
                if (tree.SelectedNode.Parent == src_top)
                    info_path = "\\src\\" + tree.SelectedNode.Text;
                else if (tree.SelectedNode.Parent == tb)
                {
                    if (tree.SelectedNode.Text.Contains("[Active Testbench]"))
                        info_path = project_dir + "\\tb\\" + active_tb;
                    else
                        info_path = project_dir + "\\tb\\" + tree.SelectedNode.Text;
                }
                else if (tree.SelectedNode.Parent == doc)
                    info_path = project_dir + "\\doc\\" + tree.SelectedNode.Text;
                else if (tree.SelectedNode.Parent == src)
                    info_path = project_dir + "\\src\\" + tree.SelectedNode.Text;
                else
                    info_path = "";
                if (info_path != "")
                {
                    file_info.Text = "[" + tree.SelectedNode.Text + "] Modified : " + File.GetLastWriteTimeUtc(info_path).ToLongTimeString();
                }
            }
            else
            {
                file_info.Text = "";
            }

        }
        private bool Read_Project_File() //读工程文件
        {
            StreamReader sr = new StreamReader(project_dir + "\\" + project_name + ".ivprj", Encoding.Default);
            string line;
            int file_cnt = 0;
            while ((line = sr.ReadLine()) != null)
            {
                if (line == "src")
                {
                    src_top.Text = "";
                    if ((line = sr.ReadLine()) != null)
                        file_cnt = int.Parse(line);
                    else return false;
                    for (int i = 0; i < file_cnt; i++)
                    {
                        line = sr.ReadLine();
                        src.Nodes.Add(new TreeNode(line));
                    }
                }
                else if (line == "tb")
                {
                    if ((line = sr.ReadLine()) != null)
                        file_cnt = int.Parse(line);
                    else return false;
                    for (int i = 0; i < file_cnt; i++)
                    {
                        line = sr.ReadLine();
                        tb.Nodes.Add(new TreeNode(line));
                    }
                }
                else if (line == "doc")
                {
                    if ((line = sr.ReadLine()) != null)
                        file_cnt = int.Parse(line);
                    else return false;
                    for (int i = 0; i < file_cnt; i++)
                    {
                        line = sr.ReadLine();
                        doc.Nodes.Add(new TreeNode(line));
                    }
                }
                else if (line == "top")
                {
                    line = sr.ReadLine();
                    if (line != "!")
                    {
                        Console.WriteLine(src.Nodes.Count);
                        TreeNode t;
                        while (src.Nodes.Count > 0)
                        {
                            for (int i = 0; i < src.Nodes.Count; i++)
                            {
                                if (src.Nodes[i].Text == line)
                                {
                                    src_top.Text = line;
                                    src.Nodes.RemoveAt(i);
                                }
                                else
                                {
                                    t = src.Nodes[i];
                                    src.Nodes.RemoveAt(i);
                                    src_top.Nodes.Add(t);
                                }
                            }
                        }
                        src.Nodes.Add(src_top);
                    }
                }
                else if (line == "active_tb")
                {
                    line = sr.ReadLine();
                    if (line != "!")
                    {
                        active_tb = line;
                        if (tb.Nodes.Count != 0)
                            foreach (TreeNode node in tb.Nodes)
                            {
                                if (node != null && node.Text == line)
                                {
                                    
                                    node.ForeColor = Color.Red;
                                    node.Text += " [Active Testbench]";
                                }
                            }
                    }
                }
            }
            sr.Close();
            return true;
        }
        private void Save_Project_File() //保存工程文件
        {
            if (project_name == "")
                return;
            FileStream fs = new FileStream(project_dir + "\\" + project_name + ".ivprj", FileMode.Create); //这个工程名没有扩展名
            StreamWriter sw = new StreamWriter(fs);
            sw.Write(project_name + "\r\nsrc\r\n");
            sw.WriteLine((src.Nodes.Count + src_top.Nodes.Count).ToString());
            if(src_top.Text != "")
            {
                sw.WriteLine(src_top.Text);
                for (int i = 0; i < src_top.Nodes.Count; i++)
                    sw.WriteLine(src_top.Nodes[i].Text);
            }
            else
            {
                for (int i = 0; i < src.Nodes.Count; i++)
                    sw.WriteLine(src.Nodes[i].Text);
            }
            sw.WriteLine("tb");
            sw.WriteLine(tb.Nodes.Count.ToString());
            for(int i=0;i<tb.Nodes.Count;i++)
            {
                if (tb.Nodes[i].Text.Contains("[Active Testbench]"))
                    sw.WriteLine(active_tb);
                else
                    sw.WriteLine(tb.Nodes[i].Text);
            }
            sw.WriteLine("doc");
            sw.WriteLine(doc.Nodes.Count.ToString());
            for (int i = 0; i < doc.Nodes.Count; i++)
                    sw.WriteLine(doc.Nodes[i].Text);
            sw.WriteLine("top");
            if (src_top.Text != "")
                sw.WriteLine(src_top.Text);
            else
                sw.WriteLine("!");
            sw.WriteLine("active_tb");
            if (active_tb != "")
                sw.WriteLine(active_tb);
            else
                sw.WriteLine("!");
            sw.Close();
            fs.Close();
        }
        private void Search_File() //查找文件
        {

        }
        private void Open_Project_Btn_Click(object sender, EventArgs e)
        {
            timer1.Enabled = false;// 关闭定时器,否则窗体点不动
            this.TopMost = false;
            create_open create_Open = new create_open();
            create_Open.ShowDialog();
            if (create_Open.is_effective)
            {
                Save_Project_File();
                tree.Nodes.Clear();
                prj.Nodes.Clear();
                outside_prj.Nodes.Clear();
                src.Nodes.Clear();
                tb.Nodes.Clear();
                doc.Nodes.Clear();
                src_top.Nodes.Clear();
                active_tb = "";
                Simulate_Btn.Enabled = true;
                Compile_Btn.Enabled = true;
                Add_Src_Btn.Enabled = true;
                tree.Enabled = true;
                project_dir = create_Open.project_dir;
                project_name = create_Open.project_name;
                prj.Text = "Project : " + project_name;
                src.Text = "Source";
                tb.Text = "Testbench";
                doc.Text = "Document";
                outside_prj.Text = "Outside Project";
                tree.Nodes.Add(prj);
                prj.Nodes.Add(src);
                prj.Nodes.Add(tb);
                prj.Nodes.Add(doc);
                //tree.Nodes.Add(outside_prj);
                Read_Project_File();
                tree.ExpandAll();
            }
            timer1.Enabled = true;// 打开定时器
        }

        private void Add_Src_Btn_Click(object sender, EventArgs e)
        {
            timer1.Enabled = false;// 关闭定时器,否则窗体点不动
            this.TopMost = false;
            new_file new_File = new new_file(username, project_dir);
            new_File.ShowDialog();
            if (new_File.is_effctive)
            {
                if (new_File.type == 0)  // source file
                {
                    if(src_top.Text == "") // 没有顶层
                        src.Nodes.Add(new TreeNode(new_File.file_name));
                    else
                        src_top.Nodes.Add(new TreeNode(new_File.file_name));
                }
                else if (new_File.type == 1)
                {
                    tb.Nodes.Add(new TreeNode(new_File.file_name));
                }
                else if (new_File.type == 2)
                {
                    doc.Nodes.Add(new TreeNode(new_File.file_name));
                }
                Save_Project_File();
            }
            timer1.Enabled = true;// 打开定时器
        }

        private void Simulate_Btn_Click(object sender, EventArgs e)
        {
            if (active_tb == "")
            {
                MessageBox.Show(this, "Active testbench has not been specified ! ", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (src_top.Text == "")
            {
                MessageBox.Show(this, "Top level module has not been specified ! ", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            FileStream fs = new FileStream(project_dir + "\\compile.cmd", FileMode.Create);
            StreamWriter sw = new StreamWriter(fs);
            sw.WriteLine("@echo off\r\ncolor 0a");
            if (iv_path == "Internal")
                sw.Write("\"" + System.Environment.CurrentDirectory + "\\iverilog\\bin\\iverilog.exe\"");
            else
                sw.Write("\"" + iv_path + "\\bin\\iverilog.exe\"");
            sw.Write(" -o " + "\"" + Path.GetFileNameWithoutExtension(active_tb) + ".o\" " + "\"tb\\" + active_tb + "\"" + " \"src\\" + src_top.Text + "\" ");
            for (int i = 0; i < src_top.Nodes.Count; i++)
            {
                sw.Write("\"src\\" + src_top.Nodes[i].Text + "\" ");
            }
            sw.Write("\r\n");
            if (iv_path == "Internal")
                sw.Write("\"" + System.Environment.CurrentDirectory + "\\iverilog\\bin\\vvp.exe\"");
            else
                sw.Write("\"" + iv_path + "\\bin\\vvp.exe\"");
            sw.Write(" \"" + Path.GetFileNameWithoutExtension(active_tb) + ".o\" \r\n");
            if (iv_path == "Internal")
                sw.Write("\"" + System.Environment.CurrentDirectory + "\\iverilog\\gtkwave\\bin\\gtkwave.exe\"");
            else
                sw.Write("\"" + iv_path + "\\gtkwave\\bin\\gtkwave.exe\"");
            sw.Write(" \"" + Path.GetFileNameWithoutExtension(active_tb) + "_tb.vcd\" \r\n");
            sw.Close();
            fs.Close();
            Process p = new Process();
            p.StartInfo.FileName = project_dir + "\\compile.cmd";
            p.StartInfo.WorkingDirectory = project_dir;
            p.Start();
            //p.WaitForExit();
            p.Close();
        }

        private void Compile_Btn_Click(object sender, EventArgs e)
        {
            if(active_tb == "")
            {
                MessageBox.Show(this, "Active testbench has not been specified ! ", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (src_top.Text =="")
            {
                MessageBox.Show(this, "Top level module has not been specified ! ", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            FileStream fs = new FileStream(project_dir + "\\compile.cmd", FileMode.Create);
            StreamWriter sw = new StreamWriter(fs);
            sw.WriteLine("@echo off\r\ncolor 0a");
            if (iv_path == "Internal")
                sw.Write("\""+ System.Environment.CurrentDirectory+"\\iverilog\\bin\\iverilog.exe\"");
            else
                sw.Write("\""+iv_path + "\\bin\\iverilog.exe\"");
            sw.Write(" -o " + "\"" + Path.GetFileNameWithoutExtension(active_tb) + ".o\" " + "\"tb\\" + active_tb+"\"" + " \"src\\"+src_top.Text+"\" ");
            for(int i=0; i<src_top.Nodes.Count;i++)
            {
                sw.Write("\"src\\"+src_top.Nodes[i].Text+"\" ");
            }
            sw.Write("\r\n");
            if (iv_path == "Internal")
                sw.Write("\"" + System.Environment.CurrentDirectory + "\\iverilog\\bin\\vvp.exe\"");
            else
                sw.Write("\"" + iv_path + "\\bin\\vvp.exe\"");
            sw.Write(" \"" + Path.GetFileNameWithoutExtension(active_tb) + ".o\" \r\npause");
            sw.Close();
            fs.Close();
            Process p = new Process();
            p.StartInfo.FileName = project_dir + "\\compile.cmd";
            p.StartInfo.WorkingDirectory = project_dir;
            p.Start();
            //p.WaitForExit();
            p.Close();
            //Console.WriteLine(Directory.GetCurrentDirectory());

        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {

        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            Save_Project_File();
        }

        private void menu_Opening(object sender, CancelEventArgs e) //控制右键菜单激活选项
        {
            if(tree.SelectedNode.Level >= 2)
            {
                menu.Items[0].Enabled = false; //Add
                if(tree.SelectedNode.Parent.Text=="Outside Project") //Add to
                    menu.Items[1].Enabled = true;
                else
                    menu.Items[1].Enabled = false;
                menu.Items[2].Enabled = true; //Rename
                if ((tree.SelectedNode.Level == 2 && tree.SelectedNode.Parent.Text == "Source" && src_top.Nodes.Count == 0) || tree.SelectedNode.Level== 3)
                    menu.Items[3].Enabled = true; // Top
                else
                    menu.Items[3].Enabled = false; // Top
                if (tree.SelectedNode.Parent.Text == "Testbench")
                    menu.Items[4].Enabled = true; //Active
                else
                    menu.Items[4].Enabled = false; //Active
                menu.Items[5].Enabled = true; //Delete
            }
            else if(tree.SelectedNode.Level == 1)
            {
                if(tree.SelectedNode.Parent.Text == "Outside Project")
                    menu.Items[0].Enabled = false; //Add
                else
                    menu.Items[0].Enabled = true; //Add
                menu.Items[1].Enabled = false;
                menu.Items[2].Enabled = false;
                menu.Items[3].Enabled = false;
                menu.Items[4].Enabled = false;
                menu.Items[5].Enabled = false;
            }
            else
            {
                menu.Items[0].Enabled = false;
                menu.Items[1].Enabled = false;
                menu.Items[2].Enabled = false;
                menu.Items[3].Enabled = false;
                menu.Items[4].Enabled = false;
                menu.Items[5].Enabled = false;
            }

        }

        private void addExistFilesToolStripMenuItem_Click(object sender, EventArgs e) //添加已存在文件
        {
            string add_file_name="";
            bool flag = true;
            timer1.Enabled = false;
            if (tree.SelectedNode.Text == "Source" || tree.SelectedNode.Text == "Testbench")
            {
                OpenFileDialog openFileDialog = new OpenFileDialog();
                openFileDialog.Filter = "Verilog Source File|*.v";
                openFileDialog.RestoreDirectory = true;
                openFileDialog.Multiselect = true;
                if (openFileDialog.ShowDialog() == DialogResult.OK)
                {
                    foreach (string file in openFileDialog.FileNames)
                    {
                        flag = true;
                        add_file_name = System.IO.Path.GetFileName(file);
                        if (src_top.Text != "")
                        {
                            for (int i = 0; i < src_top.Nodes.Count; i++)
                            {
                                if (src_top.Nodes[i].Text == add_file_name || src_top.Text == add_file_name)
                                    flag = false;
                            }
                            for (int i = 0; i < tb.Nodes.Count; i++)
                            {
                                if (tb.Nodes[i].Text == add_file_name || active_tb == add_file_name)
                                    flag = false;
                            }
                        }
                        else
                        {
                            for (int i = 0; i < src.Nodes.Count; i++)
                            {
                                if (src.Nodes[i].Text == add_file_name)
                                    flag = false;
                            }
                            for (int i = 0; i < tb.Nodes.Count; i++)
                            {
                                if (tb.Nodes[i].Text == add_file_name || active_tb == add_file_name)
                                    flag = false;
                            }
                        }
                        if (flag)
                        {
                            if (tree.SelectedNode.Text == "Source")
                            {
                                if (src_top.Text == "")
                                    src.Nodes.Add(new TreeNode(add_file_name));
                                else
                                    src_top.Nodes.Add(new TreeNode(add_file_name));
                                if (Path.GetDirectoryName(file) != project_dir + "\\src")
                                {
                                    if(!File.Exists(project_dir + "\\src\\" + add_file_name))
                                        System.IO.File.Copy(file, project_dir + "\\src\\" + add_file_name);
                                    else
                                        MessageBox.Show(this, add_file_name + " Exist!", "Information cue", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                                }
                            }
                            else if (tree.SelectedNode.Text == "Testbench")
                            {
                                tb.Nodes.Add(new TreeNode(add_file_name));
                                if (Path.GetDirectoryName(file) != project_dir + "\\tb")
                                {
                                    if (!File.Exists(project_dir + "\\tb\\" + add_file_name))
                                        File.Copy(file, project_dir + "\\tb\\" + add_file_name);
                                    else
                                        MessageBox.Show(this, add_file_name + " Exist!", "Information cue", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                                }
                            }
                        }
                    }
                }
            }
            else if (tree.SelectedNode.Text == "Document")
            {
                OpenFileDialog openFileDialog = new OpenFileDialog();
                openFileDialog.Filter = "Text File|*.txt|Markdown File|*.md|Latex File|*.tex";
                openFileDialog.RestoreDirectory = true;
                openFileDialog.Multiselect = true;
                if (openFileDialog.ShowDialog() == DialogResult.OK)
                {
                    foreach (string file in openFileDialog.FileNames)
                    {
                        flag = true;
                        add_file_name = System.IO.Path.GetFileName(file);
                        for (int i = 0; i < doc.Nodes.Count; i++)
                        {
                            if (doc.Nodes[i].Text == add_file_name)
                                flag = false;
                        }
                        if (flag)
                        {
                            doc.Nodes.Add(add_file_name);
                            if (Path.GetDirectoryName(file) != project_dir + "\\doc")
                            {
                                if(!File.Exists(project_dir + "\\doc\\" + add_file_name))
                                    File.Copy(file, project_dir + "\\doc\\" + add_file_name);
                                else
                                    MessageBox.Show(this, add_file_name + " Exist!", "Information cue", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                            }
                        } 
                    }
                }
            }
            timer1.Enabled = true;
            Save_Project_File();
        }

        private void setAsActiveTestbenchToolStripMenuItem_Click(object sender, EventArgs e) //设置活动testbench
        {
            if (active_tb != "")
            {
                for(int i=0;i<tb.Nodes.Count;i++)
                {
                    if(tb.Nodes[i].Text.Contains("[Active Testbench]"))
                    {
                        tb.Nodes[i].ForeColor = Color.Black;
                        tb.Nodes[i].Text = active_tb;
                    }
                }
            }
            active_tb = tree.SelectedNode.Text;
            tree.SelectedNode.ForeColor = Color.Red;
            tree.SelectedNode.Text += " [Active Testbench]";
            Save_Project_File();
        }

        private void setToolStripMenuItem_Click(object sender, EventArgs e) //设置顶层
        {
            string str;
            if (src_top.Text != "") //已有顶层, 交换一下名字就可以
            {
                str = src_top.Text;
                src_top.Text = tree.SelectedNode.Text;
                tree.SelectedNode.Text = str;
            }
            else //没有顶层就很麻烦
            {
                TreeNode t;
                src_top.Text = tree.SelectedNode.Text;
                while (src.Nodes.Count > 0)
                {
                    for (int i = 0; i < src.Nodes.Count; i++)
                    {
                        Console.WriteLine("process : " + src.Nodes[i].Text);
                        if (src.Nodes[i].Text == src_top.Text)
                        {
                            src.Nodes.RemoveAt(i);
                        }
                        else
                        {
                            t = src.Nodes[i];
                            src.Nodes.RemoveAt(i);
                            src_top.Nodes.Add(t);
                        }
                    }
                }
                src.Nodes.Add(src_top);
            }
            Save_Project_File();
        }

        private void deleteToolStripMenuItem_Click(object sender, EventArgs e) //删除
        {
            if(tree.SelectedNode == src_top) //想要删掉top
            {
                TreeNode t;
                while(src_top.Nodes.Count>0)
                {
                    for(int i=0;i< src_top.Nodes.Count;i++)
                    {
                        t = src_top.Nodes[i];
                        src_top.Nodes.RemoveAt(i);
                        src.Nodes.Add(t);
                    }
                }
                if (src_top.Text != "")
                    src.Nodes.Add(new TreeNode(src_top.Text));
                    // 把孤寡的top去掉
                src.Nodes.Remove(src_top);
                src_top.Text = "";
            }
            else
            {
                tree.SelectedNode.Remove();
            }
            Save_Project_File();
        }

        private void renameToolStripMenuItem_Click(object sender, EventArgs e) //重命名
        {

            Rename re = new Rename(tree.SelectedNode.Text);
            string new_name, old_name;
            timer1.Enabled = false;
            this.TopMost = false;
            old_name = tree.SelectedNode.Text;
            re.ShowDialog();
            timer1.Enabled = true;
            if (re.is_effective)
            {
                new_name = re.name;
                if(File.Exists(project_dir+"\\src\\"+new_name) || File.Exists(project_dir + "\\tb\\" + new_name) || File.Exists(project_dir + "\\doc\\" + new_name))
                {
                    MessageBox.Show(this, new_name + " Exist!", "Information cue", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
                else
                {
                    tree.SelectedNode.Text = new_name;
                    if (tree.SelectedNode.Parent == src)
                    {
                        File.Move(project_dir + "\\src\\" + old_name, project_dir + "\\src\\" + new_name);                    }
                    else if(tree.SelectedNode.Parent == tb)
                    {
                        if(old_name.Contains("[Active Testbench]"))
                        {
                            File.Move(project_dir + "\\tb\\" + active_tb, project_dir + "\\tb\\" + new_name);
                            tree.SelectedNode.Text = new_name + " [Active Testbench]";
                            active_tb = new_name;
                        }
                        else
                            File.Move(project_dir + "\\tb\\" + old_name, project_dir + "\\tb\\" + new_name);
                    }
                    else if(tree.SelectedNode.Parent == doc)
                    {
                        File.Move(project_dir + "\\doc\\" + old_name, project_dir + "\\doc\\" + new_name);
                    }
                    else if(tree.SelectedNode.Parent == src_top)
                    {
                        File.Move(project_dir + "\\src\\" + old_name, project_dir + "\\src\\" + new_name);
                    }
                }
                Save_Project_File();
            }
        }

        private void tools_btn_Click(object sender, EventArgs e)
        {

        }

        private void tree_AfterLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
          

        }

        private void tree_MouseDoubleClick(object sender, MouseEventArgs e) //实现双击打开文件
        {
            if(tree.SelectedNode == prj)
            {
                System.Diagnostics.Process.Start("Explorer.exe", project_dir);
            }
            else if(tree.SelectedNode == src)
            {
                System.Diagnostics.Process.Start("Explorer.exe", project_dir + "\\src");
            }
            else if (tree.SelectedNode == tb)
            {
                System.Diagnostics.Process.Start("Explorer.exe", project_dir + "\\tb");
            }
            else if (tree.SelectedNode == doc)
            {
                System.Diagnostics.Process.Start("Explorer.exe", project_dir + "\\doc");
            }
            else
            {
                if(tree.SelectedNode.Parent == src_top)
                    System.Diagnostics.Process.Start(editor_path, project_dir + "\\src\\"+ tree.SelectedNode.Text);
                else if(tree.SelectedNode.Parent == tb)
                {
                    if(tree.SelectedNode.Text.Contains("[Active Testbench]"))
                        System.Diagnostics.Process.Start(editor_path, project_dir + "\\tb\\" + active_tb);
                    else
                        System.Diagnostics.Process.Start(editor_path, project_dir + "\\tb\\" + tree.SelectedNode.Text);
                }
                else if(tree.SelectedNode.Parent == doc)
                    System.Diagnostics.Process.Start(editor_path, project_dir + "\\doc\\" + tree.SelectedNode.Text);
                else if(tree.SelectedNode.Parent == src)
                    System.Diagnostics.Process.Start(editor_path, project_dir + "\\src\\" + tree.SelectedNode.Text);
            }
            tree.SelectedNode.ExpandAll(); // 双击会收起来,给重新展开
        }

        void Reload_Config()
        {
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
                    default: break;
                }
            }
            sr.Close();
            lb_name.Text = "[" + username + "]";
            if (iv_path != "Internal")
                compiler.Text = "(Ext) ";
            else
                compiler.Text = "(Int) ";
        }
        private void Config_Btn_Click(object sender, EventArgs e)
        {
            timer1.Enabled = false;
            this.TopMost = false;
            config cfg = new config();
            cfg.ShowDialog();
            Reload_Config();
            timer1.Enabled = true;
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            Reload_Config();

        }
    }
}
