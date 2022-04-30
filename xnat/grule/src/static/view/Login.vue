<template>
    <div class="main-login" :style="{backgroundImage: 'url(' + require('@/img/bg-login.jpeg') +')'}">
        <el-form ref="form" :model="fd" :rules="rules" label-width="70px">
            <el-form-item label="用户名" prop="username">
                <el-input v-model="fd.username" :autofocus="!fd.username" />
            </el-form-item>
            <el-form-item label="密码" prop="password">
                <el-input v-model="fd.password" type="password" @keyup.enter="login" :autofocus="fd.username && !fd.password" show-password />
            </el-form-item>
            <el-form-item>
                <el-button type="primary" @click="login">登录</el-button>
            </el-form-item>
        </el-form>
    </div>
</template>

<script>
import {md5} from '../js/lib/md5.min.js'

export default {
    name: "Login",
    data() {
        return {
            fd: {username: '', password: ''},
            rules: {
                username: [
                    {required: true, message: 'Username required', trigger: 'blur',}
                ],
                password: [
                    {required: true, message: 'Password required', trigger: 'blur',},
                ]
            }
        }
    },
    mounted() {
      // 已经登录了, 自动跳到主页
      if (this.$user.id) {
        this.$router.push('/')
      }
    },
    methods: {
        login() {
            this.$refs.form.validate((valid) => {
                if (valid) {
                    const param = {...this.fd};
                    param.password = md5(param.password);
                    const loading = this.$loading({target: document.body})
                    axios.post('login', param).then(resp => {
                        loading.close()
                        if (resp.code === '00') {
                          this.$router.push('/')
                          this.$emit('init')
                        }
                    }).catch(e => loading.close())
                }
            });
        }
    }
}
</script>

<style scoped>
.main-login {
    height: 100vh;
    display: flex; align-items: center; justify-content: center; align-content: center;
    background-repeat: no-repeat; background-size: 100% 100%;
}
.form {
    height: 100%; display: flex; align-items: center; justify-content: center; flex-direction: column;
}
</style>
