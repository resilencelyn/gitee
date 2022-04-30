<template>
  <table class="attr-table" style="width: 100%;">
    <tr v-for="(v, k) in attrs" style="width: 100%;">
      <td align="right" style="width: 30%;">{{k}}</td>
      <td align="left" style="width: 70%;">{{v}}</td>
    </tr>
  </table>
</template>

<script>
export default {
  name: "AttrTable",
  props: {
    attrs: {type: Object, required: true}
  }
}
</script>

<style scoped>
.attr-table tr:first-child td:first-child{
  border-left: 0;
}
.attr-table tr td{
  border-bottom: 1px solid #efefef;
  border-right: 1px solid #efefef;
  height: 38px;
  line-height: 38px;
  padding: 0 12px;
}
.attr-table tr td:nth-child(even){
  border-right:0;
}
.attr-table tr:last-child td{
  border-bottom: 0;
}
</style>