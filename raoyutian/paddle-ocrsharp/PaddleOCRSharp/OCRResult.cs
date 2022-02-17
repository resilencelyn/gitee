using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
namespace PaddleOCRSharp
{
    /// <summary>
    /// OCR识别结果
    /// </summary>
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public  class OCRResult
    {
        /// <summary>
        /// 文本块列表
        /// </summary>
        public List<TextBlock> TextBlocks { get; set; } = new List<TextBlock>();
        
        public string Text
        {
            get
            {

                string result = "";
                foreach (var item in TextBlocks)
                {
                    result += item.Text;
                }
                return result;
            }
        }

    }
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class OCRPoint
    {
        public int x;

        public int y;
    }

  
}
