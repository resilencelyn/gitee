# 🔍 OpenCV Webcam Script 代码检查

## 创建人：曾逸夫



### 🛡️ Gitee CodeCheck

| 版本 | 代码问题数 | 代码平均圈复杂度 | 代码重复率 | 有效代码行数 |  发行时间  |
| :--: | :--------: | :--------------: | :--------: | :----------: | :--------: |
| v0.1 |     0      |       11.5       |    0.0%    |      97      | 2022-01-01 |
| v0.2 |     0      |       6.3⬇️       |    0.0%    |     185⬆️     | 2022-01-07 |
| v0.3 |     0      |       5.4⬇️       |    0.0%    |     259⬆️     | 2022-01-17 |
| v0.4 |     0      |       6.1⬆️       |    0.0%    |     372⬆️     | 2022-01-24 |
| v0.5 |     0      |       6.0⬇️       |    0.0%    |     468⬆️     | 2022-02-06 |

用于衡量代码复杂度（与代码可维护、测试性相关），圈复杂度越高，重构码出错的概率越高。具体风险评估建议如下：<span style="color:green;">1-10（低风险）</span>；<span style="color:#EEAD0E;">11-20（中风险）</span>；<span style="color:red;">21-50（高风险）</span>；<span style="color:#8B2323;">51+（极高风险）</span>



### 🛡️ [lizard](https://github.com/terryyin/lizard) 代码检查

| 版本 | Total nloc | Avg.NLOC | AvgCCN | Avg.token | Fun Cnt |  发行时间  |
| :--: | :--------: | :------: | :----: | :-------: | :-----: | :--------: |
| v0.1 |     97     |   23.7   |  7.7   |   242.3   |    3    | 2022-01-01 |
| v0.2 |    185⬆️    |  13.7⬆️   |  3.2⬇️  |  131.3⬇️   |   6⬆️    | 2022-01-07 |
| v0.3 |    259⬆️    |  15.1⬆️   |  2.6⬇️  |  109.1⬇️   |   9⬆️    | 2022-01-17 |
| v0.4 |    372⬆️    |  17.5⬆️   |  3.2⬆️  |  114.9⬆️   |   13⬆️   | 2022-01-24 |
| v0.5 |    468⬆️    |  16.7⬇️   |  3.1⬇️  |  116.4⬆️   |   18⬆️   | 2022-02-06 |



<details open>
<summary>⚡ v0.5</summary>

```shell
================================================
  NLOC    CCN   token  PARAM  length  location  
------------------------------------------------
      14     10    153      4      15 increment_path@7-21@./opencv_webcam/utils/ows_path.py
       7      4     65      1       7 hotkey_judge@9-15@./opencv_webcam/utils/hotkey.py
       3      1     31      1       4 argsYaml@13-16@./opencv_webcam/utils/args_yaml.py
       4      2     21      1       4 is_compressFile@21-24@./opencv_webcam/utils/compress.py
      31      6    209      5      44 webcam_compress@28-71@./opencv_webcam/utils/compress.py
       3      1     20      1       3 add_fronts@16-18@./opencv_webcam/utils/fonts_opt.py
      15      5     76      1      19 is_fonts@22-40@./opencv_webcam/utils/fonts_opt.py
      27      2    298      3      45 createLineChart@29-73@./opencv_webcam/utils/plot.py
      21      4    175      1      27 csv2chart@77-103@./opencv_webcam/utils/plot.py
       4      2     22      1       4 is_time@9-12@./opencv_webcam/utils/time_format.py
      23      9    134      1      24 time_format@16-39@./opencv_webcam/utils/time_format.py
       6      2     42      1       7 is_logSuffix@11-17@./opencv_webcam/utils/log.py
       3      1     24      3       3 log_management@21-23@./opencv_webcam/utils/log.py
       3      1     23      2       3 date_time_frames@27-29@./opencv_webcam/utils/log.py
      10      1     23     10      10 frame_opt@10-19@./opencv_webcam/utils/frame_opt.py
      52      2    499      1      55 parse_args@26-80@./opencv_webcam/opencv_webcam.py
      24      1    101     24      24 webcam_opencv@84-107@./opencv_webcam/opencv_webcam.py
      50      1    179      1      57 main@219-275@./opencv_webcam/opencv_webcam.py
11 file analyzed.
==============================================================
NLOC    Avg.NLOC  AvgCCN  Avg.token  function_cnt    file
--------------------------------------------------------------
     17      14.0    10.0      153.0         1     ./opencv_webcam/utils/ows_path.py
      8       7.0     4.0       65.0         1     ./opencv_webcam/utils/hotkey.py
      6       3.0     1.0       31.0         1     ./opencv_webcam/utils/args_yaml.py
     45      17.5     4.0      115.0         2     ./opencv_webcam/utils/compress.py
     23       9.0     3.0       48.0         2     ./opencv_webcam/utils/fonts_opt.py
     63      24.0     3.0      236.5         2     ./opencv_webcam/utils/plot.py
     28      13.5     5.5       78.0         2     ./opencv_webcam/utils/time_format.py
      0       0.0     0.0        0.0         0     ./opencv_webcam/utils/__init__.py
     14       4.0     1.3       29.7         3     ./opencv_webcam/utils/log.py
     37      10.0     1.0       23.0         1     ./opencv_webcam/utils/frame_opt.py
    227      42.0     1.3      259.7         3     ./opencv_webcam/opencv_webcam.py

===============================================================================================================
No thresholds exceeded (cyclomatic_complexity > 15 or length > 1000 or nloc > 1000000 or parameter_count > 100)
==========================================================================================
Total nloc   Avg.NLOC  AvgCCN  Avg.token   Fun Cnt  Warning cnt   Fun Rt   nloc Rt
------------------------------------------------------------------------------------------
       468      16.7     3.1      116.4       18            0      0.00    0.00
```

