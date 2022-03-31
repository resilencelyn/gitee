<script lang="ts" setup>
import { computed, defineProps, getCurrentInstance, nextTick, onMounted, onUpdated, ref } from 'vue';
import { useStore } from 'vuex';
import { ThirdPartyTypeEnum } from '@/api/dto/enumeration';
import { IAuthorizationUrlVo } from '@/api/dto/auth';
import { getAuthUrl } from '@/api/auth';
import { authEmailSign, getEmailList, otherEmailSign, seeCla } from '@/api/agreements';
// @ts-ignore
import { IAgreementVo } from '@/api/dto/agreements';
import { ILoggingDtoParams, IState as ISessionState } from '@/model/modules/session';
import { namespace as sessionNameSpace } from '@/store/modules/session';
import { IAuthorizationEmailList } from '@/model/modules/emails';
import EmailVerificationCode from '@/views/common/email-verification-code/index.vue';
import { getRedirectUri } from '@/utils/redirect-uri';

// 获取地址栏中的code
const props = defineProps({
  code: {
    type: String,
    default: '',
  },
  errorDescription: {
    type: String,
    default: '',
  },
  email: {
    type: String,
    default: '',
  },
  returnUrl: {
    type: String,
    default: '',
  },
});

const store = useStore();
const { proxy } = getCurrentInstance() as any;

// 获取token
const sessionToken = computed<ISessionState>(() => store.state[sessionNameSpace]);
// 获取邮箱信息
const authEmailList = ref<IAuthorizationEmailList>([{ email: '', labels: [''], isSigned: false }]);
// 签署按钮loading
const signLoading = ref<boolean>(false);
// 协议返回信息
const claContent = ref<IAgreementVo>({ agreementContent: '', projectName: '' });
// 是否勾选同意
const agreeFlag = ref<boolean>(false);
// cla页面loading
const pageLoading = ref<boolean>(false);
// 单选默认值
const radio = ref<string>('');
// 当前选中的值
const radioCurrentValue = ref<string>('');
// 授权url
const authUrl = ref<IAuthorizationUrlVo>();
// 选择邮箱单选flag
const radioEmailFlag = ref<boolean>(false);
// 邮箱内容显隐
const emailContentVisible = ref<boolean>(false);
// 其他验证-邮箱
const otherEmail = ref<string>('');
// 其他验证-验证码
const otherCode = ref<string>('');
// 选择的邮箱
const selectEmailValue = ref<string>('');
// 签署按钮
const signDisable = ref<boolean>(true);
// 校验邮箱
const checkEmailFlag = ref<boolean>(false);
// 签署信息隐藏
const signInfoVisible = ref<boolean>(true);
// 判断是否重新排序
const authEmailFlag = ref<boolean>(true);
const contentVisible = ref<boolean>(true);

// 获取到邮箱
// 切换验证方式
const verificationMethodVisible = ref<boolean>(true);
// 平台切换
const platformVisible = ref<boolean>(false);
// 平台邮箱
const getPlatEmailVal = ref<string>('');
// 其他邮箱
const getOtherEmailVal = ref<{ email: string; code: string }>({
  email: '',
  code: '',
});
// 获取token的参数
const tokenParam = ref<ILoggingDtoParams>({
  code: props.code,
  thirdPartyType: ThirdPartyTypeEnum.GITEE,
  redirectUri: getRedirectUri(props.email, props.returnUrl),
});

// 封装获取授权authUrl
const getAuth = async () => {
  try {
    authUrl.value = await getAuthUrl({
      thirdPartyType: ThirdPartyTypeEnum.GITEE,
      redirectUri: getRedirectUri(props.email, props.returnUrl),
    });
    location.href = authUrl.value.authorizationUrl;
  } catch (err) {
    proxy.$throw(err, proxy);
  }
};

// 封装授权邮箱判断
const authEmail = () => {
  authEmailList.value.forEach(item => {
    if (item.labels.indexOf('提交邮箱') !== -1 && !item.isSigned) {
      selectEmailValue.value = item.email;
    }
  });
  if (!selectEmailValue.value) {
    authEmailList.value.forEach(item => {
      if (item.labels.indexOf('主邮箱') !== -1 && !item.isSigned) {
        selectEmailValue.value = item.email;
      }
    });
  }
  if (!selectEmailValue.value) {
    let statusArray = ref<boolean[]>([]);
    authEmailList.value.forEach(item => {
      statusArray.value.push(item.isSigned);
    });
    if (statusArray.value.indexOf(false) === -1) {
      selectEmailValue.value = '';
      return;
    }
    selectEmailValue.value = authEmailList.value[statusArray.value.indexOf(false)].email;
  }
};

