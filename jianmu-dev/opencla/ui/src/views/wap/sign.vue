<script lang="ts" setup>
import { useRouter } from 'vue-router';
import { getEnterpriseEmails } from '@/api/agreements';
import { getCurrentInstance, ref } from 'vue';
import { IEnterpriseMailVo } from '@/api/dto/agreements';

const router = useRouter();
const { proxy } = getCurrentInstance() as any;

// 去签署
const toSignCla = () => {
  router.push({ name: 'agreement' });
};
// 签署企业CLA弹窗显隐
const popupVisible = ref<boolean>(false);
// 签署企业CLA
const enterpriseContent = ref<IEnterpriseMailVo>({ enterpriseMail: '' });

//  签署企业CLA
const enterpriseEmail = async () => {
  try {
    popupVisible.value = true;
    enterpriseContent.value = await getEnterpriseEmails();
  } catch (err) {
    proxy.$throw(err, proxy);
  }
};

// 确定
const sure = () => {
  popupVisible.value = false;
};
</script>

<template>
  <div class="sign">
    <div
      class="mobile-sign"
    >
      <!-- 顶部logo -->
      <div class="logo-container">
        <div class="cla-logo" />
      </div>
      <!-- 签署cla -->
      <div
        class="sign-cla"
      >
        <img
          class="sign-img"
          src="@/assets/svgs/sign.svg"
        >
        <div class="sign-title">
          <div class="info-title">
            贡献者许可协议
          </div>
          <div class="info-title-en">
            (Contributor License Agreement)
          </div>
        </div>
        <div class="sign-info">
          感谢您为建木开源项目做出贡献！在参与社区贡献前，您需要签署贡献者许可协议，以确保社区可以自由使用您的贡献。
        </div>
        <van-button
          class="sign-btn"
          @click="toSignCla"
        >
          签署个人CLA
        </van-button>
        <van-button
          class="enterprise"
          @click="enterpriseEmail"
        >
          签署企业CLA
        </van-button>
      </div>
      <!--   签署企业CLA弹窗   -->
      <van-popup
        v-model:show="popupVisible"
        :close-on-click-overlay="false"
        class="enterprise-popup"
      >
        <div
          calss="enterprise-content"
        >
          <jm-markdown
            v-model="enterpriseContent.enterpriseMail"
            :mode="'plaintext'"
          />
        </div>
        <button
          class="sure-btn"
          type="default"
          @click="sure"
        >
          确定
        </button>
      </van-popup>
    </div>
    <div
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
</template>

<style lang="less" scoped>
.sign {
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

  ::v-deep(.van-popup--center) {
    width: 540px;
    height: 356px;
    border-radius: 10px;
    padding: 0;
  }

  .mobile-sign {
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

    //  签署cla
    .sign-cla {
      padding: 0 30px;
      display: flex;
      flex-direction: column;
      align-items: center;

      .sign-img {
        width: 325px;
        height: 249px;
        margin: 61px 0 62px 0;
      }

      .sign-title {
        font-size: 36px;
        text-align: center;

        .info-title {
          height: 40px;
          line-height: 40px;
          font-weight: 600;
          color: #012C53;
        }

        .info-title-en {
          height: 32px;
          line-height: 32px;
          font-size: 24px;
          color: #385775;
        }
      }

      .sign-info {
        width: 690px;
        height: 94px;
        font-size: 24px;
        color: #385775;
        margin: 40px 0 80px 0;
      }

      .sign-btn,
      .enterprise {
        width: 690px;
        height: 94px;
        color: #FFF;
        border-radius: 10px;
        font-size: 36px;
        background: @themeColor;
        //border: 1px solid @themeColor;
        border: 1px solid transparent;
      }

      //  签署企业CLA
      .enterprise {
        background: #FEEEE8;
        color: #FA541C;
        margin-top: 40px;
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

  .sure {
    height: 86px;
    width: 100%;
    color: red;
  }

  .enterprise-content {
  }

  .sure-btn {
    height: 86px;
    width: 100%;
    border: none;
    background: #fff;
    border-top: 1px solid #E9E9E9;
    font-size: 36px;
    color: @themeColor;
  }
}
</style>