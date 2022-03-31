<script lang="ts" setup>
import { computed, defineProps, getCurrentInstance, nextTick, onMounted, onUpdated, ref } from 'vue';
import { useStore } from 'vuex';
import { namespace as sessionNameSpace } from '@/store/modules/session';
import { ILoggingDtoParams, IState as ISessionState } from '@/model/modules/session';
import { ThirdPartyTypeEnum } from '@/api/dto/enumeration';
import { IAgreementVo } from '@/api/dto/agreements';
import { getAuthUrl } from '@/api/auth';
import { authEmailSign, getEmailList, otherEmailSign, seeCla } from '@/api/agreements';
// @ts-ignore
import { IAuthorizationUrlVo } from '@/api/dto/auth';
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
// 复选框状态
const mobileCheckbox = ref<boolean>(false);
// 页面loading
const pageLoading = ref<boolean>(false);
// 协议返回信息
const claContent = ref<IAgreementVo>({ agreementContent: '', projectName: '' });
// 授权邮箱方式
const radioAuthEmail = ref<string>('');
// 当前选中的值
const radioCurrentValue = ref<string>('');
// 弹出层邮箱选择
const selectPopupEmail = ref<string>('');
// 其他方式-邮箱
const otherEmail = ref<string>('');
// 其他方式-验证码
const otherCode = ref<string>('');
// 选择邮箱单选flag
const radioEmailVisible = ref<boolean>(false);
// 邮箱内容显隐
const emailContentVisible = ref<boolean>(false);
// 选择授权邮箱
const selectEmail = ref<boolean>(false);
// 授权邮箱值
const authEmailValue = ref<string>('');
// 授权url
const authUrl = ref<IAuthorizationUrlVo>();
// 按钮禁用和启用
const signDisable = ref<boolean>(true);
// 选择邮箱列表标题切换
const selectEmailTitleFlag = ref<boolean>(false);
// 选择邮箱默认选项
const currentRadio = ref<string>('0');
// 监听选择签署方式
const verificationRef = ref<HTMLDivElement>();
// 签署提示的显隐
const readVisible = ref<boolean>(true);
// 底部显隐
const bottomVisible = ref<boolean>(false);
// 清除输入框-邮箱
const emailVisible = ref<boolean>(false);
// 邮箱参数
// 获取token的参数
const tokenParam = ref<ILoggingDtoParams>({
  code: props.code,
  thirdPartyType: ThirdPartyTypeEnum.GITEE,
  redirectUri: getRedirectUri(props.email, props.returnUrl),
});
const authEmailFlag = ref<boolean>(true);
// 获取到邮箱
// 切换验证方式
const verificationMethodVisible = ref<boolean>(true);
// 平台切换
const platformVisible = ref<boolean>(false);
// 平台邮箱
const getPlatEmailVal = ref<string>('');
// 获取其他邮箱
const getOtherEmailVal = ref<{ email: string; code: string }>({
  email: '',
  code: '',
});
const contentVisible = ref<boolean>(true);
// 封装获取授权url
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
  let emailArr = ref<string[]>([]);
  authEmailList.value.forEach(item => {
    emailArr.value.push(item.email);
  });
  authEmailList.value.forEach(item => {
    if (item.labels.indexOf('提交邮箱') !== -1 && !item.isSigned) {
      authEmailValue.value = item.email;
      selectPopupEmail.value = item.email;
      currentRadio.value = emailArr.value.indexOf(item.email).toString();
    }
  });
  if (!authEmailValue.value) {
    authEmailList.value.forEach(item => {
      if (item.labels.indexOf('主邮箱') !== -1 && !item.isSigned) {
        authEmailValue.value = item.email;
        selectPopupEmail.value = item.email;
        currentRadio.value = emailArr.value.indexOf(item.email).toString();
      }
    });
  }
  if (!authEmailValue.value) {
    let statusArray = ref<boolean[]>([]);
    authEmailList.value.forEach(item => {
      statusArray.value.push(item.isSigned);
    });
    // 无可签署邮箱
    if (statusArray.value.indexOf(false) === -1) {
      authEmailValue.value = '';
      selectEmailTitleFlag.value = true;
      return;
    }
    // 邮箱顺延
    authEmailValue.value = authEmailList.value[statusArray.value.indexOf(false)].email;
    selectPopupEmail.value = authEmailList.value[statusArray.value.indexOf(false)].email;
    currentRadio.value = statusArray.value.indexOf(false).toString();
  }
};

