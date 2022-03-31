<script lang="ts" setup>
import { defineProps, getCurrentInstance, onMounted, ref } from 'vue';
import { useRouter } from 'vue-router';
import { checkSign } from '@/api/agreements';

const router = useRouter();
const { proxy } = getCurrentInstance() as any;
// 获取地址栏中的code
const props = defineProps({
  email: {
    type: String,
    required: true,
  },
});

// 状态控制
const signFlag = ref<boolean>(false);

onMounted(async () => {
  try {
    const sign = await checkSign({ email: props.email });
    signFlag.value = sign.isSign;
  } catch (err) {
    proxy.$throw(err, proxy);
  }
});
// 确定
const sure = () => {
  router.push({ name: 'check' });
};

</script>
<template>
  <div class="result">
    <div
      class="result-sign"
    >
      <!-- 顶部logo -->
      <div class="logo-container">
        <div class="cla-logo" />
      </div>
      <!-- 检查结果 -->
      <div
        class="result"
      >
        <!-- 签署 -->
        <div
          v-if="signFlag"
          class="success"
        >
          <img
            class="success-img"
            src="@/assets/svgs/success.svg"
          >
          <div class="success-title">
            已签署贡献者许可协议!
          </div>
          <div
            class="sure-btn"
            @click="sure"
          >
            确定
          </div>
        </div>
        <!-- 未签署 -->
        <div
          v-else
          class="fail"
        >
          <img
            class="fail-img"
            src="@/assets/svgs/fail.svg"
          >
          <div class="fail-title">
            未签署贡献者许可协议!
          </div>
          <div
            class="sure-btn"
            @click="sure"
          >
            确定
          </div>
        </div>
      </div>
    </div>
  </div>
</template>
<style lang="less" scoped>
.result {
  .result-sign {
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

    // 检查结果
    .result {
      .sure-btn {
        width: 690px;
        height: 94px;
        line-height: 94px;
        text-align: center;
        background: @themeColor;
        border-radius: 10px;
        font-size: 30px;
        color: #fff;
      }

      // 结果共用样式
      .success, .fail {
        display: flex;
        flex-direction: column;
        align-items: center;

        .success-img, .fail-img {
          width: 120px;
          height: 120px;
          margin: 128px 0 60px 0;
        }

        .success-title, .fail-title {
          color: #042749;
          font-size: 30px;
          font-weight: 500;
          line-height: 42px;
          margin-bottom: 80px;
        }
      }
    }
  }
}
</style>