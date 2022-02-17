
namespace PaddleOCRSharp
{ 
   /// <summary>
   /// 模型配置对象
   /// </summary>
    public  class OCRModelConfig
    {
        /// <summary>
        /// det_infer模型路径
        /// </summary>
        public string det_infer { get; set; }
        /// <summary>
        /// det_infer模型路径
        /// </summary>
        public string cls_infer { get; set; }
        /// <summary>
        /// det_infer模型路径
        /// </summary>
        public string rec_infer { get; set; }
        /// <summary>
        /// ppocr_keys.txt文件名全路径
        /// </summary>
        public string keys { get; set; }


       
    }
}
