import { define } from '../../core.js'
import '../ripple/main.js'

const template = `<style>:host{display:inline-flex;align-items:center;vertical-align:middle;position:relative;user-select:none;cursor:pointer;line-height:1.6;font-size:1rem}:host([disabled=true]){pointer-events:none;filter:grayscale(1);opacity:.6}.root{border-radius:50%;overflow:hidden;width:40px;height:40px;justify-content:center;align-items:center;display:flex;position:relative}.icon{border:solid 2px var(--color-text-secondary);border-radius:2px;height:18px;width:18px;box-sizing:border-box}:host([checked=true]) .icon{border-color:rgba(var(--color-accent))}.icon-full,.un{position:absolute;width:24px;height:24px;fill:rgba(var(--color-accent));transition:transform .2s,filter .2s;filter:opacity(0);transform:scale(.4)}:host([checked=true]) .icon-full,:host([indeterminate=true]) .un{filter:opacity(1);transform:scale(1)}:host([checked=true]) .root{--color-ripple:rgba(var(--color-accent),.24)}</style><m-ripple class="root" part="root"><div class="icon"></div><svg viewBox="0 0 24 24" class="icon-full"><path d="M19 3H5c-1.11 0-2 .9-2 2v14c0 1.1.89 2 2 2h14c1.11 0 2-.9 2-2V5c0-1.1-.89-2-2-2zm-9 14l-5-5 1.41-1.41L10 14.17l7.59-7.59L19 8l-9 9z"></path></svg><svg class="un" viewBox="0 0 1024 1024"><path d="M725.333333 554.666667H298.666667v-85.333334h426.666666m85.333334-341.333333H213.333333c-47.36 0-85.333333 37.973333-85.333333 85.333333v597.333334a85.333333 85.333333 0 0 0 85.333333 85.333333h597.333334a85.333333 85.333333 0 0 0 85.333333-85.333333V213.333333a85.333333 85.333333 0 0 0-85.333333-85.333333z"></path></svg></m-ripple><slot></slot>`
const props = ['disabled', 'checked', 'value', 'name', 'indeterminate']

const setup = (shadow, node) => {
  node.addEventListener('click', () => {
    node.checked = !node.checked
    node.dispatchEvent(new Event('change'))
  })
  return {
    disabled: false,
    checked: {
      get: false,
      set: () => node.indeterminate && (node.indeterminate = false)
    },
    value: '',
    name: '',
    indeterminate: false
  }
}

define('checkbox', { template, props, setup })