// 封装获取邮箱列表
const emailsList = async () => {
  try {
    authEmailList.value = await getEmailList({ thirdPartyType: ThirdPartyTypeEnum.GITEE });
  } catch (err) {
    proxy.$throw(err, proxy);
  }
};

onUpdated(async () => {
  // 能获取到邮箱
  if (!verificationMethodVisible.value) {
    if (platformVisible.value) {
      signDisable.value = !(getPlatEmailVal.value && agreeFlag.value) as boolean;
    } else {
      signDisable.value = !(getOtherEmailVal.value.email && getOtherEmailVal.value.code && agreeFlag.value) as boolean;
    }
  } else {
    // 签署按钮禁用启用
    if (radio.value === ThirdPartyTypeEnum.GITEE) {
      signDisable.value = !(selectEmailValue.value && selectEmailValue.value !== '' && agreeFlag.value);
    } else {
      signDisable.value = !(otherEmail.value && otherCode.value && agreeFlag.value);
    }
  }
  emailContentVisible.value = (radio.value === 'other' || (radio.value === ThirdPartyTypeEnum.GITEE && authEmailList.value[0].email)) as boolean;
  // 优化onUpdated值更新前后重复请求的问题
  if (radioCurrentValue.value === radio.value) {
    return;
  }
  radioCurrentValue.value = radio.value;

  // 验证方式
  if (radioCurrentValue.value === ThirdPartyTypeEnum.GITEE) {
    if (!props.code) {
      await getAuth();
    }
    if (props.code && !sessionToken.value.session?.token) {
      await getAuth();
    } else {
      if (!authEmailList.value[0].email) {
        await emailsList();
        authEmail();
      }
      radio.value = ThirdPartyTypeEnum.GITEE;
      radioEmailFlag.value = true;
    }
  } else {
    // 其他邮箱
    radioEmailFlag.value = false;
  }
});

// 定位到底部
const positionBottom = async () => {
  await nextTick();
  document.documentElement.scrollTop = document.body.scrollHeight;
};

// 获取协议内容
const getAgreementContent = async () => {
  // 初始化协议内容
  try {
    signInfoVisible.value = false;
    pageLoading.value = true;
    claContent.value = await seeCla();
  } catch (err) {
    proxy.$throw(err, proxy);
  } finally {
    pageLoading.value = false;
    signInfoVisible.value = true;
  }
};
// 页面初始化-获取协议内容
onMounted(async () => {
  try {
    // 获取协议内容
    await getAgreementContent();
    if (props.code && !props.email) {
      // 获取token
      tokenParam.value.redirectUri = getRedirectUri(props.email, props.returnUrl);
      await store.dispatch('session/create', tokenParam.value);
      await emailsList();
      authEmail();
      radio.value = ThirdPartyTypeEnum.GITEE;
      radioEmailFlag.value = true;
      // 定位到底部
      await positionBottom();
    } else if (props.code && props.email) {
      // 获取token
      tokenParam.value.redirectUri = getRedirectUri(props.email, props.returnUrl);
      await store.dispatch('session/create', tokenParam.value);
      // 从gitee授权回来
      getPlatEmailVal.value = props.email;
    }
  } catch (err) {
    proxy.$throw(err, proxy);
    verificationMethodVisible.value = true;
  }
});

// 签署成功判断是否跳转
const getReturnUrl = () => {
  if (props.returnUrl) {
    proxy.$success('签署成功，即将跳转...');
    setTimeout(() => {
      location.href = props.returnUrl;
    }, 3000);
    return;
  }
  proxy.$success('签署成功');
};
// 封装平台邮箱签署
const signPlatformEmail = async (platformEmail: string) => {
  try {
    await authEmailSign({ thirdPartyType: ThirdPartyTypeEnum.GITEE, email: platformEmail });
    signInfoVisible.value = false;
    getReturnUrl();
  } catch (err) {
    proxy.$throw(err, proxy);
  }
};

