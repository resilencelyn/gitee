<template>
  <el-container >
    <el-header style="padding:0px;height: 100%">
      <el-menu  class="el-menu-demo " mode="horizontal"
                style="background: -webkit-linear-gradient(top, rgb(29 76 102), rgb(13 40 67));border: 1px solid #1e4b68;">
        <el-menu-item class="Mymenu-item" aria-readonly="true" index="3">关于</el-menu-item>
        <el-menu-item class="Mymenu-item" aria-readonly="true" index="2">开源计划</el-menu-item>
       <!-- <el-menu-item class="Mymenu-item"  index="3">解决方案</el-menu-item>
        <el-menu-item class="Mymenu-item"  index="2">产品中心</el-menu-item>-->
        <el-menu-item class="Mymenu-item" aria-readonly="true" index="1">首页</el-menu-item>
      </el-menu>
    </el-header>
    <el-main style="padding:0px;">
      <template >
        <el-carousel :interval="5000" style="height: 658px;width: 100%;" arrow="never" >
          <el-carousel-item class="carousel carousel_1">
          </el-carousel-item>
          <el-carousel-item class="carousel carousel_2">
          </el-carousel-item >
          <el-carousel-item class="carousel carousel_3">
          </el-carousel-item>
          <el-carousel-item class="carousel carousel_4">
          </el-carousel-item>
        </el-carousel>
      </template>

      <el-form ref="loginForm" :model="loginForm" style="position: absolute; top: 185px;right: 12%;  z-index: 199;" :rules="loginRules" class="login-form">
        <h3 class="title">IOT综合业务支撑平台</h3>
        <el-form-item prop="username">
          <el-input v-model="loginForm.username" type="text" auto-complete="off" placeholder="账号">
            <svg-icon slot="prefix" icon-class="user" class="el-input__icon input-icon" />
          </el-input>
        </el-form-item>
        <el-form-item prop="password">
          <el-input
            v-model="loginForm.password"
            type="password"
            auto-complete="off"
            placeholder="密码"
            @keyup.enter.native="handleLogin"
          >
            <svg-icon slot="prefix" icon-class="password" class="el-input__icon input-icon" />
          </el-input>
        </el-form-item>
        <el-form-item prop="code">
          <el-input
            v-model="loginForm.code"
            auto-complete="off"
            placeholder="验证码"
            style="width: 63%"
            @keyup.enter.native="handleLogin"
          >
            <svg-icon slot="prefix" icon-class="validCode" class="el-input__icon input-icon" />
          </el-input>
          <div class="login-code">
            <img :src="codeUrl" @click="getCode" class="login-code-img"/>
          </div>
        </el-form-item>
        <el-checkbox v-model="loginForm.rememberMe" style="margin:0px 0px 25px 0px;">记住密码</el-checkbox>
        <el-form-item style="width:100%;">
          <el-button
            :loading="loading"
            size="medium"
            type="primary"
            style="width:100%;"
            @click.native.prevent="handleLogin"
          >
            <span v-if="!loading">登 录</span>
            <span v-else>登 录 中...</span>
          </el-button>
        </el-form-item>
      </el-form>

      <!--<div>
        <el-row style="width: 80%;margin:50px auto 20px;" :gutter="50" class="my_panel-group" >
          <el-col :xs="24" :sm="8" :md="6" :lg="6" class="card-panel-col"  >
              <el-card :body-style="{ padding: '0px' }" class="my_loginCard">
               &lt;!&ndash; <el-row>
                  <el-col :xs="6" :sm="6" :md="6" :lg="6">
                    <div class="card-panel-icon-wrapper icon-simCard">
                      <svg-icon icon-class="simCard" class-name="card-panel-icon"/>
                    </div>
                  </el-col>
                  <el-col :xs="18" :sm="18" :md="18" :lg="18">
                   <ul class="illustrateUl">
                        <li>通信管理</li>
                        <li>物联卡 常用功能</li>
                        <li>资费组 资费计划管理</li>
                        <li>卡信息 变更记录</li>
                        <li>智能诊断 在线信息</li>
                    </ul>
                  </el-col>
                </el-row>&ndash;&gt;
              </el-card>
            </el-col>

          <el-col :xs="24" :sm="8" :md="6" :lg="6" class="card-panel-col"  >
            <el-card :body-style="{ padding: '0px' }" class="my_loginCard">
              &lt;!&ndash;<el-row>
                <el-col :xs="6" :sm="6" :md="6" :lg="6">
                  <div class="card-panel-icon-wrapper icon-simCard">
                    <svg-icon icon-class="customer" class-name="card-panel-icon"/>
                  </div>
                </el-col>
                <el-col :xs="18" :sm="18" :md="18" :lg="18">
                  <ul class="illustrateUl">
                    <li>客户管理</li>
                    <li>系统内置 企业管理</li>
                    <li>企业用户 信息管理</li>
                    <li>角色菜单 权限划分</li>
                    <li>企业资费 划分配置</li>
                  </ul>
                </el-col>
              </el-row>&ndash;&gt;
            </el-card>
          </el-col>

          <el-col :xs="24" :sm="8" :md="6" :lg="6" class="card-panel-col"  >
            <el-card :body-style="{ padding: '0px' }" class="my_loginCard">
              &lt;!&ndash;<el-row>
                <el-col :xs="6" :sm="6" :md="6" :lg="6">
                  <div class="card-panel-icon-wrapper icon-simCard">
                    <svg-icon icon-class="api" class-name="card-panel-icon"/>
                  </div>
                </el-col>
                <el-col :xs="18" :sm="18" :md="18" :lg="18">
                 <ul class="illustrateUl">
                    <li>API管理</li>
                    <li>灵活配置 上游API</li>
                    <li>对外开放 系统接口</li>
                    <li>数据同步 算法配置</li>
                    <li>通道同步 数据进度</li>
                  </ul>
                </el-col>
              </el-row>&ndash;&gt;
            </el-card>
          </el-col>

          <el-col :xs="24" :sm="8" :md="6" :lg="6" class="card-panel-col"  >
            <el-card :body-style="{ padding: '0px' }" class="my_loginCard">
              &lt;!&ndash;<el-row>
                <el-col :xs="6" :sm="6" :md="6" :lg="6">
                  <div class="card-panel-icon-wrapper icon-simCard">
                    <svg-icon icon-class="Collection" class-name="card-panel-icon"/>
                  </div>
                </el-col>
                <el-col :xs="18" :sm="18" :md="18" :lg="18">
                  <ul class="illustrateUl">
                    <li>订单管理</li>
                    <li>资费 交易订单</li>
                    <li>预存 充值订单</li>
                    <li>购物 交易订单</li>
                    <li>预存 消费明细</li>
                  </ul>
                </el-col>
              </el-row>&ndash;&gt;
            </el-card>
          </el-col>

        </el-row>

      </div>-->
    </el-main>

        <!--  底部  -->
        <!--<div class="el-login-footer">
          <span>Copyright © 2018-2021 yunze.vip All Rights Reserved.</span>
        </div>-->




  </el-container>

