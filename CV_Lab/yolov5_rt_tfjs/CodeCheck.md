# 🔍 YOLOv5 RT TFJS 代码检查

## 创建人：曾逸夫



### 🛡️ Gitee CodeCheck

| 版本 | 代码问题数 | 代码平均圈复杂度 | 代码重复率 | 有效代码行数 |  发行时间  |
| :--: | :--------: | :--------------: | :--------: | :----------: | :--------: |
| v0.1 |     0      |       9.0        |    0.0%    |     126      | 2022-02-25 |
| v0.2 |     0      |      ⬆️13.3       |    0.0%    |     ⬆️213     | 2022-02-27 |
| v0.3 |     0      |       ⬇️6.6       |   ⬆️20.4%   |     ⬆️294     | 2022-03-02 |

用于衡量代码复杂度（与代码可维护、测试性相关），圈复杂度越高，重构码出错的概率越高。具体风险评估建议如下：<span style="color:green;">1-10（低风险）</span>；<span style="color:#EEAD0E;">11-20（中风险）</span>；<span style="color:red;">21-50（高风险）</span>；<span style="color:#8B2323;">51+（极高风险）</span>



### 🛡️ [lizard](https://github.com/terryyin/lizard) 代码检查

| 版本 | Total nloc | Avg.NLOC | AvgCCN | Avg.token | Fun Cnt |
| :--: | :--------: | :------: | :----: | :-------: | :-----: |
| v0.1 |    101     |   7.2    |  1.3   |   50.0    |   13    |
| v0.2 |    ⬆️159    |   ⬆️8.3   |  ⬆️1.9  |   ⬆️61.5   |   ⬆️18   |
| v0.3 |    ⬆️180    |   ⬇️7.8   |  1.9   |   ⬇️58.0   |   ⬆️20   |

❗ 注：lizard仅对JavaScript文件和Python文件进行检查


<details open>
<summary>⚡ v0.3</summary>

```shell
================================================
  NLOC    CCN   token  PARAM  length  location  
------------------------------------------------
       4      1     20      1       4 root@13-16@./yolov5_rt_tfjs/yolov5_rt_tfjs_fastapi/server.py
       2      1      9      0       2 index@8-9@./yolov5_rt_tfjs/yolov5_rt_tfjs_flask/server.py
       5      2     19      0       5 (anonymous)@45-49@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       4      1     10      0       4 (anonymous)@52-55@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
      10      3     51      0      17 (anonymous)@43-59@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       7      1     45      0      10 colorsSwitch@63-72@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       6      1     30      1       6 selectColor@75-80@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       4      1     62      2       4 hexToRgba@83-86@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
      55     11    565      1      80 renderPredictions@90-169@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       1      1     34      0       1 (anonymous)@178-178@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       5      1     22      0       7 (anonymous)@185-191@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       4      1     15      0      12 (anonymous)@181-192@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       5      1     57      2      21 detectFrame@173-193@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       3      1      6      0       3 video01.onloadedmetadata@222-224@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       3      1     11      0       6 (anonymous)@220-225@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       5      1     25      0      10 (anonymous)@217-226@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       3      1     12      0       4 (anonymous)@231-234@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
      21      4    107      1      40 openCam@197-236@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       7      2     53      0       8 (.onchange@240-247@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       3      1      6      0       3 webcam_open.onclick@251-253@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
3 file analyzed.
==============================================================
NLOC    Avg.NLOC  AvgCCN  Avg.token  function_cnt    file
--------------------------------------------------------------
     16       4.0     1.0       20.0         1     ./yolov5_rt_tfjs/yolov5_rt_tfjs_fastapi/server.py
      8       2.0     1.0        9.0         1     ./yolov5_rt_tfjs/yolov5_rt_tfjs_flask/server.py
    156       8.4     1.9       62.8        18     ./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js

===============================================================================================================
No thresholds exceeded (cyclomatic_complexity > 15 or length > 1000 or nloc > 1000000 or parameter_count > 100)
==========================================================================================
Total nloc   Avg.NLOC  AvgCCN  Avg.token   Fun Cnt  Warning cnt   Fun Rt   nloc Rt
------------------------------------------------------------------------------------------
       180       7.8     1.9       58.0       20            0      0.00    0.00
```

