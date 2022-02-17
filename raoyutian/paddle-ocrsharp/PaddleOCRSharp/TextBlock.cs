using System.Collections.Generic;
using System.Drawing;
using System.Text;

namespace PaddleOCRSharp
{
    /// <summary>
    /// 识别的文本块
    /// </summary>
    public class TextBlock
    {
        public List<Point> BoxPoints { get; set; } = new List<Point>();
        public string Text { get; set; }
        /// <summary>
        /// 得分
        /// </summary>
        public float Score { get; set; }
    }
}
