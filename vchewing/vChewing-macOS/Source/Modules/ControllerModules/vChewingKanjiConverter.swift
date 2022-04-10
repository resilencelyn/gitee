// Copyright (c) 2021 and onwards The vChewing Project (MIT-NTL License).
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

1. The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

2. No trademark license is granted to use the trade names, trademarks, service
marks, or product names of Contributor, except as required to fulfill notice
requirements above.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

import Cocoa

extension String {
	fileprivate mutating func selfReplace(_ strOf: String, _ strWith: String = "") {
		self = self.replacingOccurrences(of: strOf, with: strWith)
	}
}

@objc class vChewingKanjiConverter: NSObject {
	@objc class func cnvTradToKangXi(_ strObj: String) -> String {
		var strObj = strObj
		strObj.selfReplace("偽", "僞")
		strObj.selfReplace("啟", "啓")
		strObj.selfReplace("吃", "喫")
		strObj.selfReplace("嫻", "嫺")
		strObj.selfReplace("媯", "嬀")
		strObj.selfReplace("峰", "峯")
		strObj.selfReplace("么", "幺")
		strObj.selfReplace("抬", "擡")
		strObj.selfReplace("稜", "棱")
		strObj.selfReplace("簷", "檐")
		strObj.selfReplace("汙", "污")
		strObj.selfReplace("洩", "泄")
		strObj.selfReplace("溈", "潙")
		strObj.selfReplace("潀", "潨")
		strObj.selfReplace("為", "爲")
		strObj.selfReplace("床", "牀")
		strObj.selfReplace("痺", "痹")
		strObj.selfReplace("痴", "癡")
		strObj.selfReplace("皂", "皁")
		strObj.selfReplace("著", "着")
		strObj.selfReplace("睪", "睾")
		strObj.selfReplace("秘", "祕")
		strObj.selfReplace("灶", "竈")
		strObj.selfReplace("粽", "糉")
		strObj.selfReplace("韁", "繮")
		strObj.selfReplace("才", "纔")
		strObj.selfReplace("群", "羣")
		strObj.selfReplace("唇", "脣")
		strObj.selfReplace("參", "蔘")
		strObj.selfReplace("蒍", "蔿")
		strObj.selfReplace("眾", "衆")
		strObj.selfReplace("裡", "裏")
		strObj.selfReplace("核", "覈")
		strObj.selfReplace("踴", "踊")
		strObj.selfReplace("缽", "鉢")
		strObj.selfReplace("針", "鍼")
		strObj.selfReplace("鯰", "鮎")
		strObj.selfReplace("麵", "麪")
		strObj.selfReplace("顎", "齶")
		strObj.selfReplace("口喫", "口吃")
		strObj.selfReplace("合着", "合著")
		strObj.selfReplace("名着", "名著")
		strObj.selfReplace("巨着", "巨著")
		strObj.selfReplace("鉅着", "鉅著")
		strObj.selfReplace("昭着", "昭著")
		strObj.selfReplace("所着", "所著")
		strObj.selfReplace("遺着", "遺著")
		strObj.selfReplace("顯着", "顯著")
		strObj.selfReplace("土着", "土著")
		strObj.selfReplace("着作", "著作")
		strObj.selfReplace("着名", "著名")
		strObj.selfReplace("着式", "著式")
		strObj.selfReplace("着志", "著志")
		strObj.selfReplace("着於", "著於")
		strObj.selfReplace("着書", "著書")
		strObj.selfReplace("着白", "著白")
		strObj.selfReplace("着稱", "著稱")
		strObj.selfReplace("着者", "著者")
		strObj.selfReplace("着述", "著述")
		strObj.selfReplace("着錄", "著錄")
		strObj.selfReplace("蹇喫", "蹇吃")
		strObj.selfReplace("大着", "大著")
		strObj.selfReplace("刊着", "刊著")
		strObj.selfReplace("玄着", "玄著")
		strObj.selfReplace("白着", "白著")
		strObj.selfReplace("住着", "住著")
		strObj.selfReplace("刻着", "刻著")
		strObj.selfReplace("卓着", "卓著")
		strObj.selfReplace("拙着", "拙著")
		strObj.selfReplace("查着", "查著")
		strObj.selfReplace("炳着", "炳著")
		strObj.selfReplace("原着", "原著")
		strObj.selfReplace("專着", "專著")
		strObj.selfReplace("焯着", "焯著")
		strObj.selfReplace("着論", "著論")
		strObj.selfReplace("着績", "著績")
		strObj.selfReplace("較着", "較著")
		strObj.selfReplace("彰着", "彰著")
		strObj.selfReplace("撰着", "撰著")
		strObj.selfReplace("編着", "編著")
		strObj.selfReplace("論着", "論著")
		strObj.selfReplace("雜着", "雜著")
		strObj.selfReplace("譯着", "譯著")
		strObj.selfReplace("地覈", "地核")
		strObj.selfReplace("多覈", "多核")
		strObj.selfReplace("氘覈", "氘核")
		strObj.selfReplace("杏覈", "杏核")
		strObj.selfReplace("非覈", "非核")
		strObj.selfReplace("覈三", "核三")
		strObj.selfReplace("覈下", "核下")
		strObj.selfReplace("覈災", "核災")
		strObj.selfReplace("覈武", "核武")
		strObj.selfReplace("覈狀", "核狀")
		strObj.selfReplace("覈桃", "核桃")
		strObj.selfReplace("覈彈", "核彈")
		strObj.selfReplace("覈戰", "核戰")
		strObj.selfReplace("覈糖", "核糖")
		strObj.selfReplace("覈醣", "核醣")
		strObj.selfReplace("晶覈", "晶核")
		strObj.selfReplace("熱覈", "熱核")
		strObj.selfReplace("反覈", "反核")
		strObj.selfReplace("卵覈", "卵核")
		strObj.selfReplace("果覈", "果核")
		strObj.selfReplace("剋覈", "剋核")
		strObj.selfReplace("覈力", "核力")
		strObj.selfReplace("覈子", "核子")
		strObj.selfReplace("覈仁", "核仁")
		strObj.selfReplace("覈心", "核心")
		strObj.selfReplace("覈四", "核四")
		strObj.selfReplace("覈果", "核果")
		strObj.selfReplace("覈型", "核型")
		strObj.selfReplace("覈苷", "核苷")
		strObj.selfReplace("覈能", "核能")
		strObj.selfReplace("覈傘", "核傘")
		strObj.selfReplace("覈發", "核發")
		strObj.selfReplace("覈電", "核電")
		strObj.selfReplace("覈塵", "核塵")
		strObj.selfReplace("覈酸", "核酸")
		strObj.selfReplace("覈膜", "核膜")
		strObj.selfReplace("覈爆", "核爆")
		strObj.selfReplace("痔覈", "痔核")
		strObj.selfReplace("陰覈", "陰核")
		strObj.selfReplace("殽覈", "殽核")
		strObj.selfReplace("結覈", "結核")
		strObj.selfReplace("菌覈", "菌核")
		strObj.selfReplace("煤覈", "煤核")
		strObj.selfReplace("着涎茶", "著涎茶")
		strObj.selfReplace("喫口令", "吃口令")
		strObj.selfReplace("鄧艾喫", "鄧艾吃")
		strObj.selfReplace("杏仁覈", "杏仁核")
		strObj.selfReplace("覈一廠", "核一廠")
		strObj.selfReplace("覈二廠", "核二廠")
		strObj.selfReplace("覈三廠", "核三廠")
		strObj.selfReplace("覈融合", "核融合")
		strObj.selfReplace("覈四廠", "核四廠")
		strObj.selfReplace("覈生化", "核生化")
		strObj.selfReplace("覈災變", "核災變")
		strObj.selfReplace("覈動力", "核動力")
		strObj.selfReplace("覈試爆", "核試爆")
		strObj.selfReplace("杏覈兒", "杏核兒")
		strObj.selfReplace("原子覈", "原子核")
		strObj.selfReplace("覈分裂", "核分裂")
		strObj.selfReplace("覈化學", "核化學")
		strObj.selfReplace("覈反應", "核反應")
		strObj.selfReplace("覈半徑", "核半徑")
		strObj.selfReplace("覈污染", "核污染")
		strObj.selfReplace("覈武器", "核武器")
		strObj.selfReplace("覈苷酸", "核苷酸")
		strObj.selfReplace("覈蛋白", "核蛋白")
		strObj.selfReplace("覈黃疸", "核黃疸")
		strObj.selfReplace("覈黃素", "核黃素")
		strObj.selfReplace("覈裝置", "核裝置")
		strObj.selfReplace("覈電廠", "核電廠")
		strObj.selfReplace("覈廢料", "核廢料")
		strObj.selfReplace("覈彈頭", "核彈頭")
		strObj.selfReplace("覈潛艇", "核潛艇")
		strObj.selfReplace("覈燃料", "核燃料")
		strObj.selfReplace("桃覈雕", "桃核雕")
		strObj.selfReplace("細胞覈", "細胞核")
		strObj.selfReplace("棗覈臉", "棗核臉")
		strObj.selfReplace("以微知着", "以微知著")
		strObj.selfReplace("見微知着", "見微知著")
		strObj.selfReplace("恩威並着", "恩威並著")
		strObj.selfReplace("視微知着", "視微知著")
		strObj.selfReplace("睹微知着", "睹微知著")
		strObj.selfReplace("遐邇着聞", "遐邇著聞")
		strObj.selfReplace("積微成着", "積微成著")
		strObj.selfReplace("地下覈試", "地下核試")
		strObj.selfReplace("地下覈爆", "地下核爆")
		strObj.selfReplace("非覈武區", "非核武區")
		strObj.selfReplace("覈反應器", "核反應器")
		strObj.selfReplace("覈物理學", "核物理學")
		strObj.selfReplace("覈能發電", "核能發電")
		strObj.selfReplace("覈能電廠", "核能電廠")
		strObj.selfReplace("覈能廢料", "核能廢料")
		strObj.selfReplace("覈能潛艇", "核能潛艇")
		strObj.selfReplace("覈磁共振", "核磁共振")
		strObj.selfReplace("熱覈反應", "熱核反應")
		strObj.selfReplace("賣李鑽覈", "賣李鑽核")
		strObj.selfReplace("雙覈都市", "雙核都市")
		strObj.selfReplace("罵人不吐覈", "罵人不吐核")
		return strObj
	}

