<template>
  <div id="app" class="bg-color" ref="app" style="position: absolute;top: 0;bottom: 0;left: 0;right: 0;overflow-y: auto;">
    <!--    <img alt="Vue logo" src="./assets/logo.png">-->
    <!--    <HelloWorld msg="Welcome to Your Vue.js App"/>-->
    <hi-header back more :style="{marginTop:statusBarHeight+'px'} ">
      <span slot="title">西门子检测盒</span>
    </hi-header>
    <div style="padding: 16px;margin-top: 100px">

      <!--      <hi-annular></hi-annular>-->
      <hi-product
          class="logo"
          :bigUrl="'/img/ic_public_default.png'"
          :miniUrl="'/img/logo-supor.png'"></hi-product>
      <hi-card :height="88">
        <hi-row :padding="0">
          <hi-col :span="1">
            <div class="text-color-one model-big-title-size model-big-item1">{{ (connectType==1?'待机':(connectType==2?'连接中...':'已连接')) }}</div>
          </hi-col>
          <hi-col :span="1">
            <div class="model-big-item2" v-show="connectType==3">
              <div class="model-content-title-size hi-color-4">优</div>
              <div class="model-title-subtitle-size hi-color-4">PM2.5:20</div>
            </div>
          </hi-col>
          <hi-col :span="1">
            <img :src="'/img/'+(connectType==1?'ic_power_02':(connectType==2?'loading':'ic_power_01'))+'.png'" @click="connectType != 2 && connect()" class="model-big-item3">
          </hi-col>
        </hi-row>
      </hi-card>


      <hi-row :padding="8">
        <hi-col :span="1">
          <hi-card >
            <hi-row class="item2" :padding="0">
              <hi-col :span="1">
                <div class="item1-div">
                  <div class="text-color-one model-title-main-size" @click="showDialog = true">提示</div>
                </div>
              </hi-col>
              <hi-col :span="1">
                <img src="/img/appoint_on.png" class="item-logo">
              </hi-col>
            </hi-row>
          </hi-card>
        </hi-col>
        <hi-col :span="1">
          <hi-card>
            <hi-dropdown :list="dropdownList" v-model="id">
              <span slot="content">
                <hi-row class="item2" :padding="0">
                  <hi-col :span="1">
                    <div class="item2-div">
                      <div class="text-color-one model-title-main-size">风速</div>
                      <div class="text-color-two model-title-subtitle-size">{{getName(this.id)}}</div>
                    </div>
                  </hi-col>
                  <hi-col :span="1">
                    <img src="/img/appoint_on.png" class="item-logo">
                  </hi-col>
                </hi-row>
              </span>
            </hi-dropdown>
          </hi-card>
        </hi-col>
      </hi-row>

      <hi-card :height="95">
        <hi-swipe :height="70">
          <div slot="swipe-item">
            <div class="swipe-item" v-for="(item,index) in 3" :key="index">
              <hi-row
                  :padding="0"
                  is-vertical-line
                  style="margin-top: 16px">
                <hi-col :span="1" v-for="(item1,index1) in 3" :key="index1">
                  <div class="model-content-title-size text-color-one">标题</div>
                  <div class="model-title-subtitle-size text-color-two">副标题</div>
                </hi-col>
              </hi-row>
            </div>
          </div>
        </hi-swipe>
      </hi-card>

      <hi-card>
        <hi-row>
          <hi-col :span="1">
            <div class="item3-div">
              <div class="text-color-one model-title-main-size">亮度</div>
              <div class="text-color-two model-title-subtitle-size" style="margin-top: 2px">{{progressVal*10}}%</div>
            </div>
          </hi-col>
          <hi-col :span="4">
            <div style="padding-right: 16px;">
              <hi-progress class="item3-rang" :min="0" :max="10" v-model="progressVal"></hi-progress>
            </div>
          </hi-col>
        </hi-row>
      </hi-card>
      <hi-row :padding="8">
        <hi-col :span="1">
          <hi-card >
            <hi-row class="item2" :padding="0">
              <hi-col :span="1">
                <div class="item1-div">
                  <div class="text-color-one model-title-main-size" @click="showTimeDialog = true">{{ time?time:'时间' }}</div>
                </div>
              </hi-col>
              <hi-col :span="1">
                <img src="/img/appoint_on.png" class="item-logo">
              </hi-col>
            </hi-row>
          </hi-card>
        </hi-col>
        <hi-col :span="1">
          <hi-card>
            <hi-row>
              <hi-col :span="1" style="margin-top: 20px;margin-left: 16px;">
                <hi-switch v-model="switchState" @change="switchChange"></hi-switch>
              </hi-col>
              <hi-col :span="1" style="margin-top: 20px;margin-right: 16px;">
                <hi-switch
                    v-model="switchState1"
                    no-color="red"></hi-switch>
              </hi-col>
            </hi-row>
          </hi-card>
        </hi-col>
      </hi-row>

      <hi-row :padding="8">
        <hi-col :span="1">
          <hi-card></hi-card>
        </hi-col>
        <hi-col :span="1">
          <hi-card></hi-card>
        </hi-col>
      </hi-row>

      <hi-row :padding="8">
        <hi-col :span="1">
          <hi-card ></hi-card>
        </hi-col>
        <hi-col :span="1">
          <hi-card></hi-card>
        </hi-col>
      </hi-row>

    </div>
    <hi-dialog
        :visible="showDialog"
        title="温馨提示" @openDialog="openDialog">
      <div slot="dialog-body">
        开启功能后，净化器在关闭未断电源的状态下也会检测PM2.5值，检测过程中会伴有轻微风声
      </div>
      <div slot="dialog-foot">
        <hi-row is-vertical-line :padding="0">
          <hi-col :span="1" class="hi-btn-color-blue">
            <span @click="showDialog = false">取消</span>
          </hi-col>
          <hi-col :span="1" class="hi-btn-color-blue">
            确认开始
          </hi-col>
        </hi-row>
      </div>
    </hi-dialog>
    <hi-time-picker :visible="showTimeDialog" title="设置时间" @confirm="timeConfirm" v-model="time"></hi-time-picker>
  </div>
