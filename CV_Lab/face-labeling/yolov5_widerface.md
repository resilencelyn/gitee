# 🔥 基于YOLOv5的人脸检测模型的构建

## 创建人：曾逸夫



### 💡 WIDER FACE数据集准备

📌 [官网](http://shuoyang1213.me/WIDERFACE/)下载4个文件：`WIDER Face Training Images`|`WIDER Face Validation Images`|`WIDER Face Testing Images`|`Face annotations`

📌 下载后会得到4个压缩文件：`WIDER_train.zip`|`WIDER_val.zip`|`WIDER_test.zip`|`wider_face_split.zip`

📌 其中，`wider_face_split.zip`中存在标签gt文件， `WIDER_train.zip`和`WIDER_val.zip`具有gt标签，`WIDER_test.zip` 无gt标签。解压`wider_face_split.zip`，得到`wider_face_train_bbx_gt.txt`和`wider_face_val_bbx_gt.txt`

📌 WIDER FACE标注如下：

```
0--Parade/0_Parade_marchingband_1_849.jpg
1
449 330 122 149 0 0 0 0 0 0 
0--Parade/0_Parade_Parade_0_904.jpg
1
361 98 263 339 0 0 0 0 0 0 
0--Parade/0_Parade_marchingband_1_799.jpg
21
78 221 7 8 2 0 0 0 0 0 
78 238 14 17 2 0 0 0 0 0 
113 212 11 15 2 0 0 0 0 0 
134 260 15 15 2 0 0 0 0 0 
163 250 14 17 2 0 0 0 0 0 
201 218 10 12 2 0 0 0 0 0 
182 266 15 17 2 0 0 0 0 0 
245 279 18 15 2 0 0 0 0 0 
304 265 16 17 2 0 0 0 2 1 
328 295 16 20 2 0 0 0 0 0 
389 281 17 19 2 0 0 0 2 0 
406 293 21 21 2 0 1 0 0 0 
436 290 22 17 2 0 0 0 0 0 
522 328 21 18 2 0 1 0 0 0 
643 320 23 22 2 0 0 0 0 0 
653 224 17 25 2 0 0 0 0 0 
793 337 23 30 2 0 0 0 0 0 
535 311 16 17 2 0 0 0 1 0 
29 220 11 15 2 0 0 0 0 0 
3 232 11 15 2 0 0 0 2 0 
20 215 12 16 2 0 0 0 2 0 
```

📌 WIDER FACE包含61个人脸场景

```
[0, 'Parade']
[1, 'Handshaking']
[2, 'Demonstration']
[3, 'Riot']
[4, 'Dancing']
[5, 'Car_Accident']
[6, 'Funeral']
[7, 'Cheering']
[8, 'Election_Campain']
[9, 'Press_Conference']
[10, 'People_Marching']
[11, 'Meeting']
[12, 'Group']
[13, 'Interview']
[14, 'Traffic']
[15, 'Stock_Market']
[16, 'Award_Ceremony']
[17, 'Ceremony']
[18, 'Concerts']
[19, 'Couple']
[20, 'Family_Group']
[21, 'Festival']
[22, 'Picnic']
[23, 'Shoppers']
[24, 'Soldier_Firing']
[25, 'Soldier_Patrol']
[26, 'Soldier_Drilling']
[27, 'Spa']
[28, 'Sports_Fan']
[29, 'Students_Schoolkids']
[30, 'Surgeons']
[31, 'Waiter_Waitress']
[32, 'Worker_Laborer']
[33, 'Running']
[34, 'Baseball']
[35, 'Basketball']
[36, 'Football']
[37, 'Soccer']
[38, 'Tennis']
[39, 'Ice_Skating']
[40, 'Gymnastics']
[41, 'Swimming']
[42, 'Car_Racing']
[43, 'Row_Boat']
[44, 'Aerobics']
[45, 'Balloonist']
[46, 'Jockey']
[47, 'Matador_Bullfighter']
[48, 'Parachutist_Paratrooper']
[49, 'Greeting']
[50, 'Celebration_Or_Party']
[51, 'Dresses']
[52, 'Photographers']
[53, 'Raid']
[54, 'Rescue']
[55, 'Sports_Coach_Trainer']
[56, 'Voter']
[57, 'Angler']
[58, 'Hockey']
[59, 'people']
[61, 'Street_Battle']
```

📌 WIDER FACE数据集原始数据分布

| 数据集名称  | 实例数 | 图片数 | 标记图片数 |
| :---------: | :----: | :----: | :--------: |
| WIDER_train | 159416 | 12880  |   12876    |
|  WIDER_val  | 39706  |  3226  |    3226    |
|    总数     | 199122 | 16106  |   16102    |

❗ 注意：上表中的**实例数**是经过清洗，去除重复实例后的数据

### 💡 YOLOv5训练集和验证集的制作

#### ⚡ 数据清洗

📌 清除重复的实例

```
# 21_Festival_Festival_21_604.jpg
# 219 454 9 15 2 0 0 0 1 0  wider_face_val_bbx_gt.txt
# 219 454 9 15 2 0 0 0 1 0  wider_face_val_bbx_gt.txt

# 2_Demonstration_Protesters_2_231.jpg
# 55 463 24 29 2 0 0 0 1 0  wider_face_train_bbx_gt
# 55 463 24 29 2 0 0 0 1 0  wider_face_train_bbx_gt

# 37_Soccer_Soccer_37_851.jpg
# 695 671 13 9 2 0 0 0 0 0 wider_face_train_bbx_gt
# 695 671 13 9 2 0 0 0 2 0  wider_face_train_bbx_gt

# 7_Cheering_Cheering_7_17.jpg
# 410 314 3 8 0 0 0 1 0 0 wider_face_train_bbx_gt
# 410 314 3 8 0 0 0 1 0 0 wider_face_train_bbx_gt
```

📌 清除边框坐标错误的实例

```
# 39_Ice_Skating_iceskiing_39_583.jpg
# 1026 474 0 23 2 0 0 0 2 0 wider_face_val_bbx_gt.txt

# 54_Rescue_rescuepeople_54_29.jpg
# 1050 142 0 50 2 0 0 0 1 0 wider_face_train_bbx_gt
```

📌 清除无标记图片

```
# wider_face_val_bbx_gt.txt
# 0_Parade_Parade_0_452.jpg
# 2_Demonstration_Political_Rally_2_444.jpg
# 39_Ice_Skating_iceskiing_39_380.jpg
# 46_Jockey_Jockey_46_576.jpg
```



#### ⚡ 训练集和验证集分布

❤️ 将原始数据集清洗后重新分布，如下表：

| 数据集名称 | 实例数 | 图片数 | 实例占比（%） | 图片占比（%） |
| :--------: | :----: | :----: | :-----------: | :-----------: |
|   训练集   | 160758 | 12937  |     80.73     |     80.34     |
|   验证集   | 38364  |  3165  |     19.27     |     19.66     |
|    总数    | 199122 | 16102  |      100      |      100      |



### 💡 人脸检测模型的构建





### 💡 训练结果分析和总结



