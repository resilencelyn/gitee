if [ ! -d "./fonts" ]; then
  mkdir ./fonts
fi
cd ./fonts
# 下载宋体
wget -c -t 0 https://gitee.com/CV_Lab/opencv_webcam/attach_files/959173/download/SimSun.ttc
# 下载新罗马字体
wget -c -t 0 https://gitee.com/CV_Lab/opencv_webcam/attach_files/959172/download/TimesNewRoman.ttf