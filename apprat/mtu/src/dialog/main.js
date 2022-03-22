import { global, define } from '../../core.js'
import '../ripple/main.js'
import { animationEnd } from '../../util.js'

const template = `<style>:host{display:inline-block;vertical-align:middle}.root{z-index:3;display:flex;position:fixed;top:0;left:0;width:100%;height:100%;justify-content:center;align-items:center;padding:24px;box-sizing:border-box;pointer-events:none;opacity:0;transition:opacity .2s}:host([state=true]) .root{opacity:1;pointer-events:auto}.mask{position:absolute;top:0;left:0;width:100%;height:100%;background:rgba(0,0,0,.6)}@keyframes open{0%{transform:scale(.9)}100%{transform:scale(1)}}@keyframes close{0%{transform:scale(1)}100%{transform:scale(.9)}}.body{background:var(--color-background-card);box-shadow:0 11px 15px -7px rgba(0,0,0,.2),0 24px 38px 3px rgba(0,0,0,.14),0 9px 46px 8px rgba(0,0,0,.12);border-radius:2px;width:100%;max-width:392px;display:flex;flex-direction:column;max-height:100%;position:relative;animation:close .2s}:host([state=true]) .body{animation:open .2s}:host([size=full]) .root{padding:0}:host([size=full]) .body{max-width:none;height:100%;border-radius:0}:host([size=full]) .content{flex-grow:1}:host([size=large]) .body{max-width:960px}.title{font-size:1.25rem;font-weight:500;letter-spacing:1px;user-select:none;word-break:break-all;flex-shrink:0;margin:24px 24px 20px 24px}.title:empty{display:none}.title:not(:empty)+.content>.message{margin-top:0}.content{overflow:auto;scrollbar-width:none}.message{line-height:1.6;white-space:pre-wrap;margin:24px}.message:empty{display:none}.message::selection{background:rgba(var(--color-accent));color:#fff}.content::-webkit-scrollbar{display:none}.action{flex-shrink:0;display:flex;justify-content:flex-end;margin:12px;flex-wrap:wrap}.action>.button{height:36px;min-width:48px;display:flex;align-items:center;justify-content:center;cursor:pointer;color:rgba(var(--color-accent));padding:0 8px;border-radius:2px;overflow:hidden;user-select:none;font-size:.9rem;text-transform:uppercase;--color-ripple:rgba(var(--color-accent),.14)}.action>.button:empty{display:none}:host(:not([positive]):not([negative])) .action,:host([positive=""]):host([negative=""]) .action{display:none}.action>.button+.button{margin-left:8px}</style><slot name="active"></slot><div class="root" part="root" title=""><div class="mask"></div><div class="body" part="body"><div class="title" part="title"></div><div class="content" part="content"><div class="message"></div><slot></slot></div><div class="action"><m-ripple class="button negative" part="button"></m-ripple><m-ripple class="button positive" part="button"></m-ripple></div></div></div>`
const props = ['state', 'size', 'title', 'message', 'positive', 'negative']

const setup = (shadow, node) => {
  const active = shadow.querySelector('[name=active]')
  const body = shadow.querySelector('.body')
  const title = shadow.querySelector('.title')
  const message = shadow.querySelector('.message')
  const positive = shadow.querySelector('.positive')
  const negative = shadow.querySelector('.negative')
  const mask = shadow.querySelector('.mask')
  active.addEventListener('click', () => node.state = true)
  const close = () => {
    node.state = false
    animationEnd(body, () => node.dispatchEvent(new Event('close')))
  }
  mask.addEventListener('touchstart', close)
  mask.addEventListener('click', close)
  positive.addEventListener('click', () => {
    close()
    node.dispatchEvent(new Event('positive'))
  })
  negative.addEventListener('click', () => {
    close()
    node.dispatchEvent(new Event('negative'))
  })
  return {
    state: false,
    size: ['normal', 'large', 'full'],
    title: {
      get: () => title.innerText,
      set: v => title.innerText = v
    },
    message: {
      get: () => message.innerText,
      set: v => message.innerText = v
    },
    positive: {
      get: () => positive.innerText,
      set: v => positive.innerText = v,
    },
    negative: {
      get: () => negative.innerText,
      set: v => negative.innerText = v,
    }
  }
}

define('dialog', { template, props, setup })

global.dialog = ({ title = '', message = '', positive = '', negative = '', onPositive = null, onNegative = null, onClose = null, size = 'none' }) => {
  const dom = document.createElement('m-dialog')
  dom.style.display = 'inline'
  dom.title = title
  dom.message = message
  dom.positive = positive
  dom.negative = negative
  dom.size = size
  dom.addEventListener('positive', e => {
    onPositive && onPositive(e)
  })
  dom.addEventListener('negative', e => {
    onNegative && onNegative(e)
  })
  dom.addEventListener('close', () => {
    document.body.removeChild(dom)
    onClose && onClose()
  })
  document.body.appendChild(dom)
  window.getComputedStyle(dom).top
  dom.state = true
}