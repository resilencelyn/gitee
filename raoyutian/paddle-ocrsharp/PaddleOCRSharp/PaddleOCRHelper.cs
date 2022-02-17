using System;
using System.Drawing;
namespace PaddleOCRSharp
{
    /// <summary>
    /// PaddleOCR NET帮助类
    /// </summary>
    public static class PaddleOCRHelper
    {
        /// <summary>
        /// Gitee（码云）项目地址
        /// </summary>
        public static string GiteeUrl => "https://gitee.com/raoyutian/paddle-ocrsharp";
        /// <summary>
        /// Github项目地址
        /// </summary>
        public static string GithubUrl => "https://github.com/raoyutian/PaddleOCRSharp";
 
        /// <summary>
        /// 文本识别
        /// </summary>
        /// <param name="imagefile">图像文件</param>
        /// <param name="parameter"></param>
        /// <returns></returns>
       [Obsolete("请用PaddleOCREngine引擎进行文字识别,该方法将在下一版本移除", false)]
        public static OCRResult DetectText(string imagefile, OCRModelConfig modelConfig,  OCRParameter parameter = null)
        {
            using (PaddleOCREngine engine = new PaddleOCREngine(modelConfig, parameter))
            {
                return engine.DetectText(imagefile);
            }
        }

        /// <summary>
        ///文本识别
        /// </summary>
        /// <param name="image">图像</param>
        /// <param name="parameter">参数</param>
        /// <returns></returns>
        [Obsolete("请用PaddleOCREngine引擎进行文字识别,该方法将在下一版本移除", false)]
        public static OCRResult DetectText(Image image, OCRModelConfig modelConfig, OCRParameter parameter = null)
        {
            using (PaddleOCREngine engine = new PaddleOCREngine(modelConfig, parameter))
            {
                return engine.DetectText(image);
            }
        }
        /// <summary>
        ///文本识别
        /// </summary>
        /// <param name="imagebyte">图像内存流</param>
        /// <param name="parameter">参数</param>
        /// <returns></returns>
        [Obsolete("请用PaddleOCREngine引擎进行文字识别,该方法将在下一版本移除", false)]
        public static OCRResult DetectText(byte[] imagebyte, OCRModelConfig modelConfig, OCRParameter parameter = null)
        {
            using (PaddleOCREngine engine = new PaddleOCREngine(modelConfig, parameter))
            {
                return engine.DetectText(imagebyte);
            }
        }
        /// <summary>
        ///结构化文本识别(临时解决方案，需要的请研究官方的结构化识别)
        /// </summary>
        /// <param name="image">图像</param>
        /// <param name="parameter">参数</param>
        /// <returns></returns>
        [Obsolete("请用PaddleOCREngine引擎进行文字识别,该方法将在下一版本移除", false)]
        public static OCRStructureResult DetectStructure(Image image, OCRModelConfig modelConfig, OCRParameter parameter = null)
        {
            using (PaddleOCREngine engine = new PaddleOCREngine(modelConfig, parameter))
            {
                return engine.DetectStructure(image);
            }
        }
  
    }
}
