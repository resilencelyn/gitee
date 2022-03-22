import { define } from '../../core.js'
import '../ripple/main.js'

const template = `<style>:host{display:inline-block;vertical-align:middle}.root{display:flex;flex-wrap:wrap;justify-content:center;margin:-4px}.item.more>svg{fill:var(--color-text);width:18px;height:18px}.item{cursor:pointer;height:32px;width:32px;display:flex;justify-content:center;align-items:center;overflow:hidden;border-radius:50%;margin:4px}.item[num]:empty:after{content:attr(num)}.item.disabled{pointer-events:none;filter:grayscale(1);opacity:.6}.item>svg{fill:var(--color-icon);height:24px;width:24px;pointer-events:none}.item.checked{background:rgba(var(--color-accent));color:#fff;box-shadow:0 3px 1px -2px rgb(0 0 0 / 20%),0 2px 2px 0 rgb(0 0 0 / 14%),0 1px 5px 0 rgb(0 0 0 / 12%);pointer-events:none;--color-ripple:rgba(255, 255, 255, .4)}</style><div class="root" part="root"><m-ripple class="item prev" part="block"><svg viewBox="0 0 24 24"><path d="M15.41 7.41L14 6l-6 6 6 6 1.41-1.41L10.83 12z"></path></svg></m-ripple><m-ripple class="item" num="1" part="block"></m-ripple><m-ripple class="item more" part="block"><svg viewBox="0 0 24 24"><path d="M6 10c-1.1 0-2 .9-2 2s.9 2 2 2 2-.9 2-2-.9-2-2-2zm12 0c-1.1 0-2 .9-2 2s.9 2 2 2 2-.9 2-2-.9-2-2-2zm-6 0c-1.1 0-2 .9-2 2s.9 2 2 2 2-.9 2-2-.9-2-2-2z"></path></svg></m-ripple><m-ripple class="item but" part="block"></m-ripple><m-ripple class="item but" part="block"></m-ripple><m-ripple class="item but" part="block"></m-ripple><m-ripple class="item more" part="block"><svg viewBox="0 0 24 24"><path d="M6 10c-1.1 0-2 .9-2 2s.9 2 2 2 2-.9 2-2-.9-2-2-2zm12 0c-1.1 0-2 .9-2 2s.9 2 2 2 2-.9 2-2-.9-2-2-2zm-6 0c-1.1 0-2 .9-2 2s.9 2 2 2 2-.9 2-2-.9-2-2-2z"></path></svg></m-ripple><m-ripple class="item last" part="block"></m-ripple><m-ripple class="item next" part="block"><svg viewBox="0 0 24 24"><path d="M10 6L8.59 7.41 13.17 12l-4.58 4.59L10 18l6-6z"></path></svg></m-ripple></div>`
const props = ['total', 'value', 'row']
const setup = (shadow, node) => {
  const root = shadow.querySelector('.root')
  const [prev, next] = shadow.querySelectorAll('.prev,.next')
  const last = shadow.querySelector('.last')
  const [morePrev, moreNext] = shadow.querySelectorAll('.more')
  const but = shadow.querySelectorAll('.but')

  const update = () => {
    const { total = 1, value = 1, row = 20 } = node
    const max = Math.ceil(total / row)
    if (value > max) return node.value = 1
    let min = Math.max(Math.min(value - 1, max - 3), 2)
    for (const item of but) {
      const v = min++
      item.style.display = max < 6 && v >= max ? 'none' : ''
      item.setAttribute('num', v)
    }
    prev.classList[value === 1 ? 'add' : 'remove']('disabled')
    next.classList[value === max ? 'add' : 'remove']('disabled')
    morePrev.style.display = max < 6 || value <= 3 ? 'none' : ''
    moreNext.style.display = max < 6 || value >= max - 2 ? 'none' : ''
    morePrev.setAttribute('num', value - 3)
    moreNext.setAttribute('num', value + 3)
    root.querySelector('.checked')?.classList.remove('checked')
    root.querySelector('.item[num="' + value + '"]')?.classList.add('checked')
    last.setAttribute('num', max)
    last.style.display = max === 1 ? 'none' : ''
  }
  root.addEventListener('click', e => {
    const target = e.target
    if (target.tagName !== 'M-RIPPLE') return
    let v = Number(target.getAttribute('num'))
    if (target === prev) v = node.value - 1
    if (target === next) v = node.value + 1
    node.value = v
    node.dispatchEvent(new Event('change'))
  })
  update()
  return {
    total: {
      get: 1,
      set: update
    },
    value: {
      get: 1,
      set: update
    },
    row: {
      get: 20,
      set: update
    }
  }
}

define('pagination', { template, props, setup })