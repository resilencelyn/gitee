# 递归下载项目
git clone https://gitee.com/CV_Lab/yolov5_rt_tfjs.git --recursive

cd yolov5-tfjs/yolov5

# 安装依赖
pip install -r ./requirements.txt -U
pip install tensorflowjs==3.13.0

# 下载yolov5s.pt
wget -c -t 0 https://github.com/ultralytics/yolov5/releases/download/v6.1/yolov5s.pt

# 导出yolov5 tfjs文件
python export.py --weights yolov5s.pt --include tfjs

# 复制到工作目录
cp -r yolov5s_web_model ../yolov5_rt_tfjs_src/static