</details>


<details close>
<summary>⚡ v0.2</summary>

```shell
================================================
  NLOC    CCN   token  PARAM  length  location  
------------------------------------------------
       5      2     19      0       5 (anonymous)@38-42@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       4      1     10      0       4 (anonymous)@45-48@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
      10      3     51      0      17 (anonymous)@36-52@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       7      1     45      0      10 colorsSwitch@56-65@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       6      1     30      1       6 selectColor@68-73@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       4      1     62      2       4 hexToRgba@76-79@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
      55     11    565      1      80 renderPredictions@83-162@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       1      1     34      0       1 (anonymous)@168-168@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       5      1     22      0       7 (anonymous)@175-181@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       4      1     15      0      12 (anonymous)@171-182@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       4      1     34      2      18 detectFrame@166-183@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       3      1      6      0       3 video01.onloadedmetadata@212-214@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       3      1     11      0       6 (anonymous)@210-215@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       5      1     25      0      10 (anonymous)@207-216@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       3      1     12      0       4 (anonymous)@221-224@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
      21      4    107      1      40 openCam@187-226@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       7      2     53      0       8 (.onchange@230-237@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       3      1      6      0       3 webcam_open.onclick@241-243@./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
1 file analyzed.
==============================================================
NLOC    Avg.NLOC  AvgCCN  Avg.token  function_cnt    file
--------------------------------------------------------------
    159       8.3     1.9       61.5        18     ./yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js

===============================================================================================================
No thresholds exceeded (cyclomatic_complexity > 15 or length > 1000 or nloc > 1000000 or parameter_count > 100)
==========================================================================================
Total nloc   Avg.NLOC  AvgCCN  Avg.token   Fun Cnt  Warning cnt   Fun Rt   nloc Rt
------------------------------------------------------------------------------------------
       159       8.3     1.9       61.5       18            0      0.00    0.00
```

</details>

<details close>
<summary>⚡ v0.1</summary>

```shell
================================================
  NLOC    CCN   token  PARAM  length  location  
------------------------------------------------
       6      1     45      0       9 colorsSwitch@31-39@yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       6      1     30      1       6 selectColor@42-47@yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
      35      3    342      1      52 renderPredictions@51-102@yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       2      1     34      0       2 (anonymous)@108-109@yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       4      1     14      0       7 (anonymous)@116-122@yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       4      1     15      0      12 (anonymous)@112-123@yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       4      1     33      2      19 detectFrame@106-124@yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       3      1      6      0       3 video01.onloadedmetadata@143-145@yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       3      1     11      0       6 (anonymous)@141-146@yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       5      1     25      0      10 (anonymous)@138-147@yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       3      1     12      0       4 (anonymous)@152-155@yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
      15      3     77      0      30 componentDidMount@128-157@yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
       3      1      6      0       3 webcam_open.onclick@162-164@yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js
1 file analyzed.
==============================================================
NLOC    Avg.NLOC  AvgCCN  Avg.token  function_cnt    file
--------------------------------------------------------------
    101       7.2     1.3       50.0        13     yolov5_rt_tfjs/yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js

===============================================================================================================
No thresholds exceeded (cyclomatic_complexity > 15 or length > 1000 or nloc > 1000000 or parameter_count > 100)
==========================================================================================
Total nloc   Avg.NLOC  AvgCCN  Avg.token   Fun Cnt  Warning cnt   Fun Rt   nloc Rt
------------------------------------------------------------------------------------------
       101       7.2     1.3       50.0       13            0      0.00    0.00
```

</details>

### 💡 参考

- Gitee CodeCheck
- [lizard](https://github.com/terryyin/lizard)