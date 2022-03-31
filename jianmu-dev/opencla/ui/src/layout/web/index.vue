<script lang="ts" setup>
import { ref } from 'vue';
import { useRoute } from 'vue-router';

const route = useRoute();

// 通过路由判断是否logo可跳转
const checkFlag = ref<boolean>(route.path.slice(0, 6) !== '/check');
</script>
<template>
  <div class="pc-layout">
    <div class="header">
      <div class="header-content">
        <router-link
          v-if="checkFlag"
          :to="{name:'sign'}"
          class="logo-link"
        >
          <div class="cla-logo" />
        </router-link>
        <div
          v-else
          class="logo-link"
        >
          <div class="cla-logo" />
        </div>
      </div>
      <div class="content">
        <router-view class="pages" />
      </div>
      <div class="footer">
        <a
          href="https://jianmu.dev/"
          target="_blank"
        >建木官网</a>
        <span class="line" />
        <a
          href="https://gitee.com/jianmu-dev/opencla"
          target="_blank"
        >参与贡献</a>
        <span class="line" />
        <a href="mailto:support@jianmu.dev">联系我们</a>
      </div>
    </div>
  </div>
</template>

<style lang="less" scoped>
.pc-layout {
  background: lightskyblue;
  display: flex;
  flex-direction: column;
  align-content: space-between;


  .header {
    background: #FFFFFF;
    box-shadow: 0px 2px 10px 0px #EFF0F3;
    box-sizing: border-box;

    .header-content {
      width: 1300px;
      height: 64px;
      margin: 0 auto;
      display: flex;
      align-items: center;
      justify-content: space-between;

      .logo-link,
      .cla-logo {
        width: 152px;
        height: 30px;
      }

      .cla-logo {
        width: 148px;
        height: 35px;
        background: url(@logoPath) no-repeat;
        background-size: 100%;
      }
    }
  }

  .content {
    min-height: calc(100vh - 128px);
    background: #F1F6FC;
    display: flex;
    justify-content: center;

    .pages {
      width: 1300px;
    }
  }

  .footer {
    display: flex;
    align-items: center;
    justify-content: center;
    height: 64px;
    background: #FFFFFF;
    color: #042749;
    font-size: 14px;

    a {
      display: inline-block;
      height: 25px;
    }

    .line {
      display: inline-block;
      width: 1px;
      height: 14px;
      background: #E3EAF0;
      margin: 0 20px;
    }
  }

}
</style>