// 封装其他邮箱签署
const signOtherEmail = async (otherEmail: string, OtherCode: string) => {
  try {
    await otherEmailSign({ email: otherEmail, checkCode: OtherCode, emailFlag: 'check' });
    signInfoVisible.value = false;
    getReturnUrl();
  } catch (err) {
    proxy.$throw(err, proxy);
  }
};

// 签署
const toSign = async () => {
  // 能获取到邮箱
  if (!verificationMethodVisible.value) {
    signLoading.value = true;
    try {
      if (platformVisible.value) {
        await signPlatformEmail(getPlatEmailVal.value);
      } else {
        await signOtherEmail(getOtherEmailVal.value.email, getOtherEmailVal.value.code);
      }
    } catch (err) {
      proxy.$throw(err, proxy);
    } finally {
      signLoading.value = false;
    }
  } else {
    signLoading.value = true;
    try {
      if (radio.value === ThirdPartyTypeEnum.GITEE) {
        await signPlatformEmail(selectEmailValue.value);
      } else {
        await signOtherEmail(otherEmail.value, otherCode.value);
      }
    } catch (err) {
      proxy.$throw(err, proxy);
    } finally {
      signLoading.value = false;
    }
  }
};

// 检查是否签署/其他邮箱验证
const emailCheck = (sign: boolean) => {
  if (sign) {
    checkEmailFlag.value = true;
  }
};

// 清空邮箱
const clearOtherEmail = () => {
  otherEmail.value = '';
  checkEmailFlag.value = false;
};

// 关闭下拉
const closeSelect = () => {
  authEmailFlag.value = false;
};


// 页面初始判断
const pageInit = async () => {
  if (props.email) {
    // gitee邮箱
    if (props.email.endsWith('@user.noreply.gitee.com')) {
      if (!props.code) {
        contentVisible.value = false;
        await getAuth();
      } else {
        verificationMethodVisible.value = false;
        platformVisible.value = true;
        getPlatEmailVal.value = props.email;
      }

    } else {
      // 其他邮箱
      verificationMethodVisible.value = false;
      platformVisible.value = false;
      getOtherEmailVal.value.email = props.email as string;
    }
  } else {
    // props中没有邮箱
    if (props.code && props.email) {
      verificationMethodVisible.value = false;
      platformVisible.value = true;
      getPlatEmailVal.value = props.email as string;
    } else if (props.errorDescription) {
      proxy.$error(props.errorDescription);
      return;
    }
  }
};
pageInit();
</script>

