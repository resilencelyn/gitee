import { define } from '../../core.js'
import '../ripple/main.js'

const template = `<style>:host{display:block;height:inherit;justify-content:center;align-items:center;min-width:72px;height:100%;cursor:pointer;flex-shrink:0;text-transform:uppercase}:host([disabled=true]){pointer-events:none;filter:grayscale(1);opacity:.6}.root{height:100%;display:flex;justify-content:center;align-items:center;padding:0 12px}::slotted([slot=start]){margin-right:8px}::slotted([slot=end]){margin-left:8px}::slotted(*){pointer-events:none}</style><m-ripple class="root" part="root"><slot name="start"></slot><span><slot></slot></span><slot name="end"></slot></m-ripple>`

const props = ['disabled', 'checked']
const setup = () => {
  return {
    disabled: false
  }
}

define('tab-item', { template, props, setup })