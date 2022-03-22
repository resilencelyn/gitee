import { define } from '../../core.js'
import '../ripple/main.js'

const template = `<style>:host{display:inline-flex;align-items:center;vertical-align:middle;position:relative;cursor:pointer;user-select:none;line-height:1.6;font-size:1rem}.root{width:40px;height:40px;border-radius:50%;overflow:hidden;justify-content:center;align-items:center;display:flex}:host([disabled=true]){pointer-events:none;filter:grayscale(1);opacity:.6}:host([checked=true]) .root{--color-ripple:rgba(var(--color-accent),.24)}.icon{width:24px;height:24px;fill:var(--color-text-secondary)}.def{filter:opacity(1);transition:filter .2s}.full{position:absolute;filter:opacity(0);fill:rgba(var(--color-accent));transform:scale(.4);transition:filter .2s,transform .2s}:host([checked=true]) .def{filter:opacity(0);fill:rgba(var(--color-accent))}:host([checked=true]) .full{filter:opacity(1);transform:scale(1)}</style><m-ripple class="root" part="root"><svg viewBox="0 0 24 24" class="icon def"><path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm0 18c-4.42 0-8-3.58-8-8s3.58-8 8-8 8 3.58 8 8-3.58 8-8 8z"></path></svg><svg viewBox="0 0 24 24" class="icon full"><path d="M12 7c-2.76 0-5 2.24-5 5s2.24 5 5 5 5-2.24 5-5-2.24-5-5-5zm0-5C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm0 18c-4.42 0-8-3.58-8-8s3.58-8 8-8 8 3.58 8 8-3.58 8-8 8z"></path></svg></m-ripple><slot></slot>`
const props = ['disabled', 'checked', 'value', 'name']

const setup = (shadow, node) => {
  node.addEventListener('click', () => {
    if (node.checked === true) return
    node.checked = true
    node.dispatchEvent(new Event('change'))
    if (node.hasAttribute('name') === false) return
    const all = document.querySelectorAll('m-radio[name=' + node.getAttribute('name') + ']')
    for (const item of all) {
      if (item === node) continue
      item.checked = false
    }
  })
  return {
    disabled: false,
    checked: false,
    value: '',
    name: ''
  }
}

define('radio', { template, props, setup })