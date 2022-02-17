using System.Collections.Generic;
namespace PaddleOCRSharp
{
    /// <summary>
    /// OCR结构化识别结果
    /// </summary>
    public sealed class OCRStructureResult
    {
        public OCRStructureResult()
        {
            Cells = new List<StructureCells>();
            TextBlocks = new List<TextBlock>();
        }

        /// <summary>
        /// 行数
        /// </summary>
        public int RowCount { get; set; }
        /// <summary>
        /// 列数
        /// </summary>
        public int ColCount { get; set; }
        /// <summary>
        /// 单元格 列表
        /// </summary>
        public List<StructureCells> Cells { get; set; }

        /// <summary>
        /// 文本块列表
        /// </summary>
        public List<TextBlock> TextBlocks { get; set; }

    }

    /// <summary>
    /// 单元格
    /// </summary>
    public sealed class StructureCells
    {
        public StructureCells()
        {
            TextBlocks = new List<TextBlock>();
        }

        /// <summary>
        /// 行数
        /// </summary>
        public int Row { get; set; }
        /// <summary>
        /// 列数
        /// </summary>
        public int Col { get; set; }
        /// <summary>
        /// 文本块
        /// </summary>
        public List<TextBlock> TextBlocks { get; set; }
        /// <summary>
        /// 识别文本
        /// </summary>
        public string Text { get; set; }
    }

}

