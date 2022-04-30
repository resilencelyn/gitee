<!-- 字段自动选择器 -->
<template>
    <el-select v-model="modelValue" filterable remote :loading="loading" :remote-method="load"
               clearable :placeholder="placeholder" :size="size" @change="change" style="width: 100%" >
        <el-option v-for="opt in opts" :key="opt.key" :label="opt.name" :value="opt.key" />
    </el-select>
</template>

<script>
export default {
    name: "FieldAutocomplete",
    props: {
        limit: {type: Number, default: 5},
        size: {
            type: String, default: 'default'
        },
        modelValue: String,
        placeholder: {type: String, default: '字段'},
    },
    emits: ['update:modelValue'],
    data() {
        return { opts: [], loading: false, }
    },
    created() {
        this.load('', this.modelValue)
    },
    methods: {
        load(kw, id) {
            this.loading = true
            axios.get(`field/paging/1?limit=${this.limit}${kw? '&kw=' + kw : ''}${id ? '&id=' + id : ''}`).then(resp => {
                this.loading = false
                if (resp.code === '00') this.opts = resp.data.list
            }).catch(e => this.loading = false)
        },
        change(v) {
            this.$emit('update:modelValue', v)
        }
    }
}
</script>

<style scoped>

</style>