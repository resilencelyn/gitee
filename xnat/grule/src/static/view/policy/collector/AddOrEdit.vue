<template>
    <el-scrollbar :max-height="maxHeight">
        <el-form ref="form" :model="fd" :rules="rules" label-width="98px"  style="width: calc(100% - 10px)" :disabled="!hasUpdateAuth">
            <el-row>
                <el-col :span="14">
                    <el-form-item label="收集器名" prop="name">
                        <el-input v-model="fd.name" />
                    </el-form-item>
                </el-col>
                <el-col :span="6">
                    <el-form-item label="类型" prop="type">
                        <el-select v-model="fd.type"  style="width: 100%">
                            <el-option v-for="opt in $cfg.collectorType" :key="opt.key" :value="opt.key" :label="opt.name"/>
                        </el-select>
                    </el-form-item>
                </el-col>
                <el-col :span="4">
                    <el-form-item label="记录结果" prop="recordResult">
                        <el-checkbox v-model="fd.recordResult"  />
                    </el-form-item>
                </el-col>
            </el-row>

            <el-form-item label="缓存动态key" prop="cacheKey">
                <el-input v-model="fd.cacheKey"  placeholder="缓存key = 收集器id + 动态key(GString)" />
            </el-form-item>
            <el-form-item label="缓存超时" prop="cacheTimeoutFn">
                <ace-groovy v-model="fd.cacheTimeoutFn" style="height: 70px" :readonly="!hasUpdateAuth"
                            :placeholder="`固定缓存时间计算Groovy脚本\n1. 返回整数, 表示在多少分钟后过期\n2. 返回Date, 表示过期时间点\n3. 返回Duration类型, 表示在Duration时间之后过期`"
                />
            </el-form-item>

            <el-form-item label="描述" prop="comment">
                <el-input v-model="fd.comment" type="textarea" :rows="2" placeholder="此收集器的描述信息" />
            </el-form-item>

            <el-form-item v-if="fd.type === 'Http'" label="接口地址" prop="url">
                <el-input v-model="fd.url"  placeholder="http://example.com/api/test?param=${var1}"/>
            </el-form-item>
            <el-form-item v-if="fd.type === 'Sql'" label="jdbcUrl" prop="url">
                <el-input v-model="fd.url"  placeholder="jdbc:mysql://localhost:3306/rule?useSSL=false&user=root&password=root&useUnicode=true&characterEncoding=utf-8&allowPublicKeyRetrieval=true"/>
            </el-form-item>

            <el-row v-if="fd.type === 'Http'">
                <el-col :span="12">
                    <el-form-item  label="方法" prop="method">
                        <el-select v-model="fd.method"  style="width: 100%">
                            <el-option v-for="opt in $cfg.httpMethod" :key="opt.key" :value="opt.key" :label="opt.name"/>
                        </el-select>
                    </el-form-item>
                </el-col>
                <el-col :span="12">
                    <el-form-item label="读超时(ms)" prop="timeout">
                        <el-input-number v-model="fd.timeout"  :min="20" :max="600000" controls-position="right" />
                    </el-form-item>
                </el-col>
            </el-row>

            <el-form-item v-if="fd.type === 'Http' && fd.method === 'POST'" label="ContentType" prop="contentType">
                <el-select v-model="fd.contentType"  style="width: 100%">
                    <el-option v-for="opt in $cfg.httpContentType" :key="opt.key" :value="opt.key" :label="opt.name"/>
                </el-select>
            </el-form-item>
            <el-form-item v-if="fd.type === 'Http' && fd.method === 'POST'" label="请求体" prop="bodyStr">
                <ace-json v-if="fd.contentType === $cfg.httpContentType.find(o => o.name === 'json').key" v-model="fd.bodyStr" style="height: 100px" placeholder="json 字符串(GString)"/>
                <el-input v-else v-model="fd.bodyStr" type="textarea" :rows="4" placeholder="字符串(GString). 例: a=1&b=${bb}" />
            </el-form-item>

            <el-row>
                <el-col :span="12">
                    <el-form-item v-if="fd.type === 'Sql'" label="最少闲连接" prop="minIdle">
                        <el-input-number v-model="fd.minIdle"  :min="0" :max="50" />
                    </el-form-item>
                </el-col>
                <el-col :span="12">
                    <el-form-item v-if="fd.type === 'Sql'" label="最大连接数" prop="maxActive">
                        <el-input-number v-model="fd.maxActive"  :min="1" :max="50" />
                    </el-form-item>
                </el-col>
            </el-row>

            <el-form-item v-if="fd.type === 'Http'" label="是否成功" prop="dataSuccessScript">
                <ace-groovy v-model="fd.dataSuccessScript" style="height: 90px; width: 670px" :readonly="!hasUpdateAuth"/>
            </el-form-item>
            <el-form-item v-if="fd.type === 'Http'" label="结果解析" prop="parseScript">
                <ace-groovy v-model="fd.parseScript" style="height: 220px; width: 670px" :readonly="!hasUpdateAuth"/>
            </el-form-item>
            <el-form-item v-if="fd.type === 'Script'" label="值计算脚本" prop="computeScript">
                <ace-groovy v-model="fd.computeScript" style="height: 300px; width: 670px" :readonly="!hasUpdateAuth" placeholder='// Groovy脚本' />
            </el-form-item>
            <el-form-item v-if="fd.type === 'Sql'" label="Sql执行脚本" prop="sqlScript">
                <ace-groovy v-model="fd.sqlScript" style="height: 250px; width: 670px" :readonly="!hasUpdateAuth" :placeholder='`// Groovy脚本. 默认数据底操作变量DB(Groovy SQL实例) \nDB.firstRow("select * from test")["id"]`' />
            </el-form-item>

            <el-form-item v-if="hasUpdateAuth">
                <el-button type="primary"  @click="submit" v-loading="submitting">提交</el-button>
            </el-form-item>
        </el-form>
    </el-scrollbar>
