<template>
<el-form ref="form" :model="fd" label-width="100px" :rules="rules">
  <el-form-item label="新密码" prop="newPassword">
    <el-input v-model="fd.newPassword" type="password" show-password />
  </el-form-item>
  <el-form-item label="确认密码" prop="confirmPassword">
    <el-input v-model="fd.confirmPassword" type="password" @keyup.enter="changePwd" show-password />
  </el-form-item>
  <el-form-item>
    <el-button @click="() => $emit('close')" :loading="loading">取消</el-button>
    <el-button type="primary" @click="changePwd" :loading="loading">确定</el-button>
  </el-form-item>
</el-form>
</template>

<script>
import {md5} from '../../../js/lib/md5.min.js'

export default {
  name: "ResetPass",
  props: {
    user: {type: Object, required: true}
  },
  data() {
    const fd = {}
    return {
      fd,
      rules: {
        newPassword: [
          {required: true, message: '必填', trigger: 'blur',}
        ],
        confirmPassword: [{
          trigger: 'blur', required: true,
          validator(rule, value, callback) {
            if (fd.confirmPassword == null || fd.confirmPassword === '') callback('必填')
            else if (fd.newPassword !== fd.confirmPassword) callback('两次密码不一样')
            else callback()
          }
        }]
      },
      loading: false,
    }
  },
  methods: {
    changePwd() {
      this.$refs.form.validate((valid) => {
        if (valid) {
          this.loading = true
          axios.post(`user/${this.user.id}/restPassword`, {newPassword: md5(this.fd.newPassword)}).then(res => {
            this.loading = false
            if (res.code === '00') {
              this.$message.success('密码重置成功')
              this.$emit('close')
            }
          }).catch(e => this.loading = false)
        }
      })
    }
  }
}
</script>

<style scoped>

</style>