// 监听vant输入框值更新
const updateInput = () => {
  signDisable.value = !(radioAuthEmail.value === 'other' && otherEmail.value && otherCode.value);
};
// 获取邮箱列表封装
const emailLists = async () => {
  try {
    authEmailList.value = await getEmailList({ thirdPartyType: ThirdPartyTypeEnum.GITEE });
  } catch (err) {
    proxy.$throw(err, proxy);
  }
};
// 页面更新触发判断
onUpdated(async () => {
  // 签署按钮禁用启用
  if (!verificationMethodVisible.value) {
    if (platformVisible.value) {
      signDisable.value = !getPlatEmailVal.value as boolean;
    } else {
      signDisable.value = !(getOtherEmailVal.value.email && getOtherEmailVal.value.code) as boolean;
    }
  } else {
    if (radioAuthEmail.value === ThirdPartyTypeEnum.GITEE) {
      signDisable.value = !(authEmailValue.value !== '' && authEmailList.value[0].email);
    } else {
      signDisable.value = !(otherEmail.value && otherCode.value);
    }
  }
  emailContentVisible.value = (radioAuthEmail.value === 'other' || authEmailList.value[0].email && radioAuthEmail.value === ThirdPartyTypeEnum.GITEE) as boolean;
  // 判断清除按钮显隐
  emailVisible.value = otherEmail.value !== '';
  // 优化onUpdated值更新前后重复请求的问题
  if (radioCurrentValue.value === radioAuthEmail.value) {
    return;
  }
  radioCurrentValue.value = radioAuthEmail.value;

  // gitee邮箱
  if (radioCurrentValue.value === ThirdPartyTypeEnum.GITEE) {
    if (!props.code) {
      await getAuth();
    } else if (props.code && !sessionToken.value.session?.token) {
      await getAuth();
    } else {
      if (!authEmailList.value[0].email) {
        await emailLists();
        authEmail();
      }
      radioAuthEmail.value = ThirdPartyTypeEnum.GITEE;
      radioEmailVisible.value = true;
    }
  } else {
    if (!otherEmail.value && !otherCode.value) {
      signDisable.value = true;
    }
    radioEmailVisible.value = false;
  }
});

