<script lang="ts" setup>
import { defineProps, ref } from 'vue';
import { useRouter } from 'vue-router';

// 获取地址栏中的code
const props = defineProps({
  email: {
    type: String,
    default: '',
  },
});
const router = useRouter();
const email = ref<string>(props.email || '');
const checkLoading = ref<boolean>(false);

// 检查
const check = () => {
  router.push({ name: 'result', query: { email: email.value } });
};

if (props.email) {
  check();
}
</script>
<template>
  <div
    class="check"
  >
    <div
      v-loading="props.email"
      class="check-container"
    >
      <div
        v-show="!props.email"
        class="check-sign"
      >
        <!-- 顶部logo -->
        <div class="logo-container">
          <div class="cla-logo" />
        </div>
        <!-- 检查 -->
        <div
          class="check-content"
        >
          <img
            class="check-img"
            src="@/assets/svgs/sign.svg"
          >
          <div class="check-title">
            贡献者许可协议
          </div>
          <div class="check-title-en">
            (Contributor License Agreement)
          </div>
          <div class="contributor-name">
            <el-input
              v-model="email"
              placeholder="请输入贡献者邮箱"
            />
          </div>
          <el-button
            :loading="checkLoading"
            class="check-btn"
            @click="check"
          >
            检查
          </el-button>
        </div>
        <!-- 检查结果 -->
        <router-view />
      </div>
      <!-- footer -->
      <div
        v-show="!props.email"
        class="footer"
      >
        <a
          class="footer-logo"
          href="https://jianmu.dev/"
        >建木官网</a>
        <span class="line" />
        <a href="https://gitee.com/jianmu-dev/opencla">参与贡献</a>
        <span class="line" />
        <a href="mailto:support@jianmu.dev">联系我们</a>
      </div>
    </div>
  </div>
</template>
<style lang="less" scoped>
.check {
  height: 100vh;

  ::v-deep(.el-dropdown-menu) {
    padding: 0;
    margin: 0;

    .el-dropdown-menu__item {
      padding: 0px;
      width: 263px;
      height: 123px;
      display: flex;
      align-content: center;
      justify-content: center;
      font-size: 30px;
      color: #042749;
    }
  }

  .check-container {
    height: 100vh;

    .check-sign {
      min-height: calc(100vh - 88px);
      //  顶部logo
      .logo-container {
        box-sizing: border-box;
        padding: 34px 30px 34px;
        border-bottom: 1px solid rgba(0, 0, 0, .1);
        display: flex;
        justify-content: space-between;

        .cla-logo {
          width: 240px;
          height: 57px;
          background: url(@logoPath) no-repeat;
          background-size: 100%;
        }

        .user-header {
          width: 48px;
          height: 48px;
          border-radius: 50%;
        }
      }

      // 检查
      .check-content {
        display: flex;
        flex-direction: column;
        align-items: center;

        .check-img {
          width: 325px;
          height: 249px;
          margin: 87px 0 42px 0;
        }

        .check-title {
          color: #012C53;
          font-size: 36px;
          font-weight: 600;
          line-height: 40px;
        }

        .check-title-en {
          color: #385775;
          font-size: 24px;
          line-height: 32px;
          margin-bottom: 80px;
        }

        .contributor-name {
          ::v-deep(.el-input__inner) {
            width: 690px;
            height: 94px;
            border-radius: 10px;
            border: 1px solid #B9CFE6;
            margin-bottom: 60px;
            text-indent: 10px;
          }
        }

        ::v-deep(.el-button) {
          width: 690px;
          height: 94px;
          background: @themeColor;
          border: 1px solid @themeColor;
          border-radius: 10px;
          color: #fff;
          font-size: 30px;
        }
      }
    }

    .footer {
      height: 88px;
      border-top: 1px solid #E3EAF0;
      box-sizing: border-box;
      display: flex;
      align-items: center;
      justify-content: center;
      font-size: 24px;
      color: #385775;

      .line {
        display: inline-block;
        width: 1px;
        height: 20px;
        background: #E3EAF0;
        margin: 0 20px;
      }
    }
  }
}
</style>