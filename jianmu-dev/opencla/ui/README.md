# OpenCLA UI
### 编译环境
 - NodeJS 14.19.1 LTS及以上

### 安装
```
# 安装依赖
npm install 或 yarn

# 启动 http://localhost:3000/
npm run serve 或 yarn serve

# 构建生产环境
npm run build 或 yarn build
```

### 构建docker镜像
```
# 创建docker镜像
docker build -t jianmudev/opencla-ui:${version} .

# 上传docker镜像
docker push jianmudev/opencla-ui:${version}
```

### 自定义主题
##### 自定义主题色

进入目录`src/components/theme/common`

1、打开`vars.less`

```
- 去掉@themeColor默认的主题色值
- 按照less语法修改common/vars.less中的@themeColor进行自定义主题色值的设置

例如：@themeColor: orange;
```

2、打开`vars.scss`

```
- 去掉$themeColor默认的主题色值
- 按照scss语法修改common/vars.scss中的$themeColor进行自定义主题色值的设置

例如：$themeColor: orange;
```


##### 自定义logo

进入目录`src/components/theme/common`

```
- 将自定义的logo保存在assets目录下
- 打开vars.less，去掉@logoPath的默认值
- 按照less的语法修改common/vars.less的@logoPath进行自定义logo设置

例如：
    自定义logo地址：assets/svgs/logo.svg
    自定义logo格式：@logoPath:'@/assets/svgs/logo.svg';
    自定义logo格式(url)：@logoPath: 'https://source.xxx/custom/logo.png'
```


##### 替换favicon.ico

进入目录public，使用需要的图标替换favicon.ico，替换的图标需要命名为favicon.ico

##### 自定义网页title

打开根目录下的index.html，将head中title修改为需要的内容

