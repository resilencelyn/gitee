<template>
  <div>
    <svg-icon icon-class="github" @click="goto"/>
  </div>
</template>

<script>
export default {
  name: 'YunZeGit',
  data() {
    return {
      url: 'https://gitee.com/sdyunze/iotlink'
    }
  },
  methods: {
    goto() {
      window.open(this.url)
    }
  }
}
</script>
