import { define } from '../../core.js'
import '../ripple/main.js'

const template = `<style>:host{display:block;flex-basis:100%;height:100%;max-width:124px;transition:font-size .2s}:host([disabled=true]){pointer-events:none;filter:grayscale(1);opacity:.6}.root{height:100%;display:flex;align-items:center;justify-content:center;flex-direction:column}slot{pointer-events:none}::slotted(m-icon){margin-bottom:2px}</style><m-ripple class="root" part="root"><slot></slot></m-ripple>`
const props = ['disabled']
const setup = () => {
  return {
    disabled: false
  }
}

define('bottom-nav-item', { template, setup, props })