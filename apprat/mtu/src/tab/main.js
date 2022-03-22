import { define } from '../../core.js'

const template = `<style>::slotted(:nth-child(1)){color:rgba(var(--color-accent));--color-icon:rgba(var(--color-accent))}:host([theme=dark]) ::slotted(:nth-child(1)){color:#fff;--color-icon:#fff}</style><style>:host{min-height:48px;display:flex;position:relative;justify-content:center;background:var(--color-background-bar);color:var(--color-text-secondary)}:host([theme=dark]){background:rgba(var(--color-primary));color:rgba(255,255,255,.7);--color-ripple:rgba(255,255,255,.4);--color-icon:rgba(255,255,255,.7)}:host([theme=dark]) .block{background:#fff}:host([mode=fixed]) .root{overflow:visible;width:100%}:host([mode=fixed]) .body{display:flex}:host([mode=fixed]) ::slotted(m-tab-item){flex-basis:100%;flex-shrink:1}.root{min-height:100%;overflow-x:auto;max-width:100%;position:relative;scrollbar-width:none}.root::-webkit-scrollbar{display:none}.body{display:inline-flex;position:relative;height:100%}.bar{height:2px;width:0;position:absolute;bottom:0;left:0;transform:translateX(0);transition:transform .2s;box-sizing:border-box}.block{background:rgba(var(--color-accent));height:100%}.bar.noAnime{transition:none}</style><div class="root" part="root"><div class="body"><slot></slot></div><div class="bar" part="bar"><div class="block" part="block"></div></div></div>`
const props = ['theme', 'mode', 'select']

const setup = (shadow, node) => {
  const style = shadow.querySelector('style')
  const styleText = style.textContent

  const root = shadow.querySelector('.root')
  const body = shadow.querySelector('.body')
  const bar = shadow.querySelector('.bar')
  const update = () => {
    const view = node.children[node.select]
    if (!view) return
    bar.style.transform = 'translateX(' + (view.offsetLeft - root.offsetLeft) + 'px)'
    bar.style.width = view.offsetWidth + 'px'
  }
  body.addEventListener('click', e => {
    if (e.target.tagName !== 'M-TAB-ITEM') return
    const v = [].slice.call(node.children).indexOf(e.target)
    const o = node.select
    node.select = v
    if (v !== o) node.dispatchEvent(new Event('change'))
  })
  new ResizeObserver(() => {
    bar.classList.add('noAnime')
    update()
    bar.classList.remove('noAnime')
  }).observe(body)
  return {
    onConnected: update,
    select: {
      get: 0,
      set: v => {
        style.textContent = styleText.replace(new RegExp(':nth-child\\(1\\)', 'g'), `:nth-child(${v + 1})`)
        update()
      }
    },
    theme: ['normal', 'dark'],
    mode: ['normal', 'fixed']
  }
}

define('tab', { template, props, setup })