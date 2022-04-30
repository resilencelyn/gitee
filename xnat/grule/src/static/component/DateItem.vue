<template>
    <span>{{timeStr}}</span>
</template>

<script>
import dayjs from '../js/lib/dayjs.min.js'

export default {
    name: "DateItem",
    props: {
        time: {type: [Number, Date]},
        format: {type: String, default: 'YYYY-MM-DD HH:mm:ss'}
    },
    computed: {
        timeStr() {
            if (!this.time) return '';
            return dayjs(this.time).format(this.format);
        }
    }
}
</script>

<style scoped>

</style>