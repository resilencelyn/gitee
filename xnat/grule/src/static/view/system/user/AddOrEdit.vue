<template>
  <el-form v-model="fd" label-width="100px">
    <el-form-item label="用户名" prop="name">
      <el-input v-model="fd.name" autofocus />
    </el-form-item>
    <el-form-item label="密码" prop="password" >
      <el-input v-model="fd.password" type="password" @keyup.enter="add" show-password />
    </el-form-item>
    <el-form-item>
      <el-button @click="$emit('close')">取消</el-button>
      <el-button type="primary" @click="add" :loading="adding">添加</el-button>
    </el-form-item>
  </el-form>
</template>

<script>
import {md5} from '../../../js/lib/md5.min.js'

export default {
  name: "AddOrEdit",
  props: {
    user: Object
  },
  data() {
    return {
      fd: {},
      adding: false,
    }
  },
  methods: {
    add() {
      this.adding = true
      axios.put(`user/`, {name: this.fd.name, password: md5(this.fd.password)}).then(resp => {
        this.adding = false
        if (resp.code === '00') {
          this.$message.success(`添加用户(${this.fd.name})成功`)
          this.$emit('close')
        }
      }).catch(e => this.adding = false)
    }
  }
}
</script>

<style scoped>

</style>