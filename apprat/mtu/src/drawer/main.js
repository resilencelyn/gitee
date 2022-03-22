import { define } from '../../core.js'

const template = `<style>:host{display:block;height:100%;--drawer-start-width:260px}.root{display:flex;position:relative;height:100%;justify-content:flex-end;overflow:hidden}.content{width:calc(100% - var(--drawer-start-width));flex-shrink:0;transition:width .2s}::slotted(*){height:100%;display:block}.mask{z-index:2;position:absolute;width:100%;height:100%;left:0;top:0;pointer-events:none;display:none;background:rgba(0,0,0,.6);filter:opacity(0)}.start{z-index:2;background:var(--color-background-card);flex-shrink:0;position:absolute;width:var(--drawer-start-width);left:0;height:100%}.start>.shadow{box-shadow:0 8px 10px -5px rgba(0,0,0,.2),0 16px 24px 2px rgba(0,0,0,.14),0 6px 30px 5px rgba(0,0,0,.12);pointer-events:none;position:absolute;width:100%;height:100%;left:0;top:0;filter:opacity(0)}.start>.left{position:relative;height:100%}@media(min-width:720px){.start{transition:width .2s;overflow:hidden;white-space:nowrap}:host([start=false]) .start{width:0}:host([start=false]) .content{width:100%}}@media(max-width:720px){.mask{transition:filter .2s;display:block}.content{width:100%}.start{position:absolute;top:0;left:0;height:100%;transform:translateX(-100%)}.start>.left{pointer-events:none}.noAnime .start{pointer-events:none}.root:not(.noAnime) .start{transition:transform .2s}.root:not(.noAnime) .start>.shadow{transition:filter .2s}:host([mobile_start=true]) .start{transform:translateX(0%)}:host([mobile_start=true]) .mask{filter:opacity(1);pointer-events:auto}:host([mobile_start=true]) .start>.shadow{filter:opacity(1)}:host([mobile_start=true]) .start>.left{pointer-events:auto}}</style><div class="root"><div class="content" part="content"><slot></slot></div><div class="mask" part="mask"></div><div class="start" part="start"><div class="shadow"></div><div class="left"><slot name="start"></slot></div></div></div>`

const props = ['start', 'mobile_start']

const setup = (shadow, node) => {
  const root = shadow.querySelector('.root')
  const start = shadow.querySelector('.start')
  const startShadow = shadow.querySelector('.start>.shadow')
  const mask = shadow.querySelector('.mask')
  mask.addEventListener('click', () => node.toggle())

  let info = null

  //记录首次
  const firstMove = (x, y) => {
    if ((node.mobile_start && x > start.offsetWidth) || (!node.mobile_start && x > 24)) return
    info = { state: 0, width: start.offsetWidth, x, y, timestamp: Date.now() }
  }

  const touchMove = e => {
    const { pageX, pageY } = e.changedTouches[0]
    const x = pageX - node.offsetLeft
    const y = pageY - node.offsetTop
    if (info === null) return firstMove(x, y)
    if (info.state === 0) {
      const ov = Math.abs(x - info.x) > Math.abs(y - info.y)
      info.state = ov ? 1 : null
      if (ov) root.classList.add('noAnime')
    }
    if (info.state !== 1) return
    const xx = x - info.x
    let move = node.mobile_start ? xx : -info.width + xx
    if (move > 0) move = 0
    if (move < -info.width) move = -info.width
    start.style.transform = 'translateX(' + move + 'px)'
    const opacity = (move + info.width) / info.width
    startShadow.style.filter = `opacity(${opacity})`
    mask.style.filter = `opacity(${opacity})`
  }

  const touchEnd = e => {
    const infos = info
    info = null
    root.classList.remove('noAnime')
    if (infos === null || infos.state !== 1) return
    const { pageX } = e.changedTouches[0]
    const xx = (pageX - node.offsetLeft) - infos.x
    const time = Date.now() - infos.timestamp
    let v = node.mobile_start
    if (!node.mobile_start) {
      if ((time < 300 && xx > 40) || (time > 300 && xx > (infos.width / 2))) v = true
    } else {
      if ((time < 300 && xx < -40) || (time > 300 && xx < -(infos.width / 2))) v = false
    }
    mask.removeAttribute('style')
    start.removeAttribute('style')
    startShadow.removeAttribute('style')
    node.mobile_start = v
  }

  node.addEventListener('touchmove', touchMove, { passive: true })
  node.addEventListener('touchend', touchEnd)
  node.addEventListener('touchcancel', touchEnd)
  node.toggle = () => {
    if (window.innerWidth >= 720) return node.start = !node.start
    node.mobile_start = !node.mobile_start
  }
  return {
    start: true,
    mobile_start: false
  }
}

define('drawer', { template, props, setup })