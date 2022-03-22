import { define } from '../../core.js'
import './icon.css'

const template = `<style>:host{font-family:'Material Icons',fantasy;width:24px;height:24px;overflow:hidden;font-weight:normal;user-select:none;color:var(--color-icon);font-size:24px;line-height:normal;letter-spacing:normal;text-transform:none;display:inline-block;white-space:nowrap;word-wrap:normal;direction:ltr;-webkit-font-feature-settings:'liga';-webkit-font-smoothing:antialiased}</style><div translate="no">home</div>`
const props = ['value']

const setup = shadow => {
  const root = shadow.querySelector('div')
  return {
    value: {
      get: () => root.innerHTML,
      set: v => root.innerHTML = v
    }
  }
}

define('icon', { template, props, setup })