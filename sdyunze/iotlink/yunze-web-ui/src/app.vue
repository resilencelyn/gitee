<template>
  <div id="app">
    <router-view/>
  </div>
</template>

<script>

  export default {
    mounted() {
      window.addEventListener("beforeDestory",()=>{
        sessionStorage.clear()
      })
    }
  }

</script>

<style>
  /deep/.el-table--border th.gutter:last-of-type {
    display: block!important;
    width: 17px!important;
  }

</style>