<template>
  <div
    v-loading="pageLoading"
    class="agreement"
  >
    <div
      v-if="contentVisible"
      class="agreement-container"
    >
      <!--  协议标题  -->
      <div
        v-if="claContent.projectName"
        class="agreement-title"
      >
        <div class="title">
          {{ claContent.projectName }}开源项目个人贡献者许可协议
        </div>
      </div>
      <!--  协议内容  -->
      <div class="agreement-content">
        <div class="markdown">
          <jm-markdown
            v-model="claContent.agreementContent"
            :mode="'readonly'"
          />
        </div>
        <!--  签署方式-邮箱选择 -->
        <div
          v-if="signInfoVisible"
          class="sign-info"
        >
          <!--  选择验证方式  -->
          <div
            v-if="verificationMethodVisible"
            class="select-verification-method"
          >
            <!--  选择验证方式内容 -->
            <div class="select-verification">
              <div class="verification-title">
                请选择验证方式
              </div>
              <div class="verification-info">
                注：邮箱必须为提交代码的邮箱，否则将无法通过CLA验证。如果您打算使用多个邮箱提交代码，请分别用它们签署CLA协议。
              </div>
              <el-radio-group v-model="radio">
                <el-radio label="GITEE">
                  gitee邮箱验证
                </el-radio>
                <el-radio label="other">
                  其他邮箱验证
                </el-radio>
              </el-radio-group>
            </div>
            <!--  邮箱内容  -->
            <div
              v-if="emailContentVisible"
              class="email-content"
            >
              <!-- 授权邮箱验证邮箱选择 -->
              <div
                v-if="radioEmailFlag"
                class="auth-email"
              >
                <span class="select-label">邮箱</span>
                <el-select
                  v-model="selectEmailValue"
                  placeholder="Select"
                  @blur="closeSelect"
                >
                  <el-option
                    disabled
                    label="置灰邮箱为已签署协议邮箱，无须重复签署"
                    value="置灰邮箱为已签署协议邮箱，无须重复签署"
                  />
                  <el-option
                    v-show="false"
                    disabled
                    label="无可签署邮箱"
                    value=""
                  />
                  <el-option
                    v-for="item in authEmailList"
                    :key="item.email"
                    :disabled="item.isSigned"
                    :label="item.email"
                    :value="item.email"
                  >
                    <template #default>
                      {{ item.email }}
                      <el-tag
                        v-if="item.labels.indexOf('主邮箱') !== -1"
                        class="ml-2"
                        effect="plain"
                        type="success"
                      >
                        {{ item.labels[item.labels.indexOf('主邮箱')] }}
                      </el-tag>
                      <el-tag
                        v-if="item.labels.indexOf('提交邮箱') !== -1"
                        class="ml-2"
                        effect="plain"
                        type="warning"
                      >
                        {{ item.labels[item.labels.indexOf('提交邮箱')] }}
                      </el-tag>
                    </template>
                  </el-option>
                </el-select>
              </div>
              <!--  其他邮箱  -->
              <div
                v-if="!radioEmailFlag"
                class="other-email-container"
              >
                <div class="other-email">
                  <div class="label">
                    邮箱
                  </div>
                  <el-input
                    v-model="otherEmail"
                    placeholder="请输入邮箱"
                    @input="checkEmailFlag = false"
                  />
                  <span
                    v-if="checkEmailFlag"
                    class="signed"
                  >
                    该邮箱为已签署协议邮箱，无须重复签署
                  </span>
                  <img
                    v-show="otherEmail"
                    class="clear-email"
                    src="@/assets/svgs/clear.svg"
                    @click="clearOtherEmail"
                  >
                </div>
                <div class="verification-code">
                  <div class="label">
                    验证码
                  </div>
                  <el-input
                    v-model="otherCode"
                    maxlength="6"
                    placeholder="请输入验证码"
                  />
                  <!--  向邮箱发送验证码  -->
                  <div class="email-verification">
                    <email-verification-code
                      :email="otherEmail"
                      class="email-verification-code"
                      email-flag="check"
                      @email-check="emailCheck"
                    />
                  </div>
                </div>
              </div>
            </div>
          </div>
          <!--  获取邮箱判断签署方式  -->
          <div
            v-else
            class="get-sign-method"
          >
            <!--  平台授权邮箱  -->
            <div
              v-if="platformVisible"
              class="platform-auth-email"
            >
              <span class="platform-label">邮箱</span>
              <!--  getPlatformEmail -->
              <el-input
                v-model="getPlatEmailVal"
                disabled
              />
            </div>
            <!--  其他邮箱     -->
            <div
              v-else
              class="get-other-emails"
            >
              <div class="get-other-email">
                <span class="label">邮箱</span>
                <!--  getOtherEmail  -->
                <el-input
                  v-model="getOtherEmailVal.email"
                  disabled
                />
                <span
                  v-if="checkEmailFlag"
                  class="signed"
                >
                  该邮箱为已签署协议邮箱，无须重复签署
                </span>
              </div>
              <div class="get-other-code">
                <span class="label">验证码</span>
                <el-input
                  v-model="getOtherEmailVal.code"
                  maxlength="6"
                />
                <!--  向邮箱发送验证码  -->
                <div class="email-verification">
                  <email-verification-code
                    :email="getOtherEmailVal.email"
                    class="email-verification-code"
                    email-flag="check"
                    @email-check="emailCheck"
                  />
                </div>
              </div>
            </div>
          </div>
          <!--  勾选协议  -->
          <div class="agree">
            <div>
              <el-checkbox
                ref="buttonRef"
                v-model="agreeFlag"
              />
            </div>
            <span
              class="read"
              @click="agreeFlag = !agreeFlag"
            >我已阅读并同意上述个人贡献者许可协议的条款。</span>
          </div>
          <!--  签署按钮  -->
          <div class="sign-btn-container">
            <el-button
              :disabled="signDisable"
              :loading="signLoading"
              type="primary"
              @click="toSign"
            >
              签署
            </el-button>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<style lang="less" scoped>
