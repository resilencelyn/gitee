import { define } from '../../core.js'
import './main.js'
import '../ripple/main.js'

const template = `<style>:host{display:inline-block;vertical-align:middle}:host([state=true]) .root{opacity:1;pointer-events:auto}:host([state=true]) .body{transform:scale(1)}.root{z-index:3;position:fixed;top:0;left:0;display:flex;justify-content:center;align-items:center;width:100%;height:100%;padding:24px;box-sizing:border-box;opacity:0;pointer-events:none;transition:opacity .2s}.mask{position:absolute;top:0;left:0;width:100%;height:100%;background:rgba(0,0,0,.6)}.body{background:var(--color-background-card);box-shadow:0 11px 15px -7px rgba(0,0,0,.2),0 24px 38px 3px rgba(0,0,0,.14),0 9px 46px 8px rgba(0,0,0,.12);border-radius:2px;width:100%;max-width:392px;display:flex;flex-direction:column;max-height:100%;position:relative;transition:transform .2s;transform:scale(.9)}m-date{box-shadow:none;max-width:none}.action{display:flex;margin:8px 12px 12px 12px;justify-content:flex-end;flex-wrap:wrap}.action>m-ripple{overflow:hidden;height:36px;min-width:48px;display:flex;align-items:center;justify-content:center;cursor:pointer;color:rgba(var(--color-accent));padding:0 8px;border-radius:2px;user-select:none;font-weight:500;text-transform:uppercase;--color-ripple:rgba(var(--color-accent),.24)}.action>m-ripple+m-ripple{margin-left:8px}</style><slot></slot><div class="root" part="root"><div class="mask"></div><div class="body"><m-date></m-date><div class="action"><m-ripple class="cancel">取消</m-ripple><m-ripple class="confirm">确定</m-ripple></div></div></div>`
const props = ['state', 'value', 'min', 'max']

const setup = (shadow, node) => {
  const mask = shadow.querySelector('.mask')
  const slot = shadow.querySelector('slot')
  const date = shadow.querySelector('m-date')
  const cancel = shadow.querySelector('.cancel')
  const confirm = shadow.querySelector('.confirm')

  slot.addEventListener('click', () => node.state = true)
  const close = () => node.state = false
  mask.addEventListener('click', close)
  cancel.addEventListener('click', close)
  confirm.addEventListener('click', () => {
    close()
    if (node.value !== date.value) {
      node.value = date.value
      node.dispatchEvent(new Event('change'))
    }
  })

  return {
    state: false,
    min: {
      get: date.min,
      set: v => date.min = v
    },
    max: {
      get: date.max,
      set: v => date.max = v
    },
    value: {
      get: date.value,
      set: v => date.value = v
    }
  }
}

define('date-picker', { template, props, setup })