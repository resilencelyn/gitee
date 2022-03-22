import { define } from '../../core.js'

const template = `<style>::slotted(:nth-child(1)){color:rgba(var(--color-accent));--color-icon:rgba(var(--color-accent));font-size:.875rem}:host([theme=dark]) ::slotted(:nth-child(1)){color:#fff;--color-icon:#fff}</style><style>:host{display:flex;background:var(--color-background-card);user-select:none;height:56px;cursor:pointer;justify-content:center;color:var(--color-text-secondary);font-size:.75rem}:host([theme=dark]){background:rgba(var(--color-accent));color:rgba(255,255,255,.7);--color-ripple:rgba(255, 255, 255, .4);--color-icon:rgba(255, 255, 255, .7)}.root{display:flex;justify-content:center;height:100%;width:100%}</style><div class="root" part="root"><slot></slot></div>`
const props = ['select']
const setup = (shadow, node) => {
  const root = shadow.querySelector('.root')
  const style = shadow.querySelector('style')
  const styleText = style.textContent
  root.addEventListener('click', e => {
    const path = []
    for (const item of e.composedPath()) item.tagName && path.push(item.tagName)
    if (!path.includes('M-BOTTOM-NAV-ITEM')) return
    const v = [].slice.call(node.children).indexOf(e.target)
    if (v !== node.select) node.dispatchEvent(new Event('change'))
    node.select = v
  })
  return {
    select: {
      get: 0,
      set: v => style.textContent = styleText.replace(new RegExp(':nth-child\\(1\\)', 'g'), `:nth-child(${v + 1})`)
    },
    theme: ['normal', 'dark']
  }
}

define('bottom-nav', { template, props, setup })