</details>


<details close>
<summary>⚡ v0.4</summary>

```shell
================================================
  NLOC    CCN   token  PARAM  length  location  
------------------------------------------------
      14     10    153      4      15 increment_path@7-21@./opencv_webcam-v0.4/utils/ows_path.py
       7      4     65      1       7 hotkey_judge@9-15@./opencv_webcam-v0.4/utils/hotkey.py
       3      1     31      1       3 argsYaml@13-15@./opencv_webcam-v0.4/utils/args_yaml.py
       4      2     21      1       4 is_compressFile@18-21@./opencv_webcam-v0.4/utils/compress.py
      24      6    170      5      33 webcam_compress@25-57@./opencv_webcam-v0.4/utils/compress.py
       4      2     22      1       4 is_time@9-12@./opencv_webcam-v0.4/utils/time_format.py
      27      9    152      1      32 time_format@16-47@./opencv_webcam-v0.4/utils/time_format.py
       6      2     42      1       7 is_logSuffix@11-17@./opencv_webcam-v0.4/utils/log.py
       3      1     24      3       3 log_management@21-23@./opencv_webcam-v0.4/utils/log.py
      10      1     23     10      10 frame_opt@10-19@./opencv_webcam-v0.4/utils/frame_opt.py
      52      2    507      1      55 parse_args@24-78@./opencv_webcam-v0.4/opencv_webcam.py
      24      1    105     25      24 webcam_opencv@82-105@./opencv_webcam-v0.4/opencv_webcam.py
      50      1    179      1      57 main@205-261@./opencv_webcam-v0.4/opencv_webcam.py
9 file analyzed.
==============================================================
NLOC    Avg.NLOC  AvgCCN  Avg.token  function_cnt    file
--------------------------------------------------------------
     17      14.0    10.0      153.0         1     ./opencv_webcam-v0.4/utils/ows_path.py
      8       7.0     4.0       65.0         1     ./opencv_webcam-v0.4/utils/hotkey.py
      6       3.0     1.0       31.0         1     ./opencv_webcam-v0.4/utils/args_yaml.py
     37      14.0     4.0       95.5         2     ./opencv_webcam-v0.4/utils/compress.py
     32      15.5     5.5       87.0         2     ./opencv_webcam-v0.4/utils/time_format.py
      0       0.0     0.0        0.0         0     ./opencv_webcam-v0.4/utils/__init__.py
     11       4.5     1.5       33.0         2     ./opencv_webcam-v0.4/utils/log.py
     48      10.0     1.0       23.0         1     ./opencv_webcam-v0.4/utils/frame_opt.py
    213      42.0     1.3      263.7         3     ./opencv_webcam-v0.4/opencv_webcam.py

===============================================================================================================
No thresholds exceeded (cyclomatic_complexity > 15 or length > 1000 or nloc > 1000000 or parameter_count > 100)
==========================================================================================
Total nloc   Avg.NLOC  AvgCCN  Avg.token   Fun Cnt  Warning cnt   Fun Rt   nloc Rt
------------------------------------------------------------------------------------------
       372      17.5     3.2      114.9       13            0      0.00    0.00
```

</details>


<details close>
<summary>⚡ v0.3</summary>