</template>

<script>
import AceGroovy from "../../../component/AceGroovy.vue";
import AceJson from "../../../component/AceJson.vue";
export default {
    name: "AddOrEdit",
    props: {
        collector: Object
    },
    components: {AceJson, AceGroovy},
    data() {
        const vm = this
        const fd = this.collector ? clone(this.collector, {}) : {
          name: '', type: 'Http', comment: '', method: 'GET', timeout: 10000, minIdle: 0, maxActive: 5,
          recordResult: true, contentType: this.$cfg.httpContentType.find(o => o.name === 'form').key,
          dataSuccessScript:
              `{resultStr, respCode -> // 接口返回的字符串, 接口http响应码(如果是缓存则为空)
    return (respCode == null || respCode == 200) && resultStr != null
}`, parseScript: `{resultStr, respCode -> // 接口返回的字符串, 接口http响应码(如果是缓存则为空)
    return resultStr
}`
        }
        return {
            maxHeight: '398px',
            fd,
            submitting: false,
            rules: {
                name: [
                    {
                        required: true, message: '必填', trigger: 'blur',
                    },
                    {
                        validator(rule, value, callback) {
                            if (value.length > 255) {
                                callback(new Error('最大长度255'))
                            } else callback()
                        }
                    }
                ],
                type: [
                    {
                        required: true, message: '必填', trigger: 'blur',
                    },
                ],
                url: [{
                    type: 'string',
                    validator(rule, value, callback) {
                        if (fd.type === 'Http') {
                            if (!value) callback(new Error('必填'))
                            else if (!value.startsWith('http')) callback(new Error('以 http 开头的接口地址'))
                            else callback()
                        } else if (fd.type === 'Sql') {
                            if (!value) callback(new Error('必填'))
                            else if (!value.startsWith('jdbc:')) callback(new Error('以 jdbc: 开头的数据库连接地址'))
                            else callback()
                        } else callback()
                    },
                    trigger: 'blur',
                }],
                computeScript: [{
                    trigger: 'blur',
                    validator(rule, value, callback) {
                        if (fd.type === 'Script') {
                            if (!value) callback(new Error('必填'))
                            else callback()
                        } else callback()
                    }
                }],
                sqlScript: [{
                    trigger: 'blur',
                    validator(rule, value, callback) {
                        if (fd.type === 'Sql') {
                            if (!value) callback(new Error('必填'))
                            else callback()
                        } else callback()
                    }
                }],
                minIdle: [{
                    trigger: 'change',
                    validator(rule, value, callback) {
                        if (fd.type === 'Sql') {
                            if (value > fd.maxActive) callback(new Error('不能大于 最大连接数'))
                            else callback()
                            vm.$refs.form.validateField('maxActive')
                        } else callback()
                    }
                }],
                maxActive: [{
                    trigger: 'change',
                    validator(rule, value, callback) {
                        if (fd.type === 'Sql') {
                            if (value < fd.minIdle) callback(new Error('不能小于 最小连接数'))
                            else callback()
                            vm.$refs.form.validateField('minIdle')
                        } else callback()
                    }
                }],
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
        return this.$user.privileges.has('Collector_Update')
      },
    },
    methods: {
        submit() {
            if (this.fd.id) {
              this.$refs.form.validate((valid) => {
                    if (valid) {
                        this.submitting = true
                        axios.post('collector/' + this.fd.id, this.fd).then(resp => {
                            this.submitting = false
                            if (resp.code === '00') {
                                this.$message.success(`更新收集器(${this.fd.name})成功`)
                                this.$emit('close')
                            }
                        }).catch(e => this.submitting = false)
                    }
                });
            } else {
              this.$refs.form.validate((valid) => {
                    if (valid) {
                        this.submitting = true
                        axios.put('collector/', this.fd).then(resp => {
                            this.submitting = false
                            if (resp.code === '00') {
                                this.$message.success(`添加收集器(${this.fd.name})成功`)
                                this.$emit('close')
                            }
                        }).catch(e => this.submitting = false)
                    }
                });
            }
        },
    }
}
</script>

<style scoped>

</style>