<template>
    <el-scrollbar :max-height="maxHeight">
        <el-form ref="form" :model="fd" :rules="rules" label-width="90px" style="width: calc(100% - 10px)" :disabled="!hasUpdateAuth">
            <el-form-item label="标识" prop="key">
                <el-input v-model="fd.key" />
            </el-form-item>
            <el-form-item label="名字" prop="name">
                <el-input v-model="fd.name" />
            </el-form-item>
            <el-form-item label="类型" prop="type">
                <el-select v-model="fd.type"  style="width: 100%">
                    <el-option v-for="t in $cfg.fieldType" :label="t.name" :value="t.key" />
                </el-select>
            </el-form-item>
            <el-form-item label="说明" prop="comment">
              <el-input v-model="fd.comment" placeholder="字段描述" />
            </el-form-item>
            <el-form-item v-if="(!hasUpdateAuth && fd.collectorOptions && fd.collectorOptions[0].collectorId) || hasUpdateAuth" label="值函数" prop="collectorOptions">
                <div v-for="(opt, index) in fd.collectorOptions" :key="opt" style="width: 100%; margin-top: 2px;">
                    <collector-autocomplete v-model="opt.collectorId" :readonly="!hasUpdateAuth"/>
                    <ace-groovy v-model="opt.chooseFn" style="height: 80px;" :placeholder='"// Groovy脚本. 返回true代表选择当前收集器"' :readonly="!hasUpdateAuth" />
                    <el-button v-if="hasUpdateAuth && !(fd.collectorOptions.length === 1 && index === 0)" type="danger" @click="delCollectorOpt(opt)" size="mini" circle><el-icon> <svg-delete /> </el-icon></el-button>
                    <el-button v-if="hasUpdateAuth && fd.collectorOptions.length === (index + 1)" type="success" @click="addCollectorOpt" size="mini" circle><el-icon> <svg-plus /> </el-icon></el-button>
                </div>
            </el-form-item>
            <el-form-item v-if="hasUpdateAuth">
                <el-button @click="() => $emit('close')">取消</el-button>
                <el-button type="primary" @click="submit" v-loading="submitting">提交</el-button>
            </el-form-item>
        </el-form>
    </el-scrollbar>
</template>

<script>
import CollectorAutocomplete from "../../component/CollectorAutocomplete.vue";
import SvgDelete from "../../../component/svg/SvgDelete.vue";
import SvgPlus from "../../../component/svg/SvgPlus.vue";
import AceGroovy from "../../../component/AceGroovy.vue";
export default {
    name: "AddOrEdit",
    props: {
        field: Object
    },
    components: {AceGroovy, SvgPlus, SvgDelete, CollectorAutocomplete},
    data() {
        const fd = this.field ? clone(this.field, {}) : {
          key: null, name: null, type: 'Str', comment: null,
          collectorOptions: [{collectorId: null, chooseFn: 'true'}]
        }
        if (fd.collectorOptions == null || fd.collectorOptions.length < 1) fd.collectorOptions = [{collectorId: null, chooseFn: 'true'}]
        return {
            maxHeight: '398px',
            fd,
            submitting: false,
            rules: {
                key: [
                    { required: true, message: '必填', trigger: 'blur',}
                ],
                name: [
                  { required: true, message: '必填', trigger: 'blur',}
                ],
                type: [
                  { required: true, message: '必填', trigger: 'blur',}
                ]
            }
        }
    },
    mounted() {
        this.$nextTick(() => {
            this.maxHeight = Math.min(document.body.clientHeight - 200, this.$refs.form.$el.clientHeight + 30)
        })
    },
    computed: {
      hasUpdateAuth() {
        return this.$user.privileges.has('Field_Update')
      },
    },
    methods: {
        submit() {
          // 参数
          const param = {...this.fd}
          param.collectorOptions = [];
          if (this.fd.collectorOptions && this.fd.collectorOptions.length > 0) {
            this.fd.collectorOptions.forEach((value) => {
              if (value.collectorId) {
                param.collectorOptions.push({collectorId: value.collectorId, chooseFn: value.chooseFn})
              }
            })
          }
          if (param.collectorOptions && param.collectorOptions.length > 0) {
            param.collectorOptions = JSON.stringify(param.collectorOptions);
          }
          // 更新函数
          const update = () => {
            this.submitting = true
            axios.post(`field/${this.fd.id}`, param).then(resp => {
              this.submitting = false
              if (resp.code === '00') {
                this.$emit('close')
                this.$message.success(`更新字段(${this.field.name})成功`)
              }
            }).catch(e => this.submitting = false)
          }
          // 添加函数
          const add = () => {
            this.submitting = true
            axios.put('field/', param).then(resp => {
              this.submitting = false
              if (resp.code === '00') {
                this.$emit('close')
                this.$message.success(`添加字段(${param.name})成功`)
              }
            }).catch(e => this.submitting = false)
          }
          this.$refs.form.validate((valid) => {
            if (valid) {
              if (this.fd.id) update()
              else add()
            }
          });
        },
        addCollectorOpt() {
            this.fd.collectorOptions.push({collectorId: null, chooseFn: 'true'})
        },
        delCollectorOpt(opt) {
            this.fd.collectorOptions.splice(this.fd.collectorOptions.indexOf(opt), 1)
        },
    }
}
</script>

<style scoped>

</style>