```shell
================================================
  NLOC    CCN   token  PARAM  length  location  
------------------------------------------------
      14     10    153      4      15 increment_path@7-21@./opencv_webcam-v0.3/utils/ows_path.py
       3      1     31      1       3 argsYaml@13-15@./opencv_webcam-v0.3/utils/args_yaml.py
       7      4     65      1       7 hotkey_judge@9-15@./opencv_webcam-v0.3/utils/hotkeyjudge.py
       6      2     42      1       7 is_logSuffix@11-17@./opencv_webcam-v0.3/utils/log.py
       3      1     24      3       3 log_management@21-23@./opencv_webcam-v0.3/utils/log.py
       2      1     23     10       2 frame_opt@11-12@./opencv_webcam-v0.3/utils/frame_opt.py
      42      2    415      1      44 parse_args@20-63@./opencv_webcam-v0.3/opencv_webcam.py
      19      1     85     20      19 webcam_opencv@67-85@./opencv_webcam-v0.3/opencv_webcam.py
      40      1    144      1      43 main@169-211@./opencv_webcam-v0.3/opencv_webcam.py
7 file analyzed.
==============================================================
NLOC    Avg.NLOC  AvgCCN  Avg.token  function_cnt    file
--------------------------------------------------------------
     17      14.0    10.0      153.0         1     ./opencv_webcam-v0.3/utils/ows_path.py
      6       3.0     1.0       31.0         1     ./opencv_webcam-v0.3/utils/args_yaml.py
      8       7.0     4.0       65.0         1     ./opencv_webcam-v0.3/utils/hotkeyjudge.py
      0       0.0     0.0        0.0         0     ./opencv_webcam-v0.3/utils/__init__.py
     11       4.5     1.5       33.0         2     ./opencv_webcam-v0.3/utils/log.py
     40       2.0     1.0       23.0         1     ./opencv_webcam-v0.3/utils/frame_opt.py
    177      33.7     1.3      214.7         3     ./opencv_webcam-v0.3/opencv_webcam.py

===============================================================================================================
No thresholds exceeded (cyclomatic_complexity > 15 or length > 1000 or nloc > 1000000 or parameter_count > 100)
==========================================================================================
Total nloc   Avg.NLOC  AvgCCN  Avg.token   Fun Cnt  Warning cnt   Fun Rt   nloc Rt
------------------------------------------------------------------------------------------
       259      15.1     2.6      109.1        9            0      0.00    0.00
```

</details>


<details close>
<summary>⚡ v0.2</summary>

```shell
================================================
  NLOC    CCN   token  PARAM  length  location  
------------------------------------------------
      14     10    153      4      15 increment_path@7-21@./opencv_webcam-v0.2/utils/ows_path.py
       7      4     65      1       7 hotkey_judge@9-15@./opencv_webcam-v0.2/utils/hotkeyjudge.py
       2      1     23     10       2 frame_opt@11-12@./opencv_webcam-v0.2/utils/frame_opt.py
      36      2    355      1      36 parse_args@17-52@./opencv_webcam-v0.2/opencv_webcam.py
       3      1     73     17       3 webcam_opencv@56-58@./opencv_webcam-v0.2/opencv_webcam.py
      20      1    119      1      22 main@121-142@./opencv_webcam-v0.2/opencv_webcam.py
5 file analyzed.
==============================================================
NLOC    Avg.NLOC  AvgCCN  Avg.token  function_cnt    file
--------------------------------------------------------------
     17      14.0    10.0      153.0         1     ./opencv_webcam-v0.2/utils/ows_path.py
      8       7.0     4.0       65.0         1     ./opencv_webcam-v0.2/utils/hotkeyjudge.py
      0       0.0     0.0        0.0         0     ./opencv_webcam-v0.2/utils/__init__.py
     40       2.0     1.0       23.0         1     ./opencv_webcam-v0.2/utils/frame_opt.py
    120      19.7     1.3      182.3         3     ./opencv_webcam-v0.2/opencv_webcam.py

===============================================================================================================
No thresholds exceeded (cyclomatic_complexity > 15 or length > 1000 or nloc > 1000000 or parameter_count > 100)
==========================================================================================
Total nloc   Avg.NLOC  AvgCCN  Avg.token   Fun Cnt  Warning cnt   Fun Rt   nloc Rt
------------------------------------------------------------------------------------------
       185      13.7     3.2      131.3        6            0      0.00    0.00
```

</details>


<details close>
<summary>⚡ v0.1</summary>

```shell
================================================
  NLOC    CCN   token  PARAM  length  location  
------------------------------------------------
      14      1    225      1      15 parse_args@14-28@./opencv_webcam-v0.1/opencv_webcam.py
      14     10    153      4      15 increment_path@31-45@./opencv_webcam-v0.1/opencv_webcam.py
      43     12    349     10      49 webcam_opencv@49-97@./opencv_webcam-v0.1/opencv_webcam.py
2 file analyzed.
==============================================================
NLOC    Avg.NLOC  AvgCCN  Avg.token  function_cnt    file
--------------------------------------------------------------
     96      23.7     7.7      242.3         3     ./opencv_webcam-v0.1/opencv_webcam.py
      1       0.0     0.0        0.0         0     ./opencv_webcam-v0.1/__init__.py

===============================================================================================================
No thresholds exceeded (cyclomatic_complexity > 15 or length > 1000 or nloc > 1000000 or parameter_count > 100)
==========================================================================================
Total nloc   Avg.NLOC  AvgCCN  Avg.token   Fun Cnt  Warning cnt   Fun Rt   nloc Rt
------------------------------------------------------------------------------------------
        97      23.7     7.7      242.3        3            0      0.00    0.00

```

</details>


### 💡 参考

- Gitee CodeCheck
- [lizard](https://github.com/terryyin/lizard)