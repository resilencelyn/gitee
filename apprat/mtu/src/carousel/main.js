import { define } from '../../core.js'

const template = `<style>:host{display:flex;height:196px;background:#eee;overflow:hidden;border-radius:2px;position:relative;justify-content:center;align-items:center}.root{display:flex;height:100%;width:100%;flex-shrink:0;transform:translateX(0%);transition:transform .4s}.bar{position:absolute;bottom:16px;display:flex}.bar>div{width:8px;height:8px;background:rgba(255,255,255,.4);border-radius:50%;margin:0 4px;cursor:pointer;box-shadow:0 1px 3px rgba(0,0,0,.14)}.bar>div.checked{background:rgba(var(--color-accent))}</style><div class="root" part="root"><slot></slot></div><div class="bar" part="bar"></div>`
const props = ['select', 'autoplay', 'interval']

const setup = (shadow, node) => {
  const root = shadow.querySelector('.root')
  const bar = shadow.querySelector('.bar')
  const slot = shadow.querySelector('slot')

  const change = () => {
    if (node.children.length - 1 < node.select) return node.select = 0
    const v = (node.select) * 100
    root.style.transform = 'translateX(-' + v + '%)'
    bar.querySelector('.checked').classList.remove('checked')
    bar.children[node.select].classList.add('checked')
  }

  let inter = null
  const clear = () => clearInterval(inter)

  const start = () => {
    if (!node.isConnected || !node.autoplay) return
    clear()
    inter = setInterval(() => node.select++, node.interval)
  }

  const update = () => {
    bar.innerHTML = ''
    const fragment = document.createDocumentFragment()
    let i = 0
    for (const item of node.children) {
      const view = document.createElement('div')
      view.setAttribute('part', 'block')
      const key = i
      view.addEventListener('click', () => node.select = key)
      fragment.appendChild(view)
      i++
    }
    bar.appendChild(fragment)
    bar.children[0] && bar.children[0].classList.add('checked')
    start()
  }

  slot.addEventListener('slotchange', update)

  return {
    onConnected: start,
    onDisconnected: clear,
    select: {
      get: 0,
      set: change
    },
    autoplay: {
      get: true,
      set: v => v ? start() : clear(),
    },
    interval: {
      get: 3000,
      set: () => start()
    }
  }
}

define('carousel', { template, props, setup })