<template>
  <div class="sms-verification-code">
    <div
      v-if="!isShow"
      class="code-btn"
    >
      <el-button
        :class="{error:!buttonState,success:buttonState}"
        :disabled="isDisabled"
        @click="fetchCode"
      >
        发送验证码
      </el-button>
    </div>
    <div
      v-else
      class="cut-down"
      style="font-size:14px;"
    >
      <span>{{ countDownTime }}</span>秒后重试
    </div>
    <el-dialog
      v-model="visible"
      :append-to-body="true"
      :width="dialogWidth"
      custom-class="sms-verification-code-dialog"
      destroy-on-close
      title="输入图片验证码"
      @close="closeDialog"
    >
      <div class="dialog-content">
        <el-form
          ref="imageCodeRef"
          :model="imageCodeForm"
          :rules="imageCodeRule"
          @submit.prevent
        >
          <el-form-item prop="imageCode">
            <el-input
              v-model="imageCodeForm.imageCode"
              placeholder="请输入图片验证码"
            />
            <div class="code-image">
              <img
                v-if="baseImage"
                :src="baseImage"
              >
            </div>
            <button
              :class="['refresh-btn', refreshLoading ? 'disabled' : '']"
              @click="refreshImageCode"
            />
          </el-form-item>
        </el-form>
      </div>
      <div class="dialog-bottom">
        <div class="operation">
          <el-button
            :loading="loading"
            type="primary"
            @click="confirm"
          >
            确定
          </el-button>
        </div>
      </div>
    </el-dialog>
  </div>
</template>

<script lang="ts">
import { defineComponent, getCurrentInstance, reactive, ref, watch } from 'vue';
import { fetchEmailCode, fetchImageCode } from '@/api/emails';
import { checkSign } from '@/api/agreements';
import { HttpError } from '@/utils/rest/error';
import { IImageCodeForm } from './model';
import { OtherEmailErrorCodeEnum } from '@/api/dto/enumeration';
import { isWap } from '@/utils/ua';

