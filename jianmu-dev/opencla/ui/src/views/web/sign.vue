<script lang="ts" setup>
import { useRouter } from 'vue-router';
import { getEnterpriseEmails } from '@/api/agreements';
import { getCurrentInstance, ref } from 'vue';
import { IEnterpriseMailVo } from '@/api/dto/agreements';


const router = useRouter();
const { proxy } = getCurrentInstance() as any;
// 企业邮箱内容
const enterpriseEmail = ref<IEnterpriseMailVo>({ enterpriseMail: '' });
const dialogVisible = ref<boolean>(false);
const enterpriseLoading = ref<boolean>(false);
// 去签署
const toSignCla = () => {
  router.push({ name: 'agreement' });
};

// 企业签署cla
const enterprise = async () => {
  try {
    enterpriseLoading.value = true;
    dialogVisible.value = true;
    enterpriseEmail.value = await getEnterpriseEmails();
  } catch (err) {
    proxy.$throw(err, proxy);
  } finally {
    enterpriseLoading.value = false;
  }
};
</script>

<template>
  <div
    class="sign"
  >
    <div class="sign-content">
      <img
        class="sign-img"
        src="@/assets/svgs/sign.svg"
      >
      <!--  签署CLA  -->
      <div
        class="sign-info"
      >
        <div class="title">
          <div class="info-title">
            贡献者许可协议
          </div>
          <div class="info-title-en">
            (Contributor License Agreement)
          </div>
        </div>
        <div class="content">
          感谢您为建木开源项目做出贡献！在参与社区贡献前，您需要签署贡献者许可协议，以确保社区可以自由使用您的贡献。
        </div>
        <el-button
          class="sign-btn"
          @click="toSignCla"
        >
          签署个人CLA
        </el-button>
        <el-button
          :loading="enterpriseLoading"
          class="enterprise"
          @click="enterprise"
        >
          签署企业CLA
        </el-button>
      </div>
    </div>
    <!--  企业签署cla弹窗  -->
    <el-dialog
      v-model="dialogVisible"
      :close-on-click-modal="false"
      :show-close="false"
      top="15%"
    >
      <template #title>
        <div
          class="dialog-title"
        >
          <i class="title-icons" />
          签署企业CLA
        </div>
      </template>
      <jm-markdown
        v-model="enterpriseEmail.enterpriseMail"
        :mode="'readonly'"
      />
      <template #footer>
        <span class="dialog-footer">
          <el-button
            class="sure"
            type="primary"
            @click="dialogVisible = false"
          >确定</el-button>
        </span>
      </template>
    </el-dialog>
  </div>
</template>

<style lang="less" scoped>
.sign {
  margin: 72px 0;
  display: flex;
  // 按钮
  ::v-deep(.el-button) {
    width: 160px;
    height: 44px;
    background: @themeColor;
    border-radius: 4px;
    font-size: 16px;
    border: none;
    color: #fff;
  }

  ::v-deep(.el-dialog) {
    width: 389px;
    height: 239px;

    .el-dialog__body {
      padding: 0 40px 0 54px;
    }

    .el-button {
      width: 76px;
      height: 36px;
      font-size: 14px;
    }

    //  标题
    .dialog-title {
      display: flex;
      align-items: center;
      font-size: 18px;

      .title-icons {
        width: 24px;
        height: 24px;
        margin-right: 10px;
        background: url('@/assets/svgs/warning.svg');
      }
    }
  }

  .sign-content {
    display: flex;
    width: 1080px !important;
    height: 516px;
    margin: auto;
    background: #fff;
    box-sizing: border-box;
    padding: 87px 0 69px 43px;
    border-radius: 10px;

    // 签署图片
    .sign-img {
      width: 556px;
      height: 401px;
    }

    // 签署CLA
    .sign-info {
      margin-left: 51px;
      // 标题
      .title {
        margin: 60px 0 42px 0;

        .info-title {
          font-size: 24px;
          color: #042749;
          font-weight: 500;
          line-height: 33px;
        }

        .info-title-en {
          font-size: 14px;
          color: #385775;
        }
      }

      // 内容信息
      .content {
        width: 380px;
        font-size: 16px;
        color: #042749;
        line-height: 24px;
        margin-bottom: 40px;
      }

      // 企业签署按钮
      .enterprise {
        background: #FEEEE8;
        color: @themeColor;
        margin-left: 28px;
      }

    }
  }
}
</style>