.agreement {
  width: 1300px;
  min-height: calc(100vh - 184px);
  background: #FFF;
  margin: 30px auto 26px;

  .agreement-container {
    .agreement-title {
      padding: 0 30px;

      .title {
        text-align: center;
        padding: 60px 0 30px 0;
        color: #042749;
        font-size: 30px;
        font-weight: 500;
        border-bottom: 1px solid #E7ECF1;
      }
    }

    .agreement-content {
      width: 1210px;
      padding-top: 30px;
      margin: 0 auto;
      color: #042749;
      font-size: 14px;
      line-height: 22px;
      // 避免外边距塌陷
      border-bottom: 1px solid transparent;

      .markdown {
        margin-bottom: 30px;
      }

      //  签署信息
      .sign-info {
        border-top: 1px solid #E7ECF1;
        font-size: 14px;

        // 选择验证方式
        .select-verification-method {
          // 选择验证方式内容
          .select-verification {
            margin-top: 30px;

            .verification-title {
              font-weight: 600;
              color: #042749;
              line-height: 20px;
              margin-bottom: 14px;
            }

            .verification-info {
              width: 784px;
              height: 20px;
              font-weight: 400;
              color: #7B8C9C;
              line-height: 20px;
              margin-bottom: 20px;
            }

            ::v-deep(.el-radio-group) {
              height: 20px;

              .el-radio {
                margin-right: 0px;
                color: #042749;
              }

              .el-radio:first-child {
                margin-right: 60px;
              }
            }
          }

          .email-content {
            // 授权邮箱邮箱
            .auth-email {
              margin-top: 20px;

              .select-label {
                margin-right: 14px;
              }

              .el-select {
                width: 548px;
                height: 40px;
              }

              // 取消下拉框自带箭头/调整间距
              ::v-deep(.el-popper) {
                top: 346px;
              }

              ::v-deep(.el-input__inner) {
                color: #042749;
              }
            }

            // 其他邮箱
            .other-email-container {
              .el-input {
                width: 548px;
              }

              ::v-deep(.el-input__inner) {
                width: 548px;
                height: 40px;
                padding-right: 50px;
                color: #042749;
              }

              .other-email,
              .verification-code {
                display: flex;
                align-items: center;

                .label {
                  text-align: right;
                  width: 48px;
                  margin-right: 14px;
                }
              }

              // 邮箱校验
              .other-email {
                position: relative;
                margin: 20px 0px;

                .clear-email {
                  width: 16px;
                  height: 16px;
                  position: absolute;
                  top: 12px;
                  left: 585px;
                  cursor: pointer;
                }

                .signed {
                  color: #E02020;
                  font-size: 10px;
                  position: absolute;
                  left: 59px;
                  bottom: -20px;
                }
              }

              // 验证码
              .verification-code {
                position: relative;

                .email-verification {
                  position: absolute;
                  left: 521px;
                }
              }
            }
          }
        }

        // 获取邮箱判断签署方式
        .get-sign-method {
          margin-top: 30px;
          // 平台授权邮箱
          .platform-auth-email {
            display: flex;
            align-items: center;

            .platform-label {
              width: 48px;
              margin-right: 17px;
              text-align: right;
            }

            ::v-deep(.el-input__inner) {
              width: 548px;
              height: 40px;
            }
          }

          // 其他邮箱
          .get-other-emails {
            // 邮箱
            .get-other-email,
            .get-other-code {
              display: flex;
              align-items: center;
              position: relative;

              .label {
                width: 48px;
                text-align: right;
                margin-right: 17px;
              }

              // 已签署的提示
              .signed {
                color: #E02020;
                font-size: 10px;
                position: absolute;
                left: 62px;
                top: 38px;
              }
            }

            .get-other-code {
              margin-top: 20px;
              position: relative;
            }
          ;

            ::v-deep(.el-input__inner) {
              width: 548px;
              height: 40px;
            }

            // 验证码
            .email-verification {
              position: absolute;
              left: 520px;
            }
          }
        }

        // 勾选协议
        .agree {
          display: flex;
          align-items: center;
          color: #042749;
          font-weight: 400;
          line-height: 20px;
          margin: 30px 0 20px 0;

          ::v-deep(.el-checkbox) {
            margin-right: 6px;
            height: 5px;
            margin-top: 8px;
          }

          .read {
            cursor: pointer;
          }
        }

        // 签署按钮
        .sign-btn-container {
          text-align: center;

          ::v-deep(.el-button) {
            width: 160px;
            height: 44px;
            font-size: 16px;
            font-weight: 500;
            margin-bottom: 62px;
          }
        }
      }
    }
  }
}

::v-deep(.is-dark) {
  opacity: 0.8;
}
</style>