const DEFAULT_TIME = 60;
export default defineComponent({
  emits: ['fetchSuccess', 'emailCheck'],
  props: {
    email: {
      type: String,
      required: true,
    },
    emailFlag: {
      type: String,
      required: true,
    },
  },
  setup(props, { emit }) {
    const { proxy } = getCurrentInstance() as any;
    const isShow = ref<boolean>(false);
    const visible = ref<boolean>(false);
    const isDisabled = ref<boolean>(true);
    const buttonState = ref<boolean>(false);
    const dialogWidth = ref<string>('');
    isWap() ? dialogWidth.value = '90%' : dialogWidth.value = '600px';
    watch(
      () => props.email,
      () => {
        // 邮箱正则验证
        const emailReg = /^\w+([-+.]\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*$/;
        if (emailReg.test(props.email)) {
          // 邮箱格式验证正确
          isDisabled.value = false;
          buttonState.value = true;
        } else {
          isDisabled.value = true;
        }
      },
      {
        immediate: true,
      },
    );
    const countDownTime = ref<number>(DEFAULT_TIME);
    let timer: any = null;
    const loading = ref<boolean>(false);
    const refreshLoading = ref<boolean>(false);
    const baseImage = ref<string>('');
    const imageCodeRef = ref<any>();
    const imageCodeForm = ref<IImageCodeForm>({});
    const imageCodeRule = reactive({
      imageCode: [
        { required: true, message: '图片验证码不能为空', trigger: 'blur' },
      ],
    });
    const refreshImageCode = async () => {
      if (refreshLoading.value) {
        return;
      }
      baseImage.value = '';
      try {
        refreshLoading.value = true;
        const result = await fetchImageCode({
          emailAddress: props.email,
          emailFlag: props.emailFlag,
        });
        baseImage.value = result.base64;
      } catch (err) {
        proxy.$throw(err, proxy);
      } finally {
        refreshLoading.value = false;
      }
    };
    const fetchCode = async () => {
      try {
        const isSign = await checkSign({ email: props.email });
        emit('emailCheck', isSign.isSign);
        if (isSign.isSign) {
          return;
        }
      } catch (err) {
      }
      countDownTime.value = DEFAULT_TIME;

      if (timer) {
        clearInterval(timer);
      }
      timer = setInterval(() => {
        countDownTime.value -= 1;
        if (countDownTime.value === 0) {
          isShow.value = false;
          isDisabled.value = false;
          clearInterval(timer);
          return;
        }
      }, 1000);
      try {
        // 1.调用获取邮箱验证码接口
        isDisabled.value = true;
        await fetchEmailCode({
          emailAddress: props.email,
          emailFlag: props.emailFlag,
        });
        // 获取验证码成功
        emit('fetchSuccess');
        isDisabled.value = false;
        isShow.value = true;
      } catch (err) {
        isShow.value = false;
        isDisabled.value = false;
        if (!(err instanceof HttpError)) {
          proxy.$throw(err, proxy);
          return;
        }
        // 根据不同的错误信息判断
        if (
          !(
            err.response.data.errorCode === OtherEmailErrorCodeEnum.IMAGE_CODE_NOT_EMPTY ||
            err.response.data.errorCode === OtherEmailErrorCodeEnum.CODE_EXCEED_SENDING_MAXIMUM ||
            err.response.data.errorCode === OtherEmailErrorCodeEnum.IMAGE_CODE_REFRESH_FAILED ||
            err.response.data.errorCode === OtherEmailErrorCodeEnum.VALIDATION_EMAIL_CODE_FAILED
          )
        ) {
          proxy.$throw(err, proxy);
          return;
        }
        if (err.response.data.errorCode === OtherEmailErrorCodeEnum.CODE_EXCEED_SENDING_MAXIMUM) {
          proxy.$throw(err, proxy);
          return;
        }
        // 将dialog打开，获取图片信息
        visible.value = true;
        await refreshImageCode();
      }
    };

    const closeDialog = () => {
      visible.value = false;
      imageCodeForm.value.imageCode = '';
    };
    const confirm = async () => {
      // 1.调用获取邮箱验证码接口
      imageCodeRef.value.validate(async (valid: boolean) => {
        if (!valid) {
          return false;
        }
        try {
          loading.value = true;
          isDisabled.value = true;
          await fetchEmailCode({
            emailAddress: props.email,
            emailFlag: props.emailFlag,
            imageCode: imageCodeForm.value.imageCode,
          });
          closeDialog();
          // 关闭弹框重新开始计时
          countDownTime.value = DEFAULT_TIME;
          isShow.value = true;
          emit('fetchSuccess');
        } catch (err) {
          if (!(err instanceof HttpError)) {
            proxy.$throw(err, proxy);
            return;
          }
          if (
            !(
              err.response.data.errorCode === OtherEmailErrorCodeEnum.IMAGE_CODE_NOT_EMPTY ||
              err.response.data.errorCode === OtherEmailErrorCodeEnum.CODE_EXCEED_SENDING_MAXIMUM ||
              err.response.data.errorCode === OtherEmailErrorCodeEnum.IMAGE_CODE_REFRESH_FAILED ||
              err.response.data.errorCode === OtherEmailErrorCodeEnum.VALIDATION_EMAIL_CODE_FAILED
            )
          ) {
            proxy.$throw(err, proxy);
            return;
          }
          proxy.$throw(err, proxy);
          await refreshImageCode();
        } finally {
          isDisabled.value = false;
          loading.value = false;
        }
      });
    };
    return {
      imageCodeRef,
      imageCodeRule,
      imageCodeForm,
      closeDialog,
      refreshImageCode,
      confirm,
      baseImage,
      visible,
      isDisabled,
      isShow,
      fetchCode,
      countDownTime,
      loading,
      refreshLoading,
      buttonState,
      dialogWidth,
    };
  },
});
</script>
<style lang="less" scoped>
.sms-verification-code {
  user-select: none;
  font-size: 14px;
  display: inline-block;

  .code-btn {
    cursor: pointer;
    color: @themeColor;
    height: 40px;

    button.el-button {
      width: 100%;
      height: 100%;
      padding: 0;
      border: none;
      background-color: transparent;
      color: #7B8C9C;

      &:hover {
        background-color: transparent;
      }

      &.is-disabled {
        cursor: not-allowed;
        color: #7B8C9C !important;
        opacity: 0.5;
      }
    }

    .success {
      color: @themeColor !important;
    }

    .error {
      color: #7B8C9C !important;
    }
  }

  .cut-down {
    color: #3f536e;
    cursor: not-allowed;
    opacity: 0.6;

    span {
      text-align: right;
      display: inline-block;
    }
  }
}
</style>
