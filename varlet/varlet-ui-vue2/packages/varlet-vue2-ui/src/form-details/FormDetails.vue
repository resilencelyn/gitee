<template>
  <transition name="var-form-details">
    <div class="var-form-details" v-if="errorMessage || maxlengthText">
      <div class="var-form-details__message">{{ errorMessage }}</div>
      <div class="var-form-details__length">{{ maxlengthText }}</div>
    </div>
  </transition>
</template>

<script>
import { props } from './props'
import { defineComponent } from '../utils/create'

export default defineComponent({
  name: 'VarFormDetails',

  props,
})
</script>

<style lang="less">
@import '../styles/common';
@import './formDetails';
</style>