</template>

<script>
import { getCodeImg } from "@/api/login";
import Cookies from "js-cookie";
import { encrypt, decrypt } from '@/utils/jsencrypt'

export default {
  name: "Login",
  data() {
    return {

      codeUrl: "",
      cookiePassword: "",
      loginForm: {
        username: "",
        password: "",
        rememberMe: false,
        code: "",
        uuid: ""
      },
      loginRules: {
        username: [
          { required: true, trigger: "blur", message: "用户名不能为空" }
        ],
        password: [
          { required: true, trigger: "blur", message: "密码不能为空" }
        ],
        code: [{ required: true, trigger: "change", message: "验证码不能为空" }]
      },
      loading: false,
      redirect: undefined
    };
  },
  watch: {
    $route: {
      handler: function(route) {
        this.redirect = route.query && route.query.redirect;
      },
      immediate: true
    }
  },
  created() {
    this.getCode();
    this.getCookie();
  },
  beforeCreate() {//创建之前调用
    document.querySelector('body').setAttribute('style','background-color:#0d2843;');
  },
  beforeDestroy() {//实例销毁之前调用
    document.querySelector('body').setAttribute('style', 'background-color:rgb(255 255 255 / 50%)');
  },
  methods: {
    getCode() {
      getCodeImg().then(res => {
        this.codeUrl = "data:image/gif;base64," + res.img;
        this.loginForm.uuid = res.uuid;
      });
    },
    getCookie() {
      const username = Cookies.get("username");
      const password = Cookies.get("password");
      const rememberMe = Cookies.get('rememberMe');
      this.loginForm = {
        username: username === undefined ? this.loginForm.username : username,
        password: password === undefined ? this.loginForm.password : decrypt(password),
        rememberMe: rememberMe === undefined ? false : Boolean(rememberMe)
      };
      //console.log(this.loginForm);
    },
    handleLogin() {

      this.$refs.loginForm.validate(valid => {
        if (valid) {
          this.loading = true;
          if (this.loginForm.rememberMe) {
            Cookies.set("username", this.loginForm.username, { expires: 30 });
            Cookies.set("password", encrypt(this.loginForm.password), { expires: 30 });
            Cookies.set('rememberMe', this.loginForm.rememberMe, { expires: 30 });
            //console.log(this.loginForm)
          } else {
            Cookies.remove("username");
            Cookies.remove("password");
            Cookies.remove('rememberMe');
          }
          this.$store.dispatch("Login", this.loginForm).then(() => {
            this.$router.push({ path: this.redirect || "/" }).catch(()=>{});
          }).catch(() => {
            this.loading = false;
            this.getCode();
          });
        }
      });
    }
  }
};
</script>

