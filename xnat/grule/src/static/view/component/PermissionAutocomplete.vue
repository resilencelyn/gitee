<!-- 权限自动选择器 -->
<template>
    <el-select-v2 v-model="modelValue" filterable remote :loading="loading" :remote-method="load" :options="opts"
               clearable :multiple="multiple" :collapse-tags="collapse-tags" :placeholder="placeholder" :size="size" @change="change" >
<!--        <el-option v-for="opt in opts" :key="opt.enName" :label="opt.cnName" :value="opt.enName" />-->
    </el-select-v2>
</template>

<script>
export default {
    name: "PermissionAutocomplete",
    props: {
        uId: {type: String, required: true},
        limit: {type: Number, default: 5},
        size: {
            type: String, default: 'default'
        },
        modelValue: String,
        placeholder: {type: String, default: '权限'},
        multiple: {type: Boolean, default: false},
        'collapse-tags': {type: Boolean, default: true}
    },
    emits: ['update:modelValue'],
    data() {
        return { opts: [], loading: false, }
    },
    created() {
        this.load('', this.modelValue)
    },
    methods: {
        load(nameLike) {
            this.loading = true
            axios.get(`user/privileges/${this.uId}?limit=${this.limit}${nameLike? '&nameLike=' + nameLike : ''}`).then(resp => {
                this.loading = false
                if (resp.code === '00') {
                    this.opts = resp.data.map(item => {
                        return {value: item.privilegeId, label: item.privilegeName}
                    })
                }
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