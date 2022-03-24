package com.jd.platform.jlog.core;

import com.alibaba.fastjson.JSON;
import com.jd.platform.jlog.common.handler.CollectMode;
import com.jd.platform.jlog.common.handler.JcProperties;
import com.jd.platform.jlog.common.handler.TagConfig;
import com.jd.platform.jlog.common.utils.ZstdUtils;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.Arrays;
import java.util.Base64;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName FileNode.java
 * @createTime 2022年03月08日 16:32:00
 */
public final class FileNode {

    private String fillPath;

    private long lastModity;

    public static void main(String[] args) throws IOException {

        String a = "汉字";
        System.out.println("a byte length："+a.getBytes().length);

        byte[] bt1 = ZstdUtils.compress(a.getBytes());
        String ckStr = new String(bt1);
        String newStr1 = ZstdUtils.decompress(bt1);
        System.out.println("老 字符串 bt："+ Arrays.toString(a.getBytes()));
        System.out.println("新 字符串 bt："+ Arrays.toString(ckStr.getBytes()));


        System.out.println("新字符串："+newStr1);
        System.out.println("新字符串的byte长度："+newStr1.getBytes().length);

        String str = "这是实打实分身乏术的故事大哥大哥大哥算法算法发生大发DVD放v都是这是实打实分身乏术的故事大哥大哥大哥算法算法发生大发DVD放v都是这是实打实分身乏术的故事大哥大哥大哥算法算法发生大发DVD放v都是这是实打实分身乏术的故事大哥大哥大哥算法算法发生大发DVD放v都是这是实打实分身乏术的故事大哥大哥大哥算法算法发生大发DVD放v都是这是实打实分身乏术的故事大哥大哥大哥算法算法发生大发DVD放v都是这是实打实分身乏术的故事大哥大哥大哥算法算法发生大发DVD放v都是这是实打实分身乏术的故事大哥大哥大哥算法算法发生大发DVD放v都是这是实打实分身乏术的故事大哥大哥大哥算法算法发生大发DVD放v都是这是实打实分身乏术的故事大哥大哥大哥算法算法发生大发DVD放v都是这是实打实分身乏术的故事大哥大哥大哥算法算法发生大发DVD放v都是这是实打实分身乏术的故事大哥大哥大哥算法算法发生大发DVD放v都是"+"{\"@level\":\"info\",\"@message\":\"response json: \",\"@module\":\"testing-platform.cfeature_plugin\",\"@timestamp\":\"2022-03-10T19:37:55.181928+08:00\",这是实打实分身乏术的故事大哥大哥大哥算法算法发生大发DVD放v都是这是实打实分身乏术的故事大哥大哥大哥算法算法发生大发DVD放v都是这是实打实分身乏术的故事大哥大哥大哥算法算法发生大发DVD放v都是" +
                "\"EXTRA_VALUE_AT_END\":{\"data\":{\"accountAddr\":\"bg\",\"accountArea\":\"\",\"accountBank\":\"012\",\"accountBankBranch\":\"\",\"accountBankBranchNo\":\"\",\"accountBankName\":\"BBVA BANCOMER SA\",\"accountBranch\":\"\",\"accountCardDigit\":\"\",\"accountCardType\":null,\"accountCardno\":\"012420015684144165\",\"accountCity\":\"\",\"accountCompany\":\"\",\"accountFirstName\":\"\",\"accountGrantPhoto\":{\"keys\":[\"do1_tQxmThhy446Wf8JLHK5E\"],\"urls\":[\"http://img1.didiglobal.com/static/sailing_private_b/do1_tQxmThhy446Wf8JLHK5E?expire=1647517074\\u0026signiture=iTo-tiI21Ysdn7n7Y6y2YGs36avRi0cfLUpRl0rlE-g=\"]},\"accountHolder\":\"test_accountHolder\",\"accountHolderId\":\"\",\"accountLastName\":null,\"accountType\":0,\"addr\":\"test_addr\",\"addrAlph\":\"\",\"applyId\":\"5764607808352618589\",\"applyStatus\":0,\"applyType\":1,\"areaName\":\"Area0\",\"arrBusinessAttr\":[\"140\"],\"arrTags\":[],\"authorizationInfo\":{\"payment\":{\"authorizedPics\":null," +
                "\"extraFiles\":null,\"pics\":null},\"sign\":{\"authorizedPics\":null,\"extraFiles\":null,\"pics\":null}},\"autoSwitch\":\"0\",\"avgPrice\":{\"max\":0,\"min\":0},\"avgProduceTime\":600,\"bLicenseAddr\":\"\",\"bLicenseBrand\":\"\",\"bLicenseExpireFlag\":61,\"bLicenseExpireTime\":\"1970-01-01 08:00:00\",\"bLicenseId\":\"\",\"bLicenseLpName\":\"\",\"bLicensePic\":\"\",\"bLicensePicUrl\":\"\",\"bLicenseStartTime\":\"1970-01-01 08:00:00\",\"bLicenseStatus\":\"3\",\"bLicenseValidType\":0,\"bankAgentDigit\":\"\",\"bankAgentno\":\"\",\"bdName\":\"sim_data_01(simdata01_test_v)\",\"bdPhone\":\"\",\"bizDayTime\":null,\"bizDayTimeNonAffiliate\":\"\",\"bizStatus\":1,\"bizTime\":\"{\\\"day\\\":[1,1,1,1,1,1,1],\\\"time\\\":[{\\\"begin\\\":\\\"10:00\\\",\\\"end\\\":\\\"18:00\\\"}]}\",\"bizoppDocInfo\":{\"city\":{\"files\":null,\"licensePics\":null},\"corp\":{\"files\":null,\"licensePics\":null},\"health\":{\"files\":null,\"licensePics\":null},\"land\":{\"files\":null,\"" +
                "licensePics\":null}},\"bizoppInfo\":{\"bussinessHours\":\"\",\"deliveryDistanceTime\":\"\",\"deliveryPrice\":0,\"itemCount\":null,\"minPrice\":0,\"priceRange\":null,\"ratingCount\":\"\",\"ratingValue\":\"\"},\"bizoppStatus\":5,\"blicenseShow\":1,\"brand\":\"\",\"brandId\":0,\"businessType\":1,\"button\":{\"addVisitButton\":1,\"applyOpenButton\":0,\"applySignButton\":0,\"approveButton\":0,\"batchAllotButton\":1,\"businessInformationViewButton\":1,\"cancelApplyButton\":0,\"conSignPadButton\":0,\"confirmButton\":0,\"controlOperationButton\":1,\"copyMsgGatherUrlButton\":0,\"deliveryPadButton\":1,\"deliverySettingButton\":1,\"editButton\":0,\"financialOperationButton\":1,\"giveUp\":0,\"msgGatherButton\":0,\"msgGatherEditButton\":0,\"msgGatherPlusButton\":0,\"orderInfoButton\":1,\"pickIn\":0,\"sendMsgGatherButton\":0,\"switchToNonAffiliateButton\":0,\"tagsButton\":1,\"topEatsButton\":0,\"updateAbnormalTagsButton\":1,\"updateAddressButton\":1,\"updateBusinessInfoButton\":1," +
                "\"updateCategoryButton\":1,\"updateNormalTag\":1,\"updateShopBaseInfo\":1,\"updateShopInfo\":1,\"uploadPictureButton\":1,\"viewOpenButton\":1,\"visitListButton\":1},\"cLicenseAddr\":\"\",\"cLicenseBrand\":\"\",\"cLicenseExpireFlag\":61,\"cLicenseExpireTime\":\"1970-01-01 08:00:00\",\"cLicenseId\":\"\",\"cLicenseLpName\":\"\",\"cLicensePic\":\"\",\"cLicensePicUrl\":\"\",\"cLicenseStatus\":\"3\",\"cLicenseValidType\":0,\"category\":[],\"categoryIds\":[],\"channel\":1,\"checkStatus\":0,\"city\":52080200,\"cityId\":52080200,\"clicenseShow\":1,\"clusterId\":5764607713452294000,\"clusterName\":\"\",\"commissionJson\":[],\"competeBizTime\":[],\"competeItemMenuUrl\":[],\"completeStatus\":1,\"contactCallingCode\":\"\",\"contactName\":\"test_partyaContact\",\"contactPhone\":\"15512300000\",\"contractorId\":\"5764607748000712588\",\"contractorStatus\":3,\"cosTrans\":0,\"country\":\"MX\",\"countryCode\":\"\",\"createTime\":\"2022-02-16 19:39:42\",\"creatorUserName\":\"simdata01_test_v\"," +
                "\"currency\":{\"display\":\"MX$0.00\",\"number\":\"0.00\",\"position\":0,\"sign\":\"\",\"symbol\":\"MX$\"},\"decorateId\":0,\"deliverDistance\":0,\"deliverType\":2,\"deviceFlagbin\":3,\"deviceFlagbinNeedPad\":1,\"displayable\":0,\"displayableNonAffiliate\":0,\"docFlagbin\":0,\"docId\":\"5764607687590152047\",\"docInfo\":{\"city\":{\"files\":null,\"licensePics\":null},\"corp\":{\"files\":null,\"licensePics\":null},\"health\":{\"files\":null,\"licensePics\":null},\"land\":{\"files\":null,\"licensePics\":null}},\"editTime\":\"2022-02-16 19:40:14\",\"environmentalPhoto\":[\"https://img0.didiglobal.com/static/soda_public/do1_QSl1NDnsiCYaB2A5gmis\"],\"extendMaterial\":\"\",\"extraInfo\":{\"bankFlow\":{\"files\":null,\"pics\":null},\"other\":{\"files\":null,\"pics\":null}},\"filterTags\":[{\"text\":\"正常门店\",\"type\":1}],\"filterTagsValue\":0,\"flagbin\":0,\"foodSafe\":\"\",\"foodSafePic\":\"\",\"foodSafePicUrl\":\"\",\"foodSafeShow\":1," +
                "\"frontPhoto\":[\"https://img0.didiglobal.com/static/soda_public/do1_AMgzb2wxVwL4usmf4qHu\"],\"grade\":\"\",\"headImg\":\"\",\"isDeliverCash\":1,\"isEnableCash\":1,\"isEnablePaypay\":1,\"isEnablePos\":0,\"isFoodCity\":0,\"isIllegal\":0,\"isNeedPad\":0,\"isRate\":2,\"isScopeDelivery\":1,\"itemFiles\":null,\"itemPics\":[\"https://img0.didiglobal.com/static/soda_public/do1_iad8ykReSCUcGh7sUJye\"],\"ka\":1,\"kpContactTime\":null,\"kpDepartment\":\"\",\"kpEmail\":\"test_data_platform@didiglobal.com\",\"kpLastName\":\"\",\"kpLastNameAlph\":\"\",\"kpName\":\"test_kpName\",\"kpNameAlph\":\"\",\"kpOutId\":\"\",\"kpPhone\":\"15512300001\",\"kpPhoneExtra\":null,\"kpPics\":{\"keys\":[\"https://img0.didiglobal.com/static/soda_public/do1_BXtt9g8f8dfiecSHC1dZ\"],\"urls\":[false]},\"kpSource\":\"\",\"lat\":\"28.7370616\",\"lng\":\"-106.1306375\",\"localTaxId\":\"\",\"localTaxPayer\":\"\",\"localTaxPayerAddr\":\"\",\"localTaxPayerIdType\":0,\"localTaxPaymentMethod\":\"\"," +
                "\"localTaxPics\":\"\",\"localTaxVatPayment\":0,\"logo\":\"https://img0.didiglobal.com/static/soda_public/do1_gLdUuwSCysHlKsPzYD9F\",\"logoImg\":\"https://img0.didiglobal.com/static/soda_public/do1_gLdUuwSCysHlKsPzYD9F\",\"lpLicenseId\":\"\",\"lpLicensePicBack\":\"\",\"lpLicensePicBackUrl\":\"\",\"lpLicensePicFront\":\"\",\"lpLicensePicFrontUrl\":\"\",\"lpLicensePics\":\"\",\"lpLicensePicsUrl\":\"\",\"lpLicenseStatus\":\"3\",\"lpLicenseType\":\"0\",\"lpName\":\"\",\"mainCategory\":\"\",\"manualCategory\":[],\"minPrice\":0,\"monthSales\":0,\"monthSold\":0,\"msgGatherUrl\":\"\",\"name\":\"test_1645011556_kFvjwPijhdxkmmgRXKC\",\"nameAlph\":\"\",\"notifyEmail\":\"\",\"notifyPhone\":\"\",\"objBusinessAttr\":[],\"objTags\":[],\"officialUrl\":\"\",\"open\":1,\"openCheck\":[{\"button\":\"已添加\",\"text\":\"联系电话\",\"type\":1},{\"button\":\"已填写\",\"text\":\"配送信息\",\"type\":1},{\"button\":\"已添加\",\"text\":\"门店头图\",\"type\":1},{\"button\":\"未完成\",\"text\":\"可售菜品数量\"," +
                "\"type\":0},{\"button\":\"已通过\",\"text\":\"营业时间段\",\"type\":1},{\"button\":\"已添加\",\"text\":\"门店主营品类\",\"type\":1},{\"button\":\"已添加\",\"text\":\"出餐时长\",\"type\":1}],\"openCheckNonAffiliate\":null,\"orderConfirmMethod\":\"0\",\"padInfo\":{\"isNeedPad\":0,\"isPadOnlineSign\":1},\"padStatus\":0,\"payVerifyReason\":\"\",\"payVerifyStatus\":0,\"permAddr\":\"\",\"permNo\":\"\",\"phone\":\"164501155695\",\"poi\":\"ChIJOdOau5VB6oYR-LfkU_bfr_g\",\"poiId\":\"ChIJOdOau5VB6oYR-LfkU_bfr_g\",\"poiName\":\"Arcos de Terragona 16934, Villa del Nte, 31137 Chihuahua, Chih., México\",\"postCode\":\"\",\"produceTimeMax\":\"0\",\"rank\":5,\"repeatShopId\":\"\",\"reprePhone\":\"\",\"riderAreaName\":\"\",\"riderClusterId\":52080200,\"score\":0,\"secondCategory\":\"\",\"selectCityId\":52080200,\"settledStatus\":0,\"settledTags\":0,\"shopId\":\"5764607747732277132\",\"shopLayerLevel\":\"\",\"shopLevel\":\"\",\"shopOnlineStatus\":3,\"shopOnlineStatusDesc\":\"需手工开启营业\"," +
                "\"shopOnlineStatusMsg\":\"歇业中\",\"shopPhone\":\"\",\"shopPics\":{\"environmentalPhoto\":[\"/static/soda_public/do1_QSl1NDnsiCYaB2A5gmis\"],\"frontPhoto\":[\"/static/soda_public/do1_AMgzb2wxVwL4usmf4qHu\"]},\"shopStatus\":3,\"shopTag\":0,\"signPic\":null,\"signType\":1,\"signedTime\":1645011613,\"source\":2,\"startupCost\":500000,\"startupCostLimit\":50000,\"startupCostNum\":0,\"startupCostProportion\":1000,\"startupExecWeek\":1,\"tags\":0,\"taxId\":\"234234123112\",\"taxIdType\":1,\"taxPayer\":\"\",\"taxPayerAddr\":\"\",\"taxPayerIdType\":0,\"taxPaymentMethod\":\"\",\"taxPics\":[\"https://img0.didiglobal.com/static/soda_public/do1_Gip4umImni45IUHPTvQQ\"],\"taxVatPayment\":0,\"thirdCategory\":\"\",\"timezone\":\"\",\"todayBizTime\":[],\"type\":1,\"updateTime\":\"2022-03-10 15:46:20\",\"visible\":false,\"visitStatus\":0},\"errmsg\":\"ok\",\"errno\":0,\"machine\":\"\",\"reqParams\":{\"locale\":\"zh-CN\",\"shopId\":\"5764607747732277132\"},\"time\":1646912275," +
                "\"traceId\":\"s060310x1646912272\"},\"timestamp\":\"2022-03-10T19:37:55.180+0800\"}";
        System.out.println("字符串长度： "+str.length());
        System.out.println("字符串byte长度："+str.getBytes().length);
        byte[] bt = ZstdUtils.compress(str.getBytes());
        String newStr = ZstdUtils.decompress(bt);
        System.out.println("新字符串："+newStr);
        System.out.println("新字符串的byte长度："+newStr.getBytes().length);

        System.out.println("Zstd压缩后的byte长度："+bt.length);

        byte[] bs = GzipCompress.compress(str.getBytes());
        System.out.println("Gzip压缩后的byte长度："+bs.length);
        System.out.println("Zstd压缩后的byte长度 + Gzip压缩后的byte长度："+GzipCompress.compress(bt).length);


        System.out.println("Base64 后的byte长度: "+Base64.getEncoder().encode(bt).length);

        System.out.println("=======");
        System.out.println(CollectMode.COMPRESS_LOG_RESP);
        System.out.println(CollectMode.EXTRACT_ALL);

        System.out.println();
        JcProperties properties = new JcProperties();
        String path = "/Users/didi/Desktop/jlog/example/target/classes/application.properties";
        properties.load(new FileInputStream(path));
        System.out.println(JSON.toJSONString(properties));
        TagConfig bean = properties.getBean("tag-config", TagConfig.class);
        System.out.println(JSON.toJSONString(bean));
    }

}
