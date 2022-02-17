using System.Collections.Generic;
using System.Drawing;
using System.Runtime.InteropServices;
using System;
using System.Linq;

namespace PaddleOCRSharp
{
    /// <summary>
    /// PaddleOCR识别引擎对象
    /// </summary>
    public class PaddleOCREngine : IDisposable
    {
        #region PaddleOCR API
        [DllImport("PaddleOCR.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = true)]
        internal static extern IntPtr Initialize(string det_infer, string cls_infer, string rec_infer, string keys, OCRParameter parameter);

        [DllImport("PaddleOCR.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = true)]
        internal static extern int Detect(IntPtr engine, string imagefile, out IntPtr result);

        [DllImport("PaddleOCR.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = true)]
        internal static extern int FreeEngine(IntPtr enginePtr);
        [DllImport("PaddleOCR.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = true)]
        internal static extern int FreeDetectResult(IntPtr intPtr);


        [DllImport("PaddleOCR.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = true)]
        internal static extern void DetectImage(string det_infer, string imagefile, OCRParameter parameter);
        #endregion

        #region 属性
        private IntPtr Engine;
        private float scale = 1.0f;
        #endregion

        #region 文本识别
        /// <summary>
        /// PaddleOCR识别引擎对象初始化
        /// </summary>
        /// <param name="config">模型配置对象，如果为空则按默认值</param>
        /// <param name="parameter">识别参数，为空均按缺省值</param>
        public PaddleOCREngine(OCRModelConfig config, OCRParameter parameter = null)
        {
            if (parameter == null) parameter = new OCRParameter();
            if (config == null)
            {
                string root = System.IO.Path.GetDirectoryName(typeof(OCRModelConfig).Assembly.Location);
                config = new OCRModelConfig();
                string modelPathroot = root + @"\inference";
                config.det_infer = modelPathroot + @"\ch_PP-OCRv2_det_infer";
                config.cls_infer = modelPathroot + @"\ch_ppocr_mobile_v2.0_cls_infer";
                config.rec_infer = modelPathroot + @"\ch_PP-OCRv2_rec_infer";
                config.keys = modelPathroot + @"\ppocr_keys.txt";
            }
            Engine = Initialize(config.det_infer, config.cls_infer, config.rec_infer, config.keys, parameter);
        }
        /// <summary>
        /// 对图像文件进行文本识别
        /// </summary>
        /// <param name="imagefile">图像文件</param>
        /// <returns></returns>
        public OCRResult DetectText(string imagefile)
        {
            if (!System.IO.File.Exists(imagefile)) throw new Exception($"文件{imagefile}不存在");
            IntPtr ptrResult;
            int textCount = Detect(Engine, imagefile, out ptrResult);
            if (textCount <= 0) return new OCRResult();
            if (ptrResult == IntPtr.Zero) return new OCRResult();
            OCRResult oCRResult = new OCRResult();
            IntPtr ptrFree = ptrResult;
            try
            {
                int textBlockAmount = (int)Marshal.PtrToStructure(ptrResult, typeof(int));
                //总textBlock个数

#if NET35
                ptrResult = (IntPtr)(ptrResult.ToInt64() + 4);
#else
                ptrResult = ptrResult + 4;
#endif

                IntPtr ptrtextBlock = (IntPtr)Marshal.PtrToStructure(ptrResult, typeof(IntPtr));
                ptrResult = ptrtextBlock;
                for (int i = 0; i < textBlockAmount; i++)
                {
                    //文本长度
                    int textBlockLen = (int)Marshal.PtrToStructure(ptrResult, typeof(int));
#if NET35
                    ptrResult = (IntPtr)(ptrResult.ToInt64() + 4);
#else
                    ptrResult = ptrResult + 4;
#endif

                    //文本指针
                    IntPtr textPointValue = (IntPtr)Marshal.PtrToStructure(ptrResult, typeof(IntPtr));

                    //文本
                    TextBlock textBlock = new TextBlock();
                    textBlock.Text = Marshal.PtrToStringUni(textPointValue);

                    //文本四个点
#if NET35
                    ptrResult = (IntPtr)(ptrResult.ToInt64() + 8);
#else
                    ptrResult = ptrResult +8;
#endif
                    for (int p = 0; p < 4; p++)
                    {
                        OCRPoint oCRPoint = (OCRPoint)Marshal.PtrToStructure(ptrResult, typeof(OCRPoint));
                        textBlock.BoxPoints.Add(new Point(oCRPoint.x, oCRPoint.y));

#if NET35
                        ptrResult = (IntPtr)(ptrResult.ToInt64() + Marshal.SizeOf(typeof(OCRPoint)));
#else
                        ptrResult = ptrResult + Marshal.SizeOf(typeof(OCRPoint));
#endif
                  }

                    //得分
                    float score = (float)Marshal.PtrToStructure(ptrResult, typeof(float));
                    textBlock.Score = score;
#if NET35
                    ptrResult = (IntPtr)(ptrResult.ToInt64() + 4);
#else
                    ptrResult = ptrResult + 4;
#endif

                    oCRResult.TextBlocks.Add(textBlock);
                }
            }
            catch (Exception)
            {
            }
            finally
            {
                FreeDetectResult(ptrFree);
            }
            oCRResult.TextBlocks.Reverse();
            return oCRResult;
        }

        /// <summary>
        ///对图像对象进行文本识别
        /// </summary>
        /// <param name="image">图像</param>
        /// <param name="parameter">参数</param>
        /// <returns></returns>
        public OCRResult DetectText(Image image)
        {
            if (image == null) throw new ArgumentNullException("image");
#if NET35
#else
            if (!Environment.Is64BitProcess) throw new Exception("暂不支持32位程序使用本OCR");
#endif

            string imagefile = "";
            #region 小图执行放大，分段线性缩放
            if (image.Width <= 50 || image.Height <= 50)
            {
                scale = 3;
            }
            else if (image.Width <= 100 || image.Height <= 100)
            {
                scale = 2;
            }
            else if (image.Width <= 150 || image.Height <= 150)
            {
                scale = 1.5f;
            }

            Bitmap bitmap = new Bitmap(image, new Size(Convert.ToInt32(image.Width * scale), Convert.ToInt32(image.Height * scale)));
            imagefile = System.IO.Path.GetTempPath() + Guid.NewGuid().ToString() + ".bmp";
            bitmap.Save(imagefile);
            bitmap.Dispose();
            GC.Collect();
            #endregion
            OCRResult result = DetectText(imagefile);

            #region 返回结果的区域需要还原比例
            int padding = 50;
            //if (parameter != null) padding = parameter.Padding;
            foreach (var item in result.TextBlocks)
            {
                List<Point> boxPoints = new List<Point>();

                for (int i = 0; i < item.BoxPoints.Count; i++)
                {
                    var point = item.BoxPoints[i];
                    Point p = new Point();
                    p.X = Convert.ToInt32(((float)(point.X - 0)) / scale);
                    p.Y = Convert.ToInt32(((float)(point.Y - 0)) / scale);
                    if (p.X < 0) p.X = 0;
                    if (p.Y < 0) p.Y = 0;
                    boxPoints.Add(p);
                }
                item.BoxPoints = boxPoints;
            }
            #endregion
            return result;
        }
        /// <summary>
        ///文本识别
        /// </summary>
        /// <param name="imagebyte">图像内存流</param>
        /// <param name="parameter">参数</param>
        /// <returns></returns>
        public OCRResult DetectText(byte[] imagebyte)
        {
            if (imagebyte == null) throw new ArgumentNullException("imagebyte");
#if NET35
#else
            if (!Environment.Is64BitProcess) throw new Exception("暂不支持32位程序使用本OCR");
#endif
            string imagefile = System.IO.Path.GetTempPath() + Guid.NewGuid().ToString() + ".bmp";
            System.IO.File.WriteAllBytes(imagefile, imagebyte);
            OCRResult result = DetectText(imagefile);
            System.IO.File.Delete(imagefile);
            if (result == null) return new OCRResult();
            return result;
        }

        #endregion

        #region 表格识别
        /// <summary>
        ///结构化文本识别
        /// </summary>
        /// <param name="image">图像</param>
        /// <param name="parameter">参数</param>
        /// <returns>表格识别结果</returns>
        public OCRStructureResult DetectStructure(Image image)
        {
            if (image == null) throw new ArgumentNullException("image");
#if NET35
#else
            if (!Environment.Is64BitProcess) throw new Exception("暂不支持32位程序使用本OCR");
#endif
            string imagefile = "";
            #region 小图执行放大，分段线性缩放
            if (image.Width <= 50 || image.Height <= 50)
            {
                scale = 3;
            }
            else if (image.Width <= 100 || image.Height <= 100)
            {
                scale = 2;
            }
            else if (image.Width <= 150 || image.Height <= 150)
            {
                scale = 1.5f;
            }

            Bitmap bitmap = new Bitmap(image, new Size(Convert.ToInt32(image.Width * scale), Convert.ToInt32(image.Height * scale)));
            imagefile = System.IO.Path.GetTempPath() + Guid.NewGuid().ToString() + ".bmp";
            bitmap.Save(imagefile);
            bitmap.Dispose();
            #endregion
            OCRResult result = DetectText(imagefile);
            #region 返回结果的区域需要还原比例
            int padding = 0;
            //if (parameter != null) padding = parameter.Padding;
            foreach (var item in result.TextBlocks)
            {
                List<Point> boxPoints = new List<Point>();

                for (int i = 0; i < item.BoxPoints.Count; i++)
                {
                    var point = item.BoxPoints[i];
                    Point p = new Point();
                    p.X = Convert.ToInt32(((float)(point.X - padding)) / scale);
                    p.Y = Convert.ToInt32(((float)(point.Y - padding)) / scale);
                    if (p.X < 0) p.X = 0;
                    if (p.Y < 0) p.Y = 0;
                    boxPoints.Add(p);
                }
                item.BoxPoints = boxPoints;
            }
            #endregion

            List<TextBlock> blocks = result.TextBlocks;
            if (blocks == null || blocks.Count == 0) return new OCRStructureResult();
            var listys = getzeroindexs(blocks.OrderBy(x => x.BoxPoints[0].Y).Select(x => x.BoxPoints[0].Y).ToArray(), 10);
            var listxs = getzeroindexs(blocks.OrderBy(x => x.BoxPoints[0].X).Select(x => x.BoxPoints[0].X).ToArray(), 10);

            int rowcount = listys.Count;
            int colcount = listxs.Count;
            OCRStructureResult structureResult = new OCRStructureResult();
            structureResult.TextBlocks = blocks;
            structureResult.RowCount = rowcount;
            structureResult.ColCount = colcount;
            structureResult.Cells = new List<StructureCells>();
            for (int i = 0; i < rowcount; i++)
            {
                int y_min = blocks.OrderBy(x => x.BoxPoints[0].Y).OrderBy(x => x.BoxPoints[0].Y).ToList()[listys[i]].BoxPoints[0].Y;
                int y_max = 99999;
                if (i < rowcount - 1)
                {
                    y_max = blocks.OrderBy(x => x.BoxPoints[0].Y).ToList()[listys[i + 1]].BoxPoints[0].Y;
                }

                for (int j = 0; j < colcount; j++)
                {
                    int x_min = blocks.OrderBy(x => x.BoxPoints[0].X).ToList()[listxs[j]].BoxPoints[0].X;
                    int x_max = 99999;

                    if (j < colcount - 1)
                    {
                        x_max = blocks.OrderBy(x => x.BoxPoints[0].X).ToList()[listxs[j + 1]].BoxPoints[0].X;
                    }

                    var textBlocks = blocks.Where(x => x.BoxPoints[0].X < x_max && x.BoxPoints[0].X >= x_min && x.BoxPoints[0].Y < y_max && x.BoxPoints[0].Y >= y_min).OrderBy(u => u.BoxPoints[0].X);
                    var texts = textBlocks.Select(x => x.Text).ToArray();

                    StructureCells cell = new StructureCells();
                    cell.Row = i;
                    cell.Col = j;

#if NET35
                    cell.Text = string.Join("", texts);
#else
                    cell.Text = string.Join<string>("", texts);
#endif


                    cell.TextBlocks = textBlocks.ToList();
                    structureResult.Cells.Add(cell);
                }
            }
            return structureResult;
        }
        /// <summary>
        /// 计算表格分割
        /// </summary>
        /// <param name="pixellist"></param>
        /// <param name="thresholdtozero"></param>
        /// <returns></returns>
        private List<int> getzeroindexs(int[] pixellist, int thresholdtozero = 20)
        {
            List<int> zerolist = new List<int>();
            zerolist.Add(0);
            for (int i = 0; i < pixellist.Length; i++)
            {
                if ((i < pixellist.Length - 1)
                    && (Math.Abs(pixellist[i + 1] - pixellist[i])) > thresholdtozero)
                {
                    //突增点
                    zerolist.Add(i + 1);
                }
            }
            return zerolist;
        }
        #endregion



        /// <summary>
        ///仅文本预测，在当前文件夹下保存文件名为ocr_vis.png的预测结果
        /// </summary>
        /// <param name="config">模型配置对象，如果为空则按默认值</param>
        /// <param name="imagefile">检测图像全路径</param>
        /// <param name="parameter">参数</param>
        public static void Detect(OCRModelConfig config, string imagefile, OCRParameter parameter=null)
        {
            if (parameter == null) parameter = new OCRParameter();
            if (config == null)
            {
                string root = System.IO.Path.GetDirectoryName(typeof(OCRModelConfig).Assembly.Location);
                config = new OCRModelConfig();
                string modelPathroot = root + @"\inference";
                config.det_infer = modelPathroot + @"\ch_PP-OCRv2_det_infer";
                config.cls_infer = modelPathroot + @"\ch_ppocr_mobile_v2.0_cls_infer";
                config.rec_infer = modelPathroot + @"\ch_PP-OCRv2_rec_infer";
                config.keys = modelPathroot + @"\ppocr_keys.txt";
            }
            DetectImage(config.det_infer, imagefile, parameter);
        }


        #region Dispose

        public void Dispose()
        {
            FreeEngine(Engine);
        }
        #endregion
    }
}