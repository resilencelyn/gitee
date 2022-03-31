<script lang="ts" setup>
import { defineProps, getCurrentInstance, onMounted, ref } from 'vue';
import { checkSign } from '@/api/agreements';
import { useRouter } from 'vue-router';
import { IAgreementCheckVo } from '@/api/dto/agreements';


// 获取地址栏中的code
const props = defineProps({
  email: {
    type: String,
    default: '',
  },
});

const router = useRouter();
const { proxy } = getCurrentInstance() as any;
const email = ref<string>('');
const checkLoading = ref<boolean>(false);
const checkSignInfo = ref<IAgreementCheckVo>();

// 检查api
const checkSigns = async (email: string) => {
  try {
    checkLoading.value = true;
    checkSignInfo.value = await checkSign({ email });
    if (checkSignInfo.value.isSign) {
      proxy.$success('已签署贡献者许可协议!');
    } else {
      proxy.$error('未签署贡献者许可协议!');
    }
  } catch (err) {
    proxy.$throw(err, proxy);
  } finally {
    checkLoading.value = false;
  }
};

// 检查
const check = () => {
  router.push({ name: 'check', query: { email: email.value } });
  checkSigns(email.value);
};

// url带有username自动触发
onMounted(() => {
  if (props.email) {
    email.value = props.email;
    checkSigns(props.email);
  }
});

</script>

<template>
  <div
    class="check"
  >
    <div class="check-container">
      <img
        class="check-img"
        src="@/assets/svgs/sign.svg"
      >
      <!-- 检查 -->
      <div
        class="check-content"
      >
        <div class="check-title">
          贡献者许可协议
        </div>
        <div class="check-title-en">
          (Contributor License Agreement)
        </div>
        <div class="contributor-name-label">
          邮箱
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
    </div>
  </div>
</template>

<style lang="less" scoped>
.check {
  margin: 72px 0;
  display: flex;
  // 按钮
  ::v-deep(.el-button) {
    width: 160px;
    height: 44px;
    background: @themeColor;
    border: 1px solid @themeColor;
    border-radius: 4px;
    font-size: 16px;
    font-weight: 800;
    color: #fff;
  }

  .check-container {
    display: flex;
    width: 1080px !important;
    height: 516px;
    margin: auto;
    background: #fff;
    box-sizing: border-box;
    padding: 87px 0 69px 43px;
    border-radius: 10px;

    // 图片
    .check-img {
      width: 556px;
      height: 401px;
    }

    // 检查
    .check-content {
      margin: 41px 0 0 51px;

      .check-title {
        font-size: 24px;
        font-weight: 500;
        color: #042749;
        line-height: 33px;
      }

      .check-title-en {
        color: #385775;
        font-size: 14px;
        line-height: 22px;
        margin-bottom: 40px;
      }

      .contributor-name-label {
        color: #042749;
        font-size: 14px;
        line-height: 20px;
        margin-bottom: 10px;
      }

      .contributor-name {
        margin-bottom: 40px;

        ::v-deep(.el-input__inner) {
          width: 365px;
          height: 40px;
          border-radius: 4px;
          border: 1px solid #B9CFE6;
          text-indent: 4px;
        }
      }
    }
  }
}
</style>