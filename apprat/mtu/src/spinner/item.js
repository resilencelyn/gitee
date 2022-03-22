import { define } from '../../core.js'
import '../ripple/main.js'

const template = `<style>:host{display:block;height:40px;flex-shrink:0}:host([disabled=true]){opacity:.6;pointer-events:none;filter:grayscale(1)}.root{height:100%;display:flex;align-items:center;padding:0 16px;cursor:pointer}span{white-space:nowrap;overflow:hidden;text-overflow:ellipsis}::slotted([slot=start]){margin-right:12px}</style><m-ripple class="root" part="root"><slot name="start"></slot><span><slot></slot></span></m-ripple>`
const props = ['disabled']

const setup = () => {
  return {
    disabled: false
  }
}

define('spinner-item', { template, props, setup })