</template>

<script>
export default {
  name: 'App',
  data(){
    return{
      dropdownList:[
        {
          id:0,
          name:'第一项'
        },
        {
          id:1,
          name:'第二项'
        },
        {
          id:2,
          name:'第三项'
        },
        {
          id:3,
          name:'第四项'
        },
        {
          id:4,
          name:'第五项'
        },
        {
          id:5,
          name:'第六项'
        }
      ],
      id:1,
      statusBarHeight:0,
      connectType:1,
      progressVal:3,
      showDialog:false,
      showTimeDialog:false,
      time:'03:30',
      switchState:false,
      switchState1:false
    }
  },
  watch:{
    progressVal(val){
      console.log(val)
    }
  },
  mounted() {
    if (window.hilink){
      window.getStatusBarHeight1 = res => {
        console.log(res)
        let data = JSON.parse(res);
        this.statusBarHeight = data.statusBarHeight;
      }
      window.hilink.getStatusBarHeight('getStatusBarHeight1');
      window.hilink.setTitleVisible(false);
    }
    this.$toast({
      msg:'异常'
    })
    let load = this.$loading({
      msg:'加载中...'
    })
    setTimeout(()=>{
      load.close();
    },1000)
  },
  methods:{
    switchChange(val){
      if (val){
        this.$switchTheme('dark');
      }else{
        this.$switchTheme('light');
      }
    },
    timeConfirm(type,val){
      this.showTimeDialog = false
    },
    connect(){
      if (this.connectType == 1){
        this.connectType = 2;
        setTimeout(()=>{
          this.connectType = 3;
        },5000);
      }else{
        this.connectType = 1;
      }
    },
    getName(id){
      for (let i = 0; i < this.dropdownList.length; i++) {
        if (id == this.dropdownList[i].id){return this.dropdownList[i].name}
      }
    },
    openDialog(){console.log('openDialog')},
    closeDialog(){console.log('closeDialog')}
  }
}
</script>

<style>
body{
  overflow: auto;
  -webkit-overflow-scrolling: touch;
}
#app {
  height:100%;
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
}
.logo{
  margin-top: 56px;
}
.item1-div{
  padding-left: 16px;
  text-align: left;
  line-height: 66px;
}
.item2-div,.item3-div{
  padding: 13px 16px;
  text-align: left;
}
.item-logo{
  width: 24px;
  height: 24px;
  float: right;
  padding: 20px 16px;
}
.model-big-item1{
  text-align: left;
  padding-left: 16px;
  line-height: 88px;
}
.model-big-item2{
  text-align: center;
  padding: 18px 0;
}
.model-big-item2:last-child{
  margin-top: 2px;
}
.model-big-item3{
  width: 60px;
  height: 60px;
  float: right;
  padding:14px 4px;
  line-height: 88px;

}
.item3-rang{
  width: 100%;
  margin-top: 19px;
}

</style>
