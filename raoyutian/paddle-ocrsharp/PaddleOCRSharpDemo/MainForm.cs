
using PaddleOCRSharp;
using System;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;
namespace PaddleOCRSharpDemo
{
    /// <summary>
    /// PaddleOCRSharp使用示例
    /// </summary>
    public partial class MainForm : Form
    {
        Bitmap bmp;
        public MainForm()
        {
            InitializeComponent();
        }
       
        /// <summary>
        /// 打开本地图片
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void toolStripopenFile_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "*.*|*.bmp;*.jpg;*.jpeg;*.tiff;*.tiff;*.png";
            if (ofd.ShowDialog() != DialogResult.OK) return;
            var imagebyte = File.ReadAllBytes(ofd.FileName);
            bmp = new Bitmap(new MemoryStream(imagebyte));
            pictureBox1.BackgroundImage = bmp;

            richTextBox1.Clear();
            richTextBox1.Show();
            dataGridView1.Hide();
            if (bmp == null) return;

            // 切换服务器模型库
            //OCRModelConfig config = new OCRModelConfig();
            //string root = Environment.CurrentDirectory;
            //config = new OCRModelConfig();
            //string modelPathroot = root + @"\inferenceserver";
            //config.det_infer = modelPathroot + @"\ch_ppocr_server_v2.0_det_infer";
            //config.cls_infer = modelPathroot + @"\ch_ppocr_mobile_v2.0_cls_infer";
            //config.rec_infer = modelPathroot + @"\ch_ppocr_server_v2.0_rec_infer";
            //config.keys = modelPathroot + @"\ppocr_keys.txt";


            //OCR参数
            OCRParameter oCRParameter = new OCRParameter();
            oCRParameter.numThread = 6;
            oCRParameter.Enable_mkldnn = 1;
            oCRParameter.use_angle_cls = 1;
            oCRParameter.use_polygon_score = 1;
            oCRParameter.BoxScoreThresh = 0.1f;
            OCRModelConfig config = null;
            //OCRParameter oCRParameter = null;
            OCRResult ocrResult = new OCRResult();
            using (PaddleOCREngine engine = new PaddleOCREngine(config, oCRParameter))
            {
                ocrResult = engine.DetectText(bmp);

            }
            ShowOCRResult(ocrResult);




        }
        /// <summary>
        /// 识别本地图片
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void toolStripLabel1_Click(object sender, EventArgs e)
        {
           
        }


        /// <summary>
        /// 识别截图文本
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void toolStripLabel2_Click(object sender, EventArgs e)
        {
            this.Hide();
            richTextBox1.Clear();
            richTextBox1.Show();
            dataGridView1.Hide();
            ScreenCapturer.ScreenCapturerTool screenCapturer = new ScreenCapturer.ScreenCapturerTool();
            if (screenCapturer.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                bmp = (Bitmap)screenCapturer.Image;
                pictureBox1.BackgroundImage = bmp;
                try
                {
                    OCRResult ocrResult = new OCRResult();
                    using (PaddleOCREngine engine = new PaddleOCREngine(null, null))
                    {
                        ocrResult = engine.DetectText(bmp);
                        ShowOCRResult(ocrResult);
                    }
                }
                catch (Exception ex)
                {
                }

            }
            this.Show();
        }
        /// <summary>
        /// 识别截图表格
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void toolStripLabel3_Click(object sender, EventArgs e)
        {
            this.Hide();

            ScreenCapturer.ScreenCapturerTool screenCapturer = new ScreenCapturer.ScreenCapturerTool();
            if (screenCapturer.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                bmp = (Bitmap)screenCapturer.Image;
                pictureBox1.BackgroundImage = bmp;
                richTextBox1.Hide();

                OCRStructureResult ocrResult = new OCRStructureResult();
                using (PaddleOCREngine engine = new PaddleOCREngine(null, null))
                {
                    ocrResult = engine.DetectStructure(bmp);
                }
                int rowcount = ocrResult.RowCount;
                int colcount = ocrResult.ColCount;
                DataTable dataTable = new DataTable();
                for (int i = 0; i < colcount; i++)
                {
                    dataTable.Columns.Add("Column" + (i + 1).ToString(), typeof(string));
                }
                for (int i = 0; i < rowcount; i++)
                {
                    DataRow row = dataTable.Rows.Add();
                    for (int j = 0; j < colcount; j++)
                    {
                        row[j] = ocrResult.Cells.FirstOrDefault(x => x.Row == i && x.Col == j).Text;
                    }
                }
                dataGridView1.DataSource = dataTable;
                dataGridView1.Show();
            }
            this.Show();
        }

        /// <summary>
        /// 检测
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void toolStripLabel4_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "*.*|*.bmp;*.jpg;*.jpeg;*.tiff;*.tiff;*.png";
            if (ofd.ShowDialog() != DialogResult.OK) return;

            //OCR参数
            OCRParameter oCRParameter = new OCRParameter();
            oCRParameter.numThread = 6;
            oCRParameter.Enable_mkldnn = 1;
            oCRParameter.use_angle_cls = 1;
            oCRParameter.use_polygon_score = 1;
            oCRParameter.MaxSideLen = 2048;
            oCRParameter.BoxScoreThresh = 0.1f;
           

            PaddleOCREngine.Detect(null, ofd.FileName,oCRParameter);

            string file = Environment.CurrentDirectory + "\\ocr_vis.png";
            var imagebyte = File.ReadAllBytes(file);
            bmp = new Bitmap(new MemoryStream(imagebyte));
            pictureBox1.BackgroundImage = bmp;
        }


        /// <summary>
        /// 显示结果
        /// </summary>
        private void ShowOCRResult(OCRResult ocrResult)
        {
            Bitmap bitmap = (Bitmap)this.pictureBox1.BackgroundImage;
            foreach (var item in ocrResult.TextBlocks)
            {
                richTextBox1.AppendText(item.Text + ";score:" + item.Score + "\n");
                using (Graphics g = Graphics.FromImage(bitmap))
                {
                    g.DrawPolygon(new Pen(Brushes.Red,2), item.BoxPoints.ToArray());
                }
            }
            pictureBox1.BackgroundImage = null;
            pictureBox1.BackgroundImage = bitmap;
        }
    }
}