	@objc class func cnvTradToJIS(_ strObj: String) -> String {
		// 該轉換是由康熙繁體轉換至日語當用漢字的，所以需要先跑一遍康熙轉換。
		var strObj = cnvTradToKangXi(strObj)
		strObj.selfReplace("兩", "両")
		strObj.selfReplace("輛", "両")
		strObj.selfReplace("辨", "弁")
		strObj.selfReplace("辯", "弁")
		strObj.selfReplace("瓣", "弁")
		strObj.selfReplace("辦", "弁")
		strObj.selfReplace("禦", "御")
		strObj.selfReplace("缺", "欠")
		strObj.selfReplace("絲", "糸")
		strObj.selfReplace("藝", "芸")
		strObj.selfReplace("濱", "浜")
		strObj.selfReplace("乘", "乗")
		strObj.selfReplace("亂", "乱")
		strObj.selfReplace("亙", "亘")
		strObj.selfReplace("亞", "亜")
		strObj.selfReplace("佛", "仏")
		strObj.selfReplace("來", "来")
		strObj.selfReplace("假", "仮")
		strObj.selfReplace("傳", "伝")
		strObj.selfReplace("僞", "偽")
		strObj.selfReplace("價", "価")
		strObj.selfReplace("儉", "倹")
		strObj.selfReplace("兒", "児")
		strObj.selfReplace("內", "内")
		strObj.selfReplace("剎", "刹")
		strObj.selfReplace("剩", "剰")
		strObj.selfReplace("劍", "剣")
		strObj.selfReplace("剱", "剣")
		strObj.selfReplace("劎", "剣")
		strObj.selfReplace("劒", "剣")
		strObj.selfReplace("劔", "剣")
		strObj.selfReplace("劑", "剤")
		strObj.selfReplace("勞", "労")
		strObj.selfReplace("勳", "勲")
		strObj.selfReplace("勵", "励")
		strObj.selfReplace("勸", "勧")
		strObj.selfReplace("勻", "匀")
		strObj.selfReplace("區", "区")
		strObj.selfReplace("卷", "巻")
		strObj.selfReplace("卻", "却")
		strObj.selfReplace("參", "参")
		strObj.selfReplace("吳", "呉")
		strObj.selfReplace("咒", "呪")
		strObj.selfReplace("啞", "唖")
		strObj.selfReplace("單", "単")
		strObj.selfReplace("噓", "嘘")
		strObj.selfReplace("嚙", "噛")
		strObj.selfReplace("嚴", "厳")
		strObj.selfReplace("囑", "嘱")
		strObj.selfReplace("圈", "圏")
		strObj.selfReplace("國", "国")
		strObj.selfReplace("圍", "囲")
		strObj.selfReplace("圓", "円")
		strObj.selfReplace("圖", "図")
		strObj.selfReplace("團", "団")
		strObj.selfReplace("增", "増")
		strObj.selfReplace("墮", "堕")
		strObj.selfReplace("壓", "圧")
		strObj.selfReplace("壘", "塁")
		strObj.selfReplace("壞", "壊")
		strObj.selfReplace("壤", "壌")
		strObj.selfReplace("壯", "壮")
		strObj.selfReplace("壹", "壱")
		strObj.selfReplace("壽", "寿")
		strObj.selfReplace("奧", "奥")
		strObj.selfReplace("奬", "奨")
		strObj.selfReplace("妝", "粧")
		strObj.selfReplace("孃", "嬢")
		strObj.selfReplace("學", "学")
		strObj.selfReplace("寢", "寝")
		strObj.selfReplace("實", "実")
		strObj.selfReplace("寫", "写")
		strObj.selfReplace("寬", "寛")
		strObj.selfReplace("寶", "宝")
		strObj.selfReplace("將", "将")
		strObj.selfReplace("專", "専")
		strObj.selfReplace("對", "対")
		strObj.selfReplace("屆", "届")
		strObj.selfReplace("屬", "属")
		strObj.selfReplace("峯", "峰")
		strObj.selfReplace("峽", "峡")
		strObj.selfReplace("嶽", "岳")
		strObj.selfReplace("巖", "巌")
		strObj.selfReplace("巢", "巣")
		strObj.selfReplace("帶", "帯")
		strObj.selfReplace("廁", "厠")
		strObj.selfReplace("廢", "廃")
		strObj.selfReplace("廣", "広")
		strObj.selfReplace("廳", "庁")
		strObj.selfReplace("彈", "弾")
		strObj.selfReplace("彌", "弥")
		strObj.selfReplace("彎", "弯")
		strObj.selfReplace("彥", "彦")
		strObj.selfReplace("徑", "径")
		strObj.selfReplace("從", "従")
		strObj.selfReplace("徵", "徴")
		strObj.selfReplace("德", "徳")
		strObj.selfReplace("恆", "恒")
		strObj.selfReplace("悅", "悦")
		strObj.selfReplace("惠", "恵")
		strObj.selfReplace("惡", "悪")
		strObj.selfReplace("惱", "悩")
		strObj.selfReplace("慘", "惨")
		strObj.selfReplace("應", "応")
		strObj.selfReplace("懷", "懐")
		strObj.selfReplace("戀", "恋")
		strObj.selfReplace("戰", "戦")
		strObj.selfReplace("戲", "戯")
		strObj.selfReplace("戶", "戸")
		strObj.selfReplace("戾", "戻")
		strObj.selfReplace("拂", "払")
		strObj.selfReplace("拔", "抜")
		strObj.selfReplace("拜", "拝")
		strObj.selfReplace("挾", "挟")
		strObj.selfReplace("插", "挿")
		strObj.selfReplace("揭", "掲")
		strObj.selfReplace("搔", "掻")
		strObj.selfReplace("搖", "揺")
		strObj.selfReplace("搜", "捜")
		strObj.selfReplace("摑", "掴")
		strObj.selfReplace("擇", "択")
		strObj.selfReplace("擊", "撃")
		strObj.selfReplace("擔", "担")
		strObj.selfReplace("據", "拠")
		strObj.selfReplace("擴", "拡")
		strObj.selfReplace("攝", "摂")
		strObj.selfReplace("攪", "撹")
		strObj.selfReplace("收", "収")
		strObj.selfReplace("效", "効")
		strObj.selfReplace("敕", "勅")
		strObj.selfReplace("敘", "叙")
		strObj.selfReplace("數", "数")
		strObj.selfReplace("斷", "断")
		strObj.selfReplace("晉", "晋")
		strObj.selfReplace("晚", "晩")
		strObj.selfReplace("晝", "昼")
		strObj.selfReplace("暨", "曁")
		strObj.selfReplace("曆", "暦")
		strObj.selfReplace("曉", "暁")
		strObj.selfReplace("曾", "曽")
		strObj.selfReplace("會", "会")
		strObj.selfReplace("枡", "桝")
		strObj.selfReplace("查", "査")
		strObj.selfReplace("條", "条")
		strObj.selfReplace("棧", "桟")
		strObj.selfReplace("棱", "稜")
		strObj.selfReplace("榆", "楡")
		strObj.selfReplace("榮", "栄")
		strObj.selfReplace("樂", "楽")
		strObj.selfReplace("樓", "楼")
		strObj.selfReplace("樞", "枢")
		strObj.selfReplace("樣", "様")
		strObj.selfReplace("橫", "横")
		strObj.selfReplace("檢", "検")
		strObj.selfReplace("櫻", "桜")
		strObj.selfReplace("權", "権")
		strObj.selfReplace("歐", "欧")
		strObj.selfReplace("歡", "歓")
		strObj.selfReplace("步", "歩")
		strObj.selfReplace("歲", "歳")
		strObj.selfReplace("歷", "歴")
		strObj.selfReplace("歸", "帰")
		strObj.selfReplace("殘", "残")
		strObj.selfReplace("殼", "殻")
		strObj.selfReplace("毆", "殴")
		strObj.selfReplace("每", "毎")
		strObj.selfReplace("氣", "気")
		strObj.selfReplace("污", "汚")
		strObj.selfReplace("沒", "没")
		strObj.selfReplace("涉", "渉")
		strObj.selfReplace("淚", "涙")
		strObj.selfReplace("淨", "浄")
		strObj.selfReplace("淺", "浅")
		strObj.selfReplace("渴", "渇")
		strObj.selfReplace("溌", "潑")
		strObj.selfReplace("溪", "渓")
		strObj.selfReplace("溫", "温")
		strObj.selfReplace("溼", "湿")
		strObj.selfReplace("滯", "滞")
		strObj.selfReplace("滿", "満")
		strObj.selfReplace("潛", "潜")
		strObj.selfReplace("澀", "渋")
		strObj.selfReplace("澤", "沢")
		strObj.selfReplace("濟", "済")
		strObj.selfReplace("濤", "涛")
		strObj.selfReplace("濾", "沪")
		strObj.selfReplace("瀧", "滝")
		strObj.selfReplace("瀨", "瀬")
		strObj.selfReplace("灣", "湾")
		strObj.selfReplace("焰", "焔")
		strObj.selfReplace("燈", "灯")
		strObj.selfReplace("燒", "焼")
		strObj.selfReplace("營", "営")
		strObj.selfReplace("爐", "炉")
		strObj.selfReplace("爭", "争")
		strObj.selfReplace("爲", "為")
		strObj.selfReplace("牀", "床")
		strObj.selfReplace("犧", "犠")
		strObj.selfReplace("狀", "状")
		strObj.selfReplace("狹", "狭")
		strObj.selfReplace("獨", "独")
		strObj.selfReplace("獵", "猟")
		strObj.selfReplace("獸", "獣")
		strObj.selfReplace("獻", "献")
		strObj.selfReplace("產", "産")
		strObj.selfReplace("畫", "画")
		strObj.selfReplace("當", "当")
		strObj.selfReplace("疊", "畳")
		strObj.selfReplace("疎", "疏")
		strObj.selfReplace("痹", "痺")
		strObj.selfReplace("瘦", "痩")
		strObj.selfReplace("癡", "痴")
		strObj.selfReplace("發", "発")
		strObj.selfReplace("皋", "皐")
		strObj.selfReplace("盜", "盗")
		strObj.selfReplace("盡", "尽")
		strObj.selfReplace("碎", "砕")
		strObj.selfReplace("祕", "秘")
		strObj.selfReplace("祿", "禄")
		strObj.selfReplace("禪", "禅")
		strObj.selfReplace("禮", "礼")
		strObj.selfReplace("禱", "祷")
		strObj.selfReplace("稅", "税")
		strObj.selfReplace("稱", "称")
		strObj.selfReplace("稻", "稲")
		strObj.selfReplace("穎", "頴")
		strObj.selfReplace("穗", "穂")
		strObj.selfReplace("穩", "穏")
		strObj.selfReplace("穰", "穣")
		strObj.selfReplace("竃", "竈")
		strObj.selfReplace("竊", "窃")
		strObj.selfReplace("粹", "粋")
		strObj.selfReplace("糉", "粽")
		strObj.selfReplace("絕", "絶")
		strObj.selfReplace("經", "経")
		strObj.selfReplace("綠", "緑")
		strObj.selfReplace("緖", "緒")
		strObj.selfReplace("緣", "縁")
		strObj.selfReplace("縣", "県")
		strObj.selfReplace("縱", "縦")
		strObj.selfReplace("總", "総")
		strObj.selfReplace("繋", "繫")
		strObj.selfReplace("繡", "繍")
		strObj.selfReplace("繩", "縄")
		strObj.selfReplace("繪", "絵")
		strObj.selfReplace("繼", "継")
		strObj.selfReplace("續", "続")
		strObj.selfReplace("纔", "才")
		strObj.selfReplace("纖", "繊")
		strObj.selfReplace("罐", "缶")
		strObj.selfReplace("羣", "群")
		strObj.selfReplace("聯", "連")
		strObj.selfReplace("聰", "聡")
		strObj.selfReplace("聲", "声")
		strObj.selfReplace("聽", "聴")
		strObj.selfReplace("肅", "粛")
		strObj.selfReplace("脣", "唇")
		strObj.selfReplace("脫", "脱")
		strObj.selfReplace("腦", "脳")
		strObj.selfReplace("腳", "脚")
		strObj.selfReplace("膽", "胆")
		strObj.selfReplace("臟", "臓")
		strObj.selfReplace("臺", "台")
		strObj.selfReplace("與", "与")
		strObj.selfReplace("舉", "挙")
		strObj.selfReplace("舊", "旧")
		strObj.selfReplace("舍", "舎")
		strObj.selfReplace("荔", "茘")
		strObj.selfReplace("莊", "荘")
		strObj.selfReplace("莖", "茎")
		strObj.selfReplace("菸", "煙")
		strObj.selfReplace("萊", "莱")
		strObj.selfReplace("萬", "万")
		strObj.selfReplace("蔣", "蒋")
		strObj.selfReplace("蔥", "葱")
		strObj.selfReplace("薰", "薫")
		strObj.selfReplace("藏", "蔵")
		strObj.selfReplace("藥", "薬")
		strObj.selfReplace("蘆", "芦")
		strObj.selfReplace("處", "処")
		strObj.selfReplace("虛", "虚")
		strObj.selfReplace("號", "号")
		strObj.selfReplace("螢", "蛍")
		strObj.selfReplace("蟲", "虫")
		strObj.selfReplace("蠟", "蝋")
		strObj.selfReplace("蠶", "蚕")
		strObj.selfReplace("蠻", "蛮")
		strObj.selfReplace("裝", "装")
		strObj.selfReplace("覺", "覚")
		strObj.selfReplace("覽", "覧")
		strObj.selfReplace("觀", "観")
		strObj.selfReplace("觸", "触")
		strObj.selfReplace("說", "説")
		strObj.selfReplace("謠", "謡")
		strObj.selfReplace("證", "証")
		strObj.selfReplace("譯", "訳")
		strObj.selfReplace("譽", "誉")
		strObj.selfReplace("讀", "読")
		strObj.selfReplace("變", "変")
		strObj.selfReplace("讓", "譲")
		strObj.selfReplace("豐", "豊")
		strObj.selfReplace("豫", "予")
		strObj.selfReplace("貓", "猫")
		strObj.selfReplace("貳", "弐")
		strObj.selfReplace("賣", "売")
		strObj.selfReplace("賴", "頼")
		strObj.selfReplace("贊", "賛")
		strObj.selfReplace("贗", "贋")
		strObj.selfReplace("踐", "践")
		strObj.selfReplace("輕", "軽")
		strObj.selfReplace("輛", "輌")
		strObj.selfReplace("轉", "転")
		strObj.selfReplace("辭", "辞")
		strObj.selfReplace("遞", "逓")
		strObj.selfReplace("遥", "遙")
		strObj.selfReplace("遲", "遅")
		strObj.selfReplace("邊", "辺")
		strObj.selfReplace("鄉", "郷")
		strObj.selfReplace("酢", "醋")
		strObj.selfReplace("醉", "酔")
		strObj.selfReplace("醗", "醱")
		strObj.selfReplace("醫", "医")
		strObj.selfReplace("醬", "醤")
		strObj.selfReplace("釀", "醸")
		strObj.selfReplace("釋", "釈")
		strObj.selfReplace("鋪", "舗")
		strObj.selfReplace("錄", "録")
		strObj.selfReplace("錢", "銭")
		strObj.selfReplace("鍊", "錬")
		strObj.selfReplace("鐵", "鉄")
		strObj.selfReplace("鑄", "鋳")
		strObj.selfReplace("鑛", "鉱")
		strObj.selfReplace("閱", "閲")
		strObj.selfReplace("關", "関")
		strObj.selfReplace("陷", "陥")
		strObj.selfReplace("隨", "随")
		strObj.selfReplace("險", "険")
		strObj.selfReplace("隱", "隠")
		strObj.selfReplace("雙", "双")
		strObj.selfReplace("雜", "雑")
		strObj.selfReplace("雞", "鶏")
		strObj.selfReplace("霸", "覇")
		strObj.selfReplace("靈", "霊")
		strObj.selfReplace("靜", "静")
		strObj.selfReplace("顏", "顔")
		strObj.selfReplace("顯", "顕")
		strObj.selfReplace("餘", "余")
		strObj.selfReplace("騷", "騒")
		strObj.selfReplace("驅", "駆")
		strObj.selfReplace("驗", "験")
		strObj.selfReplace("驛", "駅")
		strObj.selfReplace("髓", "髄")
		strObj.selfReplace("體", "体")
		strObj.selfReplace("髮", "髪")
		strObj.selfReplace("鬥", "闘")
		strObj.selfReplace("鱉", "鼈")
		strObj.selfReplace("鷗", "鴎")
		strObj.selfReplace("鹼", "鹸")
		strObj.selfReplace("鹽", "塩")
		strObj.selfReplace("麥", "麦")
		strObj.selfReplace("麪", "麺")
		strObj.selfReplace("麴", "麹")
		strObj.selfReplace("黃", "黄")
		strObj.selfReplace("黑", "黒")
		strObj.selfReplace("默", "黙")
		strObj.selfReplace("點", "点")
		strObj.selfReplace("黨", "党")
		strObj.selfReplace("齊", "斉")
		strObj.selfReplace("齋", "斎")
		strObj.selfReplace("齒", "歯")
		strObj.selfReplace("齡", "齢")
		strObj.selfReplace("龍", "竜")
		strObj.selfReplace("龜", "亀")
		strObj.selfReplace("叮嚀", "丁寧")
		strObj.selfReplace("鄭重", "丁重")
		strObj.selfReplace("輿論", "世論")
		strObj.selfReplace("唖鈴", "亜鈴")
		strObj.selfReplace("交叉", "交差")
		strObj.selfReplace("饗宴", "供宴")
		strObj.selfReplace("駿馬", "俊馬")
		strObj.selfReplace("堡塁", "保塁")
		strObj.selfReplace("扁平", "偏平")
		strObj.selfReplace("碇泊", "停泊")
		strObj.selfReplace("優駿", "優俊")
		strObj.selfReplace("尖兵", "先兵")
		strObj.selfReplace("尖鋭", "先鋭")
		strObj.selfReplace("共軛", "共役")
		strObj.selfReplace("饒舌", "冗舌")
		strObj.selfReplace("兇器", "凶器")
		strObj.selfReplace("鑿岩", "削岩")
		strObj.selfReplace("庖丁", "包丁")
		strObj.selfReplace("繃帯", "包帯")
		strObj.selfReplace("区劃", "区画")
		strObj.selfReplace("儼然", "厳然")
		strObj.selfReplace("友誼", "友宜")
		strObj.selfReplace("叛乱", "反乱")
		strObj.selfReplace("蒐集", "収集")
		strObj.selfReplace("抒情", "叙情")
		strObj.selfReplace("擡頭", "台頭")
		strObj.selfReplace("合弁", "合弁")
		strObj.selfReplace("歎願", "嘆願")
		strObj.selfReplace("廻転", "回転")
		strObj.selfReplace("回游", "回遊")
		strObj.selfReplace("捧持", "奉持")
		strObj.selfReplace("萎縮", "委縮")
		strObj.selfReplace("輾転", "展転")
		strObj.selfReplace("稀少", "希少")
		strObj.selfReplace("眩惑", "幻惑")
		strObj.selfReplace("広汎", "広範")
		strObj.selfReplace("曠野", "広野")
		strObj.selfReplace("廃墟", "廃虚")
		strObj.selfReplace("弁当", "弁当")
		strObj.selfReplace("弁膜", "弁膜")
		strObj.selfReplace("弁護", "弁護")
		strObj.selfReplace("辮髪", "弁髪")
		strObj.selfReplace("絃歌", "弦歌")
		strObj.selfReplace("恩誼", "恩義")
		strObj.selfReplace("意嚮", "意向")
		strObj.selfReplace("臆断", "憶断")
		strObj.selfReplace("臆病", "憶病")
		strObj.selfReplace("戦歿", "戦没")
		strObj.selfReplace("煽情", "扇情")
		strObj.selfReplace("手帖", "手帳")
		strObj.selfReplace("伎倆", "技量")
		strObj.selfReplace("抜萃", "抜粋")
		strObj.selfReplace("披瀝", "披歴")
		strObj.selfReplace("牴触", "抵触")
		strObj.selfReplace("抽籤", "抽選")
		strObj.selfReplace("勾引", "拘引")
		strObj.selfReplace("醵出", "拠出")
		strObj.selfReplace("醵金", "拠金")
		strObj.selfReplace("掘鑿", "掘削")
		strObj.selfReplace("扣除", "控除")
		strObj.selfReplace("掩護", "援護")
		strObj.selfReplace("抛棄", "放棄")
		strObj.selfReplace("撒水", "散水")
		strObj.selfReplace("敬虔", "敬謙")
		strObj.selfReplace("敷衍", "敷延")
		strObj.selfReplace("断乎", "断固")
		strObj.selfReplace("簇生", "族生")
		strObj.selfReplace("陞叙", "昇叙")
		strObj.selfReplace("煖房", "暖房")
		strObj.selfReplace("暗誦", "暗唱")
		strObj.selfReplace("闇夜", "暗夜")
		strObj.selfReplace("曝露", "暴露")
		strObj.selfReplace("涸渇", "枯渇")
		strObj.selfReplace("恰好", "格好")
		strObj.selfReplace("恰幅", "格幅")
		strObj.selfReplace("毀損", "棄損")
		strObj.selfReplace("摸索", "模索")
		strObj.selfReplace("欠欠", "欠缺")
		strObj.selfReplace("屍体", "死体")
		strObj.selfReplace("臀部", "殿部")
		strObj.selfReplace("拇指", "母指")
		strObj.selfReplace("気魄", "気迫")
		strObj.selfReplace("訣別", "決別")
		strObj.selfReplace("決潰", "決壊")
		strObj.selfReplace("沈澱", "沈殿")
		strObj.selfReplace("波瀾", "波乱")
		strObj.selfReplace("註釈", "注釈")
		strObj.selfReplace("洗滌", "洗濯")
		strObj.selfReplace("活潑", "活発")
		strObj.selfReplace("滲透", "浸透")
		strObj.selfReplace("浸蝕", "浸食")
		strObj.selfReplace("銷却", "消却")
		strObj.selfReplace("渾然", "混然")
		strObj.selfReplace("弯曲", "湾曲")
		strObj.selfReplace("熔接", "溶接")
		strObj.selfReplace("漁撈", "漁労")
		strObj.selfReplace("飄然", "漂然")
		strObj.selfReplace("激昂", "激高")
		strObj.selfReplace("火焔", "火炎")
		strObj.selfReplace("焦躁", "焦燥")
		strObj.selfReplace("斑点", "班点")
		strObj.selfReplace("溜飲", "留飲")
		strObj.selfReplace("掠奪", "略奪")
		strObj.selfReplace("疏通", "疎通")
		strObj.selfReplace("醱酵", "発酵")
		strObj.selfReplace("白堊", "白亜")
		strObj.selfReplace("相剋", "相克")
		strObj.selfReplace("智慧", "知恵")
		strObj.selfReplace("破毀", "破棄")
		strObj.selfReplace("確乎", "確固")
		strObj.selfReplace("禁錮", "禁固")
		strObj.selfReplace("符牒", "符丁")
		strObj.selfReplace("扮装", "粉装")
		strObj.selfReplace("紫斑", "紫班")
		strObj.selfReplace("終熄", "終息")
		strObj.selfReplace("綜合", "総合")
		strObj.selfReplace("編輯", "編集")
		strObj.selfReplace("義捐", "義援")
		strObj.selfReplace("肝腎", "肝心")
		strObj.selfReplace("悖徳", "背徳")
		strObj.selfReplace("脈搏", "脈拍")
		strObj.selfReplace("膨脹", "膨張")
		strObj.selfReplace("芳醇", "芳純")
		strObj.selfReplace("叡智", "英知")
		strObj.selfReplace("蒸溜", "蒸留")
		strObj.selfReplace("燻蒸", "薫蒸")
		strObj.selfReplace("燻製", "薫製")
		strObj.selfReplace("衣裳", "衣装")
		strObj.selfReplace("衰頽", "衰退")
		strObj.selfReplace("悠然", "裕然")
		strObj.selfReplace("輔佐", "補佐")
		strObj.selfReplace("訓誡", "訓戒")
		strObj.selfReplace("試煉", "試練")
		strObj.selfReplace("詭弁", "詭弁")
		strObj.selfReplace("媾和", "講和")
		strObj.selfReplace("象嵌", "象眼")
		strObj.selfReplace("貫禄", "貫録")
		strObj.selfReplace("買弁", "買弁")
		strObj.selfReplace("讚辞", "賛辞")
		strObj.selfReplace("蹈襲", "踏襲")
		strObj.selfReplace("車両", "車両")
		strObj.selfReplace("顛倒", "転倒")
		strObj.selfReplace("輪廓", "輪郭")
		strObj.selfReplace("褪色", "退色")
		strObj.selfReplace("杜絶", "途絶")
		strObj.selfReplace("連繫", "連係")
		strObj.selfReplace("連合", "連合")
		strObj.selfReplace("銓衡", "選考")
		strObj.selfReplace("醋酸", "酢酸")
		strObj.selfReplace("野鄙", "野卑")
		strObj.selfReplace("礦石", "鉱石")
		strObj.selfReplace("間歇", "間欠")
		strObj.selfReplace("函数", "関数")
		strObj.selfReplace("防御", "防御")
		strObj.selfReplace("嶮岨", "険阻")
		strObj.selfReplace("牆壁", "障壁")
		strObj.selfReplace("障礙", "障害")
		strObj.selfReplace("湮滅", "隠滅")
		strObj.selfReplace("聚落", "集落")
		strObj.selfReplace("雇傭", "雇用")
		strObj.selfReplace("諷喩", "風諭")
		strObj.selfReplace("蜚語", "飛語")
		strObj.selfReplace("香奠", "香典")
		strObj.selfReplace("骨骼", "骨格")
		strObj.selfReplace("亢進", "高進")
		strObj.selfReplace("鳥瞰", "鳥観")
		strObj.selfReplace("一攫", "一獲")
		strObj.selfReplace("肩胛", "肩甲")
		strObj.selfReplace("箇条", "個条")
		strObj.selfReplace("啓動", "起動")
		strObj.selfReplace("三叉路", "三差路")
		strObj.selfReplace("嬉遊曲", "喜遊曲")
		strObj.selfReplace("建蔽率", "建坪率")
		strObj.selfReplace("慰藉料", "慰謝料")
		strObj.selfReplace("橋頭堡", "橋頭保")
		strObj.selfReplace("油槽船", "油送船")
		strObj.selfReplace("耕耘機", "耕運機")
		return strObj
	}
}