// 定位到底部
const positionBottom = async () => {
  await nextTick();
  readVisible.value = false;
  document.documentElement.scrollTop = document.body.scrollHeight;
};
// 获取协议内容
const getAgreementContent = async () => {
  // 获取协议内容
  try {
    pageLoading.value = true;
    claContent.value = await seeCla();
    bottomVisible.value = true;
  } catch (err) {
    proxy.$throw(err, proxy);
  } finally {
    pageLoading.value = false;
  }
};
// 页面初始化-获取协议信息
onMounted(async () => {
  // 监听滚动-提示按钮的隐藏
  window.addEventListener('scroll', function () {
    if (verificationRef.value!.getBoundingClientRect().top < 590) {
      readVisible.value = false;
    }
  });
  try {
    // 协议内容初始化
    await getAgreementContent();
    if (props.code && !props.email) {
      await store.dispatch('session/create', tokenParam.value);
      await emailLists();
      authEmail();
      radioAuthEmail.value = ThirdPartyTypeEnum.GITEE;
      await positionBottom();
    } else if (props.code && props.email) {
      await store.dispatch('session/create', tokenParam.value);
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
    proxy.$toast('签署成功，即将跳转...');
    setTimeout(() => {
      location.href = props.returnUrl;
    }, 3000);
    return;
  }
  proxy.$toast('签署成功');
};
// 平台邮箱签署封装
const signPlatEmail = async (platEmail: string) => {
  try {
    await authEmailSign({ thirdPartyType: ThirdPartyTypeEnum.GITEE, email: platEmail });
    await emailLists();
    bottomVisible.value = false;
    getReturnUrl();
  } catch (err) {
    proxy.$throw(err, proxy);
  }
};

// 其他邮箱签署封装
const signOtherEmail = async (otherEmail: string, otherCode: string) => {
  try {
    await otherEmailSign({ email: otherEmail, checkCode: otherCode, emailFlag: 'check' });
    bottomVisible.value = false;
    getReturnUrl();
  } catch (err) {
    proxy.$throw(err, proxy);
  }
};

// 签署
const toSign = async () => {
  if (!mobileCheckbox.value) {
    proxy.$toast('请先勾选同意个人贡献者许可协议');
    return;
  }
  if (!verificationMethodVisible.value) {
    if (platformVisible.value) {
      await signPlatEmail(getPlatEmailVal.value);
    } else {
      await signOtherEmail(getOtherEmailVal.value.email, getOtherEmailVal.value.code);
    }
  } else {
    if (radioAuthEmail.value === ThirdPartyTypeEnum.GITEE) {
      await signPlatEmail(authEmailValue.value);
      return;
    }
    if (radioAuthEmail.value === 'other') {
      await signOtherEmail(otherEmail.value, otherCode.value);
    }
  }
};
// 当前选中邮箱
const currentEmail = (email: string) => {
  selectPopupEmail.value = email;
  for (let key in authEmailList.value) {
    if (authEmailList.value[key].email === email) {
      currentRadio.value = key;
    }
  }
};
// 打开抽屉
const openPopup = () => {
  // 重新获取当前选中邮箱
  currentEmail(authEmailValue.value);
  // 打开抽屉
  selectEmail.value = true;
};
// 取消
const cancel = () => {
  selectEmail.value = false;
  authEmailFlag.value = false;
};
// 确定
const sure = () => {
  selectEmail.value = false;
  // 将选择的值赋值给页面渲染
  authEmailValue.value = selectPopupEmail.value;
  authEmailFlag.value = false;
};

// 发送验证码成功提示
const fetchSuccess = () => {
  proxy.$toast('验证码已发送');
};

// 检查邮箱是否签署
const emailCheck = (sign: boolean) => {
  if (sign) {
    proxy.$dialog({
      message: '该邮箱为已签署协议邮箱，无须重复签署',
    });
  }
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
      getPlatEmailVal.value = props.email;
    } else if (props.errorDescription) {
      proxy.$toast(props.errorDescription);
      return;
    } else if (!props.code && !props.email) {
      verificationMethodVisible.value = true;
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
      class="agreement-content-visible"
    >
      <div
        class="mobile-cla-content"
      >
        <div
          v-show="claContent.projectName"
          class="title"
        >
          {{ claContent.projectName }}开源项目个人贡献者许可协议
        </div>
        <jm-markdown
          v-model="claContent.agreementContent"
          :mode="'readonly'"
        />
      </div>
      <!-- 底部信息 -->
      <div
        v-if="bottomVisible"
        class="bottom-info"
      >
        <!--  选择验证方式/标题/邮箱内容 -->
        <div
          v-if="verificationMethodVisible"
          class="select-verification-method"
        >
          <!-- 验证方式标题 -->
          <div
            ref="verificationRef"
            class="verification-method"
          >
            <div class="method-title">
              请选择验证方式
            </div>
            <div class="method-tips">
              注：邮箱必须为提交代码的邮箱，否则将无法通过CLA验证。如果您打算使用多个邮箱提交代码，请分别用它们签署CLA协议。
            </div>
          </div>
          <!-- 验证方式选择  -->
          <div class="verification-radio">
            <van-radio-group
              v-model="radioAuthEmail"
              direction="horizontal"
            >
              <van-radio name="GITEE">
                gitee邮箱验证
              </van-radio>
              <van-radio name="other">
                其他邮箱验证
              </van-radio>
            </van-radio-group>
          </div>
          <!-- 邮箱内容  -->
          <div
            v-if="emailContentVisible"
            class="email-content"
          >
            <!-- 授权邮箱验证验证   -->
            <div
              v-if="radioEmailVisible"
              class="auth-email"
            >
              <van-cell
                :border="false"
                arrow-direction="down"
                center
                is-link
                @click="openPopup"
              >
                <template #title>
                  <span class="custom-title">{{ authEmailValue ? authEmailValue : '无可签署邮箱' }}</span>
                </template>
              </van-cell>
              <van-popup
                v-model:show="selectEmail"
                :style="{ height: '50%' }"
                position="bottom"
                round
              >
                <template #default>
                  <!--  顶部按钮  -->
                  <div class="select-btn">
                    <div @click="cancel">
                      取消
                    </div>
                    <div
                      v-if="!selectEmailTitleFlag"
                      class="sure-btn"
                      @click="sure"
                    >
                      确定
                    </div>
                  </div>
                  <!--  顶部标题  -->
                  <div class="select-title">
                    <div
                      v-if="!selectEmailTitleFlag"
                      class="title"
                    >
                      选择签署邮箱
                    </div>
                    <div
                      v-else
                      class="title"
                    >
                      无可签署邮箱
                    </div>
                    <div class="tips">
                      置灰邮箱为已签署协议邮箱，无须重复签署
                    </div>
                  </div>
                  <!--  内容  -->
                  <div
                    class="select-content"
                  >
                    <van-radio-group
                      v-for="item of authEmailList"
                      :key="item.email"
                      v-model="selectPopupEmail"
                    >
                      <van-cell-group>
                        <van-cell
                          v-if="!item.isSigned"
                          :title="item.email"
                          clickable
                          @click="currentEmail(item.email)"
                        >
                          <template #title>
                            {{ item.email }}
                            <van-tag
                              v-if="item.labels.indexOf('主邮箱') !== -1"
                              plain
                              type="success"
                            >
                              {{ item.labels[item.labels.indexOf('主邮箱')] }}
                            </van-tag>
                            <van-tag
                              v-if="item.labels.indexOf('提交邮箱') !== -1"
                              plain
                              type="warning"
                            >
                              {{ item.labels[item.labels.indexOf('提交邮箱')] }}
                            </van-tag>
                          </template>
                          <template
                            #right-icon
                          >
                            <div
                              v-if="authEmailList.indexOf(item).toString() === currentRadio"
                              class="selected"
                            />
                          </template>
                        </van-cell>
                        <!--  已签署的邮箱  -->
                        <div
                          v-else
                          class="disabled-email"
                        >
                          {{ item.email }}
                          <van-tag
                            v-if="item.labels.indexOf('主邮箱') !== -1"
                            plain
                            type="success"
                          >
                            {{ item.labels[item.labels.indexOf('主邮箱')] }}
                          </van-tag>
                          <van-tag
                            v-if="item.labels.indexOf('提交邮箱') !== -1"
                            plain
                            type="warning"
                          >
                            {{ item.labels[item.labels.indexOf('提交邮箱')] }}
                          </van-tag>
                        </div>
                      </van-cell-group>
                    </van-radio-group>
                  </div>
                </template>
              </van-popup>
            </div>
            <!-- 其他邮箱验证 -->
            <div
              v-if="!radioEmailVisible"
              class="other-email"
            >
              <van-field
                v-model="otherEmail"
                name="otherEmail"
                placeholder="请输入邮箱"
                @update:model-value="updateInput"
              />
              <img
                v-show="emailVisible"
                class="clear-email"
                src="@/assets/svgs/clear.svg"
                @click="otherEmail = ''"
              >
              <van-field
                v-model="otherCode"
                maxlength="6"
                name="otherCode"
                placeholder="请输入验证码"
                @update:model-value="updateInput"
              />
              <!--  向邮箱发送验证码  -->
              <div class="email-verification">
                <email-verification-code
                  :email="otherEmail"
                  class="email-verification-code"
                  email-flag="check"
                  @fetch-success="fetchSuccess"
                  @email-check="emailCheck"
                />
              </div>
            </div>
          </div>
        </div>
        <!--  获取邮箱签署  -->
        <div
          v-else
          ref="verificationRef"
          class="get-sign-method"
        >
          <!-- 平台邮箱 -->
          <div
            v-if="platformVisible"
            class="get-platform-email"
          >
            <van-field
              v-model="getPlatEmailVal"
              name="otherEmail"
              placeholder="请输入邮箱"
              readonly
            />
          </div>
          <!-- 获取其他邮箱签署 -->
          <div
            v-else
            class="get-other-emails"
          >
            <van-field
              v-model="getOtherEmailVal.email"
              name="otherEmail"
              placeholder="请输入邮箱"
              readonly
            />
            <van-field
              v-model="getOtherEmailVal.code"
              maxlength="6"
              name="otherEmail"
              placeholder="请输入验证码"
            />
            <!--  获取验证码  -->
            <div class="email-verification">
              <email-verification-code
                :email="getOtherEmailVal.email"
                class="email-verification-code"
                email-flag="check"
                @fetch-success="fetchSuccess"
                @email-check="emailCheck"
              />
            </div>
          </div>
        </div>
        <!-- 签署按钮 -->
        <div
          class="sign-container"
        >
          <van-button
            :disabled="signDisable"
            class="sign-btn"
            @click="toSign"
          >
            签署
          </van-button>
          <div class="agree">
            <van-checkbox
              v-model="mobileCheckbox"
              shape="square"
            />
            <span @click="mobileCheckbox = !mobileCheckbox">我已阅读并同意上述个人贡献者许可协议的条款。</span>
          </div>
        </div>
        <!--  阅读提示  -->
        <div
          v-if="readVisible"
          class="read-tips"
        >
          <button
            class="tips-btn"
            disabled
          >
            请上滑看完本条款再签署
          </button>
        </div>
      </div>
    </div>
  </div>
</template>

<style lang="less" scoped>
.agreement {
  min-height: 100vh;
  background: #F7F9FD;

  // 协议内容
  .agreement-content-visible {
    //  内容
    .mobile-cla-content {
      padding: 0 30px 40px 30px;
      border-bottom: 1px solid transparent;
      font-size: 30px;
      color: #385775;
      line-height: 42px;
      background: #FFF;

      .title {
        font-size: 36px;
        font-weight: 600;
        color: #012C53;
        margin: 40px 0 30px 0;
      }

      // 二级标题
      .content-title {
        font-weight: 550;
      }

      // 段落缩进
      .indent {
        text-indent: 32px;
      }
    }

    // 底部信息
    .bottom-info {
      // 选择验证方式/标题/邮箱内容
      .select-verification-method {
        //验证方式标题
        .verification-method {
          margin-top: 24px;
          background: #fff;
          box-sizing: border-box;
          padding: 0 30px;
          font-size: 24px;
          border-top: 1px solid transparent;

          .method-title {
            height: 42px;
            font-size: 30px;
            font-weight: 600;
            color: #012C53;
            line-height: 42px;
            margin: 40px 0 20px 0;
          }

          .method-tips {
            width: 690px;
            line-height: 40px;
            font-size: 28px;
            font-weight: 400;
            color: #7B8C9C;
          }
        }

        // 验证方式选择
        .verification-radio {
          font-size: 30px;
          padding: 40px 30px 0px;
          background: #fff;

          .van-radio--horizontal:first-child {
            margin-right: 110px;
          }
        }

        // 邮箱内容
        .email-content {
          // 授权邮箱验证
          .auth-email {
            background: #fff;
            font-size: 24px;
            box-sizing: border-box;
            padding: 40px 30px 0px 30px;


            .van-cell {
              min-height: 94px;
              width: 690px;
              padding: 26px 30px;
              background: #F6F8FB;
              border-radius: 10px;
              border: 1px solid #E8E8E8;
              line-height: 40px;
            }


            // 邮箱选择
            .van-popup {
              .select-btn {
                display: flex;
                justify-content: space-between;
                align-items: center;
                box-sizing: border-box;
                padding: 40px 30px 4px 30px;
                font-size: 30px;
                color: #042749;

                .sure-btn {
                  color: @themeColor;
                }
              }

              .select-title {
                text-align: center;

                .title {
                  font-size: 30px;
                  font-weight: 500;
                  color: #012C53;
                  margin-bottom: 20px;
                }

                .tips {
                  font-size: 24px;
                  font-weight: 400;
                  color: #7B8C9C;
                  padding-bottom: 40px;
                  border-bottom: 1px solid #E7ECF1;
                }
              }

              .select-content {
                padding: 0 30px;

                .van-tag {
                  height: 34px;
                  margin: 0 8px;
                }

                .disabled-email, .select-email {
                  min-height: 100px;
                  font-size: 30px;
                  color: #042749;
                  line-height: 40px;
                  padding: 30px 0;
                }

                .disabled-email {
                  opacity: 0.4;
                }

                .van-cell {
                  border-top: none;
                  font-size: 30px;

                  .selected {
                    margin: 0 10px;
                    width: 14px;
                    height: 22px;
                    transform: rotate(45deg);
                    border-style: solid;
                    border-color: @themeColor;
                    border-width: 0 4px 4px 0;
                  }
                }
              }

              .van-cell {
                min-height: 100px;
                border: none;
                background: #fff;
                display: flex;
                align-items: center;
                line-height: 40px;
                padding: 30px 0px;
              }
            }
          }

          // 其他邮箱验证
          .other-email {
            background: #fff;
            padding: 40px 30px 0px;
            position: relative;

            .clear-email {
              width: 36px;
              height: 36px;
              position: absolute;
              top: 68px;
              left: 660px;
            }

            .van-cell {
              width: 690px;
              height: 94px;
              padding: 0px;
              padding-left: 30px;
              padding-right: 70px;
              border-radius: 10px;
              border: 1px solid #B9CFE6;
              line-height: 94px;
              font-size: 30px;

              .van-field__control::placeholder {
                color: red;
              }
            }

            .van-cell:first-child {
              margin-bottom: 40px;
            }

            .email-verification-code {
              position: absolute;
              top: 203px;
              right: 50px;
            }
          }
        }
      }

      // 获取邮箱签署
      .get-platform-email {
        padding: 60px 30px 0 30px;
        background: #fff;
        margin-top: 24px;

        .van-cell {
          width: 690px;
          height: 94px;
          padding: 0px;
          padding-left: 30px;
          border-radius: 10px;
          border: 1px solid #B9CFE6;
          line-height: 94px;
          font-size: 30px;

          .van-field__control::placeholder {
            color: red;
          }
        }
      }

      // 获取其他邮箱签署
      .get-other-emails {
        padding: 60px 30px 0 30px;
        background: #fff;
        margin-top: 24px;
        position: relative;

        .van-cell {
          width: 690px;
          height: 94px;
          padding: 0px;
          padding-left: 30px;
          padding-right: 30px;
          border-radius: 10px;
          border: 1px solid #B9CFE6;
          line-height: 94px;
          font-size: 30px;
        }

        .van-cell:nth-child(2) {
          margin-top: 40px;
        }

        .email-verification-code {
          position: absolute;
          bottom: 25px;
          right: 50px;
        }
      }

      //  签署按钮
      .sign-container {
        width: 750px;
        height: 240px;
        padding: 0 30px;
        background: #FFF;

        .sign-btn {
          width: 690px;
          height: 94px;
          border-radius: 10px;
          color: #FFF;
          font-size: 36px;
          background: @themeColor;
          border: 1px solid @themeColor;
          margin: 60px 0 45px 0;
        }

        .agree {
          height: 36px;
          display: flex;
          align-items: center;
          font-size: 28px;
          color: #042749;
          box-sizing: border-box;
          padding-bottom: 80px;

          .van-checkbox {
            margin-right: 15px;

            .van-checkbox__icon .van-icon {
              width: 36px;
              height: 36px;
            }
          }
        }
      }

      // 阅读提示
      .read-tips {
        width: 750px;
        height: 175px;
        background: #fff;
        box-shadow: 0px -4px 4px 0px rgba(228, 228, 228, 0.5);
        position: fixed;
        bottom: 0;
        left: 0;
        text-align: center;
        line-height: 175px;

        .tips-btn {
          width: 690px;
          height: 94px;
          line-height: 94px;
          background: @themeColor;
          border: 1px solid @themeColor;
          border-radius: 10px;
          opacity: 0.39;
          color: #fff;
        }
      }
    }
  }
}
</style>