<template>
    <el-row style="margin-bottom: 10px">
        <el-col :span="24">
            <el-input v-model="url" placeholder="请求地址"/>
        </el-col>
    </el-row>
    <el-row v-for="(param, index) in paramItems" style="margin-top: 6px">
        <el-col :span="8" :title="param.name">
            <el-input v-model="param.code" placeholder="参数名" :readonly="param.type"  />
        </el-col>
        <el-col :span="12" style="margin-left: 2px; margin-right: 2px">
            <el-select v-if="param.type === 'Bool'" v-model="param.value" :placeholder="param.name" :disabled="param._readonly" style="width: 100%">
                <el-option label="是" value="true" />
                <el-option label="否" value="false" />
            </el-select>
            <el-select v-else-if="param.enumValues" v-model="param.value" :placeholder="param.name" style="width: 100%">
                <el-option v-for="item in param.enumValues" :key="item.value" :label="item.label" :value="item.value" />
            </el-select>
            <el-input v-else-if="param.type === 'Time'" v-model="param.value" :placeholder="param.name + ', ' + param.format" :disabled="param._readonly"/>
            <el-input v-else v-model="param.value" :placeholder="param.name" :disabled="param._readonly"/>
        </el-col>
        <el-col :span="2">
            <el-space>
                <el-button type="danger" @click="del(param)" size="mini" circle><el-icon> <svg-delete /> </el-icon></el-button>
                <el-button v-if="paramItems.length === (index + 1)" type="success" @click="add" size="mini" circle><el-icon> <svg-plus /> </el-icon></el-button>
            </el-space>
        </el-col>
    </el-row>
    <el-row style="display: flex; justify-content: center; margin-top: 12px">
        <el-button type="primary" @click="test" :loading="testing">测试</el-button>
    </el-row>
    <el-row style="margin-top: 12px">
        <ace-json v-model="result" style="min-height: 220px; max-height: 600px" readonly placeholder="// 显示决策接口测试结果" />
    </el-row>
</template>

<script>
import SvgMinus from "../../../component/svg/SvgMinus.vue";
import SvgPlus from "../../../component/svg/SvgPlus.vue";
import AceJson from "../../../component/AceJson.vue";
import SvgDelete from "../../../component/svg/SvgDelete.vue";
export default {
    name: "Test",
    components: {SvgDelete, AceJson, SvgPlus, SvgMinus},
    props: {
        decision: {type: Object, required: true},
    },
    data() {
        const cacheKey = 'rule.test.' + this.decision.id;
        return {
            url: location.protocol + '//' + location.host + `/decision/${this.decision.id}/decide`,
            result: null,
            cacheKey,
            testing: false,
            paramItems: (() => {
                const itemsStr = localStorage.getItem(cacheKey);
                const items = itemsStr ? JSON.parse(itemsStr) : []
                const arr = this.decision.apiConfig ? this.decision.apiConfig.map(cfg => {
                    const item = items.find(i => i && i.code === cfg.code);
                    return {
                        ...cfg,
                        ...{
                            value: (cfg.fixValue != null ? cfg.fixValue : (cfg.defaultValue != null ? cfg.defaultValue : (item ? item.value : null))),
                            _readonly: cfg.fixValue != null
                        }}
                }) : [];
                for (const index in items) {
                    const item = items[index];
                    if (item && item.code != null && !arr.find(i => i && i.code === item.code)) {
                        arr.push(item)
                    }
                }
                return arr
            })()
        }
    },
    computed: {
    },
    methods: {
        test() {
            this.result = '';
            const params = this.paramItems.map((param) => {const o={}; o[param.code] = param.value; return o}).reduce((o1, o2) => {const o = {...o1, ...o2}; return o})
            this.testing = true
            axios.post(this.url, params).then(resp => {
                this.testing = false
                this.result = JSON.stringify(resp, null, 4).trim();
                if (resp.code === '00') {
                    this.$message.success(`测试调用: ${this.decision.name} 成功`);
                    localStorage.setItem(this.cacheKey, JSON.stringify(this.items.map(o => {return {code: o.code, value: o.value}})));
                }
            }).catch(e => this.testing = false)
        },
        add() {
            this.paramItems.push({name: `参数${this.paramItems.length + 1}`, value: null})
        },
        del(param) {
            let index = this.paramItems.indexOf(param);
            this.paramItems.splice(index, 1)
        }
    }
}
</script>

<style scoped>

</style>