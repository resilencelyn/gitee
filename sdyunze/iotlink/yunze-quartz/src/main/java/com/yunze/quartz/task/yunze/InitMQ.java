package com.yunze.quartz.task.yunze;

import com.yunze.common.config.RabbitMQConfig;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.stereotype.Component;

import javax.annotation.Resource;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * @Auther: zhang feng
 * @Date: 2021/08/31/12:13
 * @Description:
 */
@Component("initMQ")
public class InitMQ {
    @Resource
    private RabbitTemplate rabbitTemplate;
    @Resource
    private RabbitMQConfig rabbitMQConfig;

    /**
     * 初始化 Mq
     */
    public void initMQConfig(){

        List<Map<String,Object>> MqinitArr = new ArrayList<Map<String,Object>>();
        Map<String,Object> Obj_1 = new HashMap<String,Object>();
        Map<String,Object> Obj_2 = new HashMap<String,Object>();
        Map<String,Object> Obj_3 = new HashMap<String,Object>();
        Map<String,Object> Obj_4 = new HashMap<String,Object>();
        Map<String,Object> Obj_5 = new HashMap<String,Object>();
        Map<String,Object> Obj_6 = new HashMap<String,Object>();
        Map<String,Object> Obj_7 = new HashMap<String,Object>();
        Map<String,Object> Obj_8 = new HashMap<String,Object>();
        Map<String,Object> Obj_9 = new HashMap<String,Object>();
        Map<String,Object> Obj_10 = new HashMap<String,Object>();
        Map<String,Object> Obj_11 = new HashMap<String,Object>();
        Map<String,Object> Obj_12 = new HashMap<String,Object>();
        Map<String,Object> Obj_13 = new HashMap<String,Object>();
        Map<String,Object> Obj_14 = new HashMap<String,Object>();
        Map<String,Object> Obj_15 = new HashMap<String,Object>();
        Map<String,Object> Obj_16 = new HashMap<String,Object>();
        Map<String,Object> Obj_17 = new HashMap<String,Object>();
        Map<String,Object> Obj_18 = new HashMap<String,Object>();
        Map<String,Object> Obj_19 = new HashMap<String,Object>();
        Map<String,Object> Obj_20 = new HashMap<String,Object>();
        Map<String,Object> Obj_21 = new HashMap<String,Object>();
        Map<String,Object> Obj_22 = new HashMap<String,Object>();
        Map<String,Object> Obj_23 = new HashMap<String,Object>();
        Map<String,Object> Obj_24 = new HashMap<String,Object>();
        Map<String,Object> Obj_25 = new HashMap<String,Object>();
        Map<String,Object> Obj_26 = new HashMap<String,Object>();
        Map<String,Object> Obj_27 = new HashMap<String,Object>();
        Map<String,Object> Obj_28 = new HashMap<String,Object>();
        Map<String,Object> Obj_29 = new HashMap<String,Object>();
        Map<String,Object> Obj_30 = new HashMap<String,Object>();
        Map<String,Object> Obj_31 = new HashMap<String,Object>();
        Map<String,Object> Obj_32 = new HashMap<String,Object>();
        Map<String,Object> Obj_33 = new HashMap<String,Object>();
        Map<String,Object> Obj_34 = new HashMap<String,Object>();
        Map<String,Object> Obj_35 = new HashMap<String,Object>();

        Map<String,Object> Obj_36 = new HashMap<String,Object>();
        Map<String,Object> Obj_37 = new HashMap<String,Object>();
        Map<String,Object> Obj_38 = new HashMap<String,Object>();
        Map<String,Object> Obj_39 = new HashMap<String,Object>();
        Map<String,Object> Obj_40 = new HashMap<String,Object>();
        Map<String,Object> Obj_41 = new HashMap<String,Object>();
        Map<String,Object> Obj_42 = new HashMap<String,Object>();
        Map<String,Object> Obj_43 = new HashMap<String,Object>();
        Map<String,Object> Obj_44 = new HashMap<String,Object>();
        Map<String,Object> Obj_45 = new HashMap<String,Object>();
        Map<String,Object> Obj_46 = new HashMap<String,Object>();
        Map<String,Object> Obj_47 = new HashMap<String,Object>();
        Map<String,Object> Obj_48 = new HashMap<String,Object>();
        Map<String,Object> Obj_49 = new HashMap<String,Object>();
        Map<String,Object> Obj_50 = new HashMap<String,Object>();

        Map<String,Object> Obj_51 = new HashMap<String,Object>();
        Map<String,Object> Obj_52 = new HashMap<String,Object>();
        Map<String,Object> Obj_53 = new HashMap<String,Object>();
        Map<String,Object> Obj_54 = new HashMap<String,Object>();
        Map<String,Object> Obj_55 = new HashMap<String,Object>();
        Map<String,Object> Obj_56 = new HashMap<String,Object>();
        Map<String,Object> Obj_57 = new HashMap<String,Object>();
        Map<String,Object> Obj_58 = new HashMap<String,Object>();
        Map<String,Object> Obj_59 = new HashMap<String,Object>();
        Map<String,Object> Obj_60 = new HashMap<String,Object>();




        Obj_1.put("card_exchangeName","admin_exchange");
        Obj_1.put("card_queueName","admin_CardImportSelImei_queue");
        Obj_1.put("card_routingKey","admin.CardImportSelImei.queue");

        Obj_2.put("card_exchangeName","admin_exchange");
        Obj_2.put("card_queueName","admin_CardImportSet_queue");
        Obj_2.put("card_routingKey","admin.CardImportSet.queue");

        Obj_3.put("card_exchangeName","admin_exchange");
        Obj_3.put("card_queueName","admin_saveCard_queue");
        Obj_3.put("card_routingKey","admin.saveCard.queue");
        Obj_3.put("is_noDxl","1");

        Obj_4.put("card_exchangeName","admin_exchange");
        Obj_4.put("card_queueName","admin_DistributeCard_queue");
        Obj_4.put("card_routingKey","admin.DistributeCard.queue");
        Obj_4.put("is_noDxl","1");

        Obj_5.put("card_exchangeName","admin_exchange");
        Obj_5.put("card_queueName","admin_OutCard_queue");
        Obj_5.put("card_routingKey","admin_OutCard_queue");
        Obj_5.put("is_noDxl","1");


        Obj_6.put("card_exchangeName","admin_exchange");
        Obj_6.put("card_queueName","admin_BackupsyzSysLogs_queue");
        Obj_6.put("card_routingKey","admin.BackupsyzSysLogs.queue");
        Obj_6.put("is_noDxl","1");

        Obj_7.put("card_exchangeName","admin_exchange");
        Obj_7.put("card_queueName","admin_BackupsGenTable_queue");
        Obj_7.put("card_routingKey","admin.BackupsGenTable.queue");
        Obj_7.put("is_noDxl","1");

        Obj_8.put("card_exchangeName","polling_card");
        Obj_8.put("card_queueName","polling_card_activatedate");
        Obj_8.put("card_routingKey","polling.card.activatedate");
        Obj_8.put("is_noDxl","1");


        Obj_9.put("card_exchangeName","polling_card");
        Obj_9.put("card_queueName","polling_card_flow");
        Obj_9.put("card_routingKey","polling.card.flow");
        Obj_9.put("is_noDxl","1");

        Obj_10.put("card_exchangeName","polling_card");
        Obj_10.put("card_queueName","polling_card_status");
        Obj_10.put("card_routingKey","polling.card.status");
        Obj_10.put("is_noDxl","1");

        Obj_11.put("card_exchangeName","polling_card");
        Obj_11.put("card_queueName","polling_card_stop");
        Obj_11.put("card_routingKey","polling.card.stop");
        Obj_11.put("is_noDxl","1");

        Obj_12.put("card_exchangeName","admin_exchange");
        Obj_12.put("card_queueName","admin_OrderImportRecharge_queue");
        Obj_12.put("card_routingKey","admin.OrderImportRecharge.queue");


        Obj_13.put("card_exchangeName","polling_addPackage_card");
        Obj_13.put("card_queueName","p_addPackage_card");
        Obj_13.put("card_routingKey","p.addPackage.card");



        Obj_14.put("card_exchangeName","admin_card_exchange");
        Obj_14.put("card_queueName","admin_CardUpd_queue");
        Obj_14.put("card_routingKey","admin.CardUpd.queue");

      /*  Obj_15.put("card_exchangeName","dlx_admin_card_exchange");
        Obj_15.put("card_queueName","dlx_admin_CardUpd_queue");
        Obj_15.put("card_routingKey","dlx_admin.CardUpd.queue");*/


        Obj_16.put("card_exchangeName","admin_exchange");
        Obj_16.put("card_queueName","admin_WxWebOrder_queue");
        Obj_16.put("card_routingKey","admin.WxWebOrder.queue");

       /* Obj_17.put("card_exchangeName","dlx_admin_exchange");
        Obj_17.put("card_queueName","dlx_admin_WxWebOrder_queue");
        Obj_17.put("card_routingKey","dlx_admin.WxWebOrder.queue");*/

        Obj_18.put("card_exchangeName","polling_card");
        Obj_18.put("card_queueName","polling_card_disconnected");
        Obj_18.put("card_routingKey","polling.card.disconnected");
        Obj_18.put("is_noDxl","1");

        Obj_19.put("card_exchangeName","admin_card_exchange");
        Obj_19.put("card_queueName","admin_CardDiagnosis_queue");
        Obj_19.put("card_routingKey","admin.CardDiagnosis.queue");


        Obj_20.put("card_exchangeName","admin_exchange");
        Obj_20.put("card_queueName","admin_CardImportBatch_queue");
        Obj_20.put("card_routingKey","admin.CardImportBatch.queue");

        Obj_21.put("card_exchangeName","admin_exchange");
        Obj_21.put("card_queueName","admin_importSetCardInfo_queue");
        Obj_21.put("card_routingKey","admin.importSetCardInfo.queue");



        Obj_22.put("card_exchangeName","admin_exchange");
        Obj_22.put("card_queueName","admin_MonthFirstday_queue");
        Obj_22.put("card_routingKey","admin.MonthFirstday.queue");


        Obj_23.put("card_exchangeName","admin_exchange");
        Obj_23.put("card_queueName","admin_CEndValue_queue");
        Obj_23.put("card_routingKey","admin.CEndValue.queue");



        Obj_24.put("card_exchangeName","admin_exchange");
        Obj_24.put("card_queueName","admin_WxShopping_queue");
        Obj_24.put("card_routingKey","admin.WxShopping.queue");

        Obj_25.put("card_exchangeName","admin_CC_exchange");
        Obj_25.put("card_queueName","admin_ShoppingEmail_queue");
        Obj_25.put("card_routingKey","admin.ShoppingEmail.queue");



        Obj_26.put("card_exchangeName","polling_cardCardFlow_exchange");
        Obj_26.put("card_queueName","polling_card_flow_one");
        Obj_26.put("card_routingKey","admin.CardFlow.queue.One");

        Obj_27.put("card_exchangeName","polling_cardCardFlow_exchange");
        Obj_27.put("card_queueName","polling_card_flow_tow");
        Obj_27.put("card_routingKey","admin.CardFlow.queue.Tow");


        Obj_28.put("card_exchangeName","polling_cardActivateDate_exchange");
        Obj_28.put("card_queueName","polling_card_activateDate_one");
        Obj_28.put("card_routingKey","admin.CardActivateDate.queue.One");

        Obj_29.put("card_exchangeName","polling_cardActivateDate_exchange");
        Obj_29.put("card_queueName","polling_card_activateDate_tow");
        Obj_29.put("card_routingKey","admin.CardActivateDate.queue.Tow");



        Obj_30.put("card_exchangeName","polling_cardCardStatus_exchange");
        Obj_30.put("card_queueName","polling_card_status_one");
        Obj_30.put("card_routingKey","admin.CardStatus.queue.One");

        Obj_31.put("card_exchangeName","polling_cardCardStatus_exchange");
        Obj_31.put("card_queueName","polling_card_status_tow");
        Obj_31.put("card_routingKey","admin.CardStatus.queue.Tow");


        Obj_32.put("card_exchangeName","polling_cardCardStop_exchange");
        Obj_32.put("card_queueName","polling_card_Stop_one");
        Obj_32.put("card_routingKey","admin.CardStop.queue.One");

        Obj_33.put("card_exchangeName","polling_cardCardStop_exchange");
        Obj_33.put("card_queueName","polling_card_Stop_tow");
        Obj_33.put("card_routingKey","admin.CardStop.queue.Tow");


        Obj_34.put("card_exchangeName","polling_cardCardDisconnected_exchange");
        Obj_34.put("card_queueName","polling_card_Disconnected_one");
        Obj_34.put("card_routingKey","admin.CardDisconnected.queue.One");

        Obj_35.put("card_exchangeName","polling_cardCardDisconnected_exchange");
        Obj_35.put("card_queueName","polling_card_Disconnected_tow");
        Obj_35.put("card_routingKey","admin.CardDisconnected.queue.Tow");


        Obj_36.put("card_exchangeName","admin_exchange");
        Obj_36.put("card_queueName","admin_Exportallorders_queue");
        Obj_36.put("card_routingKey","admin.Exportallorders.queue");
        Obj_36.put("is_noDxl","1");


        Obj_37.put("card_exchangeName","admin_exchange");
        Obj_37.put("card_queueName","admin_Shoppingexport_queue");
        Obj_37.put("card_routingKey","admin.Shoppingexport.queue");
        Obj_37.put("is_noDxl","1");



        Obj_38.put("card_exchangeName","admin_exchange");
        Obj_38.put("card_queueName","admin_Consumption_queue");
        Obj_38.put("card_routingKey","admin.Consumption.queue");


        Obj_39.put("card_exchangeName","admin_exchange");
        Obj_39.put("card_queueName","admin_Stopped_queue");
        Obj_39.put("card_routingKey","admin.Stopped.queue");


        Obj_40.put("card_exchangeName","admin_exchange");
        Obj_40.put("card_queueName","admin_PublicMethod_queue");
        Obj_40.put("card_routingKey","admin.PublicMethod.queue");


        Obj_41.put("card_exchangeName","admin_exchange");
        Obj_41.put("card_queueName","admin_ConsumptionAndState_queue");
        Obj_41.put("card_routingKey","admin.ConsumptionAndState.queue");

        Obj_42.put("card_exchangeName","admin_exchange");
        Obj_42.put("card_queueName","admin_DueExpireTime_queue");
        Obj_42.put("card_routingKey","admin.DueExpireTim.queue");


        Obj_43.put("card_exchangeName","admin_exchange");
        Obj_43.put("card_queueName","admin_ImportStock_queue");
        Obj_43.put("card_routingKey","admin.ImportStock.queue");

        Obj_44.put("card_exchangeName","admin_exchange");
        Obj_44.put("card_queueName","admin_OpenNetwork_queue");
        Obj_44.put("card_routingKey","admin.OpenNetwork.queue");


        Obj_45.put("card_exchangeName","admin_exchange");
        Obj_45.put("card_queueName","admin_DisconnectNetwork_queue");
        Obj_45.put("card_routingKey","admin.DisconnectNetwork.queue");

        Obj_46.put("card_exchangeName","admin_exchange");
        Obj_46.put("card_queueName","admin_OpenNetwork_queue");
        Obj_46.put("card_routingKey","admin.OpenNetwork.queue");


        Obj_47.put("card_exchangeName","admin_exchange");
        Obj_47.put("card_queueName","admin_Machine_queue");
        Obj_47.put("card_routingKey","admin.Machine.queue");

        Obj_48.put("card_exchangeName","admin_exchange");
        Obj_48.put("card_queueName","admin_ExportFlow_queue");
        Obj_48.put("card_routingKey","admin.ExportFlow.queue");

        Obj_49.put("card_exchangeName","admin_exchange");
        Obj_49.put("card_queueName","admin_ExportFlowHis_queue");
        Obj_49.put("card_routingKey","admin.ExportFlowHis.queue");


        Obj_50.put("card_exchangeName","admin_exchange");
        Obj_50.put("card_queueName","admin_uploadCustomer_queue");
        Obj_50.put("card_routingKey","admin.uploadCustomer.queue");


        Obj_51.put("card_exchangeName","admin_exchange");
        Obj_51.put("card_queueName","admin_IndexDataGenerate_queue");
        Obj_51.put("card_routingKey","admin.IndexDataGenerate.queue");

        Obj_52.put("card_exchangeName","admin_exchange");
        Obj_52.put("card_queueName","admin_getSynActivateDate_queue");
        Obj_52.put("card_routingKey","admin.getSynActivateDate.queue");


        Obj_53.put("card_exchangeName","admin_exchange");
        Obj_53.put("card_queueName","admin_ApiSynCardUsed_queue");
        Obj_53.put("card_routingKey","admin.ApiSynCardUsed.queue");

        Obj_54.put("card_exchangeName","admin_exchange");
        Obj_54.put("card_queueName","admin_ApiSynCardStatus_queue");
        Obj_54.put("card_routingKey","admin.ApiSynCardStatus.queue");

        Obj_55.put("card_exchangeName","admin_exchange");
        Obj_55.put("card_queueName","admin_ApiSynUpdCardStatus_queue");
        Obj_55.put("card_routingKey","admin.ApiSynUpdCardStatus.queue");


        Obj_56.put("card_exchangeName","admin_exchange");
        Obj_56.put("card_queueName","admin_ApiSynUpdCardConnectionStatus_queue");
        Obj_56.put("card_routingKey","admin.ApiSynUpdCardConnectionStatus.queue");


        MqinitArr.add(Obj_1);
        MqinitArr.add(Obj_2);
        MqinitArr.add(Obj_3);
        MqinitArr.add(Obj_4);
        MqinitArr.add(Obj_5);
        MqinitArr.add(Obj_6);
        MqinitArr.add(Obj_7);
        MqinitArr.add(Obj_8);
        MqinitArr.add(Obj_9);
        MqinitArr.add(Obj_10);
        MqinitArr.add(Obj_11);
        MqinitArr.add(Obj_12);
        MqinitArr.add(Obj_13);
        MqinitArr.add(Obj_14);
        //MqinitArr.add(Obj_15);
        MqinitArr.add(Obj_16);
        //MqinitArr.add(Obj_17);
        MqinitArr.add(Obj_18);
        MqinitArr.add(Obj_19);

        MqinitArr.add(Obj_20);
        MqinitArr.add(Obj_21);
        MqinitArr.add(Obj_22);
        MqinitArr.add(Obj_23);
        MqinitArr.add(Obj_24);
        MqinitArr.add(Obj_25);
        MqinitArr.add(Obj_26);
        MqinitArr.add(Obj_27);
        MqinitArr.add(Obj_28);
        MqinitArr.add(Obj_29);
        MqinitArr.add(Obj_30);
        MqinitArr.add(Obj_31);

        MqinitArr.add(Obj_32);
        MqinitArr.add(Obj_33);
        MqinitArr.add(Obj_34);
        MqinitArr.add(Obj_35);


        MqinitArr.add(Obj_36);
        MqinitArr.add(Obj_37);


        MqinitArr.add(Obj_38);
        MqinitArr.add(Obj_39);
        MqinitArr.add(Obj_40);
        MqinitArr.add(Obj_41);
        MqinitArr.add(Obj_42);
        MqinitArr.add(Obj_43);
        MqinitArr.add(Obj_44);
        MqinitArr.add(Obj_45);
        MqinitArr.add(Obj_46);
        MqinitArr.add(Obj_47);
        MqinitArr.add(Obj_48);
        MqinitArr.add(Obj_49);

        MqinitArr.add(Obj_50);

        MqinitArr.add(Obj_51);

        MqinitArr.add(Obj_52);

        MqinitArr.add(Obj_53);
        MqinitArr.add(Obj_54);
        MqinitArr.add(Obj_55);
        MqinitArr.add(Obj_56);


        for (int i = 0; i < MqinitArr.size(); i++) {
            Map<String,Object> Obj = MqinitArr.get(i);
            String card_exchangeName = Obj.get("card_exchangeName").toString();
            String card_queueName = Obj.get("card_queueName").toString();
            String card_routingKey = Obj.get("card_routingKey").toString();
            String card_del_exchangeName = "dlx_"+card_exchangeName,card_del_queueName = "dlx_"+card_queueName, card_del_routingKey = "dlx_"+card_routingKey;

            try {
                if(Obj.get("is_noDxl")!=null){
                    rabbitMQConfig.creatExchangeQueue(card_exchangeName, card_queueName, card_routingKey, null, null, null,null);
                }else{
                    rabbitMQConfig.creatExchangeQueue(card_exchangeName, card_queueName, card_routingKey, card_del_exchangeName, card_del_queueName, card_del_routingKey,null);
                }
            }catch (Exception e){
                System.out.println("exchangeName "+card_exchangeName+" Mqinit 初始化失败 ……"+e.getMessage());
            }
        }


    }

    public static void main(String[] args) {
        InitMQ init = new InitMQ();
        init.initMQConfig();
    }


}