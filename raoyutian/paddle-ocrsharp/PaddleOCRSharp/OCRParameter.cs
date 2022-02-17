using System.Runtime.InteropServices;
namespace PaddleOCRSharp
{

    /// <summary>
    /// OCR识别参数
    /// </summary>
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class OCRParameter
    {
        #region 通用参数
        /// <summary>
        /// 是否使用GPU，默认关闭
        /// </summary>
        public byte use_gpu { get; set; } = 0;
        /// <summary>
        /// GPU id，使用GPU时有效
        /// </summary>
        public int gpu_id { get; set; } = 0;
        /// <summary>
        /// 申请的GPU内存，使用GPU时有效
        /// </summary>
        public int gpu_mem { get; set; } = 4000;
        /// <summary>
        /// 使用线程数，默认2
        /// </summary>
        public int numThread { get; set; } = 2;
        /// <summary>
        /// 启用mkldnn加速，默认开启
        /// </summary>
        public byte Enable_mkldnn { get; set; } = 1;

        #endregion


        #region 检测模型相关
        /// <summary>
        /// 补白边，默认50，暂时没有用
        /// </summary>
        public int Padding { get; set; } = 50;
        /// <summary>
        /// 输入图像长宽大于960时，等比例缩放图像，使得图像最长边为960
        /// </summary>
        public int MaxSideLen { get; set; } = 960;
        /// <summary>
        /// DB后处理过滤box的阈值，如果检测存在漏框情况，可酌情减小 
        /// </summary>
        public float BoxScoreThresh { get; set; } = 0.5f;
        /// <summary>
        /// 用于过滤DB预测的二值化图像，设置为0.-0.3对结果影响不明显
        /// </summary>
        public float BoxThresh { get; set; } = 0.3f;
        /// <summary>
        /// 表示文本框的紧致程度，越小则文本框更靠近文本  
        /// </summary>
        public float UnClipRatio { get; set; } = 1.6f;
        /// <summary>
        /// DoAngle 默认1启用
        /// </summary>
        public byte DoAngle { get; set; } = 0;
        /// <summary>
        /// MostAngle 默认1启用
        /// </summary>
        public byte MostAngle { get; set; } = 0;


        /// <summary>
        /// 是否使用多边形框计算bbox score，false表示使用矩形框计算。矩形框计算速度更快，多边形框对弯曲文本区域计算更准确。
        /// </summary>
        public byte use_polygon_score { get; set; } = 0;
        /// <summary>
        /// 是否对结果进行可视化，为1时，会在当前文件夹下保存文件名为ocr_vis.png的预测结果。
        /// </summary>
        public byte visualize { get; set; } = 0;
        #endregion


        #region 方向分类器相关

        /// <summary>
        /// 启用方向选择器，默认关闭
        /// </summary>
        public byte use_angle_cls { get; set; } = 0;
        /// <summary>
        /// 方向分类器的得分阈值
        /// </summary>
        public float cls_thresh { get; set; } = 0.9f;
        #endregion
    }
}