<style scoped rel="stylesheet/scss" lang="scss">
.login {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 100%;
  background-image: url("../assets/images/login-background.jpg");
  background-size: cover;
}
.title {
  margin: 0px auto 30px auto;
  text-align: center;
  color: #57a4c3;
}

.login-form {
  border-radius: 6px;
  background: rgb(0 0 0 / 10%);
  width: 400px;
  padding: 25px 25px 5px 25px;
  .el-input {
    height: 38px;
    input {
      height: 38px;
    }
  }
  .input-icon {
    height: 39px;
    width: 14px;
    margin-left: 2px;
  }
}
.login-tip {
  font-size: 13px;
  text-align: center;
  color: #bfbfbf;
}
.login-code {
  width: 33%;
  height: 38px;
  float: right;
  img {
    cursor: pointer;
    vertical-align: middle;
  }
}
.el-login-footer {
  height: 40px;
  line-height: 40px;
  position: fixed;
  bottom: 0;
  width: 100%;
  text-align: center;
  color: #fff;
  font-family: Arial;
  font-size: 12px;
  letter-spacing: 1px;
}
.login-code-img {
  height: 38px;
}

.carousel {
  background-size: cover;
  min-height: 658px;
  height:80%;
  background-position: 50% 0px;
  background-repeat: no-repeat no-repeat;
}





.carousel_1 {
  background-image: url('../assets/images/carousel_1.jpg');
}

.carousel_2 {
  background-image: url('../assets/images/carousel_2.jpg');
}

.carousel_3 {
  background-image: url('../assets/images/carousel_3.jpg');
}

.carousel_4 {
  background-image: url('../assets/images/carousel_4.jpg');
}
.Mymenu-item{
  float: right!important;
  color: white!important;
}

.Mymenu-item:hover{
  color: #57a4c3!important;
  background-color: transparent!important;
}


.card-panel-icon-wrapper  {
  padding: 10px;
  -webkit-transition: all .38s ease-out;
  transition: all .38s ease-out;
  border-radius: 6px;
  margin-right: 5px;
  font-size: 100px;
}


.my_loginCard{
  border: 1px solid #64bada;
}



.el-card.is-always-shadow{
  box-shadow: 1px 2px 11px 7px #2ea2d4;
  background-color: #0d2843;
  color: white;
}

  .illustrateUl li:first-child{
    font-size: 25px;
  }

ul,li{ list-style:none}
el-menu-item{ list-style:none}

</style>



