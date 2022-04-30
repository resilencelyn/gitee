<template>
    <el-row style="margin-bottom: 10px">
        <el-input v-model="url"  placeholder="请求地址"/>
    </el-row>
    <el-row v-for="(param, index) in items" style="margin-top: 6px">
        <el-col :span="8"><el-input v-model="param.code"  placeholder="参数名" /></el-col>
        <el-col :span="12" style="margin-left: 2px; margin-right: 2px">
            <el-input v-model="param.value"  :placeholder="param.name" />
        </el-col>
        <el-col :span="2">
            <el-space>
                <el-button type="danger" @click="del(param)" size="mini" circle><el-icon> <svg-delete /> </el-icon></el-button>
                <el-button v-if="items.length === (index + 1)" type="success" @click="add" size="mini" circle>
                    <el-icon> <svg-plus /> </el-icon>
                </el-button>
            </el-space>
        </el-col>
    </el-row>
    <el-row style="display: flex; justify-content: center; margin-top: 12px">
        <el-button type="primary"  @click="test" :loading="testing">测试</el-button>
    </el-row>
    <el-row style="margin-top: 12px">
        <ace-json v-model="result" style="min-height: 180px; max-height: 600px" />
    </el-row>
</template>

<script>
import SvgDelete from "../../../component/svg/SvgDelete.vue";
import SvgPlus from "../../../component/svg/SvgPlus.vue";
import AceJson from "../../../component/AceJson.vue";
export default {
    name: "Test",
    components: {AceJson, SvgPlus, SvgDelete},
    props: { collector: Object },
    data() {
        this.cacheKey = `rule.test.${this.collector.id}`
        return {
            url: location.protocol + '//' + location.host + `/collector/${this.collector.id}/test`,
            testing: false,
            items: (() => {
                let itemsStr = localStorage.getItem(this.cacheKey);
                if (itemsStr) return JSON.parse(itemsStr);
                return [{name: "参数", value: null}]
            })(),
            result: '',
        }
    },
    methods: {
        test() {
            let items = this.items.filter(item => item.code);
            if (items && items.length > 0) {
                items = items.map((param) => {let o={}; o[param.code] = param.value; return o}).reduce((o1, o2) => {let o = {...o1, ...o2}; return o})
            } else items = null;
            this.result = '';
            this.testing = true
            axios.post(this.url, items).then(resp => {
                this.testing = false
                if (resp.code === '00') {
                    this.result = JSON.stringify(resp.data, null, 4).trim();
                    this.$message.success(`测试调用: ${this.collector.name} 成功`);
                    localStorage.setItem(this.cacheKey, JSON.stringify(this.items.map(o => {return {code: o.code, value: o.value}})));
                }
            }).catch(e => this.testing = false)
        },
        add() {
            this.items.push({name: `参数${this.items.length + 1}`, value: null})
        },
        del(param) {
            this.items.splice(this.items.indexOf(param), 1)
        },
    }
}
</script>

<style scoped>

</style>