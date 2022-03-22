import { define } from '../../core.js'

const template = `<style>:host{display:block;height:18px;user-select:none}:host([disabled=true]){pointer-events:none;filter:grayscale(1);opacity:.6}:host([disabled=true]) input::-webkit-slider-thumb{pointer-events:none}:host([disabled=true]) input::-moz-range-thumb{pointer-events:none}.body{position:relative;height:100%}input{pointer-events:none;position:absolute;width:100%;height:100%;left:0;top:0;opacity:0;margin:0;appearance:none}input::-webkit-slider-thumb{pointer-events:auto;cursor:pointer}input::-moz-range-thumb{pointer-events:auto;cursor:pointer}.end{display:none}.root{position:relative;height:100%}.body{height:100%;display:flex;align-items:center;margin:0 6px;width:calc(100% - 12px);pointer-events:none}.body::before{content:'';height:2px;width:100%;background:rgba(var(--color-accent),.24)}.line{position:absolute;left:0;height:2px;width:0;background:rgba(var(--color-accent))}.block{position:absolute;left:0;display:flex;justify-content:center}.block:last-child{display:none}.bar{height:12px;width:12px;border-radius:50%;background:rgba(var(--color-accent));transition:transform .2s,box-shadow .2s}.block.active>.bar{transform:scale(1.5);box-shadow:0 0 0 6px rgba(var(--color-accent),.24)}.text{position:absolute;top:-48px;height:48px;width:32px;transform:scale(.4);filter:opacity(0);transition:transform .2s,filter .2s;transform-origin:bottom}.block.active .text{transform:scale(1);filter:opacity(1)}.text::after{position:relative;content:attr(value);display:flex;width:100%;height:32px;justify-content:center;align-items:center;color:#fff;font-size:.75rem}.text>span{width:100%;height:100%;position:absolute}.text>span::after,.text>span::before{content:'';width:100%;height:32px;background:rgba(var(--color-accent));position:absolute}.text>span::before{border-radius:50%}.text>span::after{top:11px;transform:rotate(45deg) scale(.5)}:host([range=true]) .block:last-child,:host([range=true]) .end{display:flex}@media(pointer:fine){.end:hover+.body>.block:last-child>.bar,.start:hover+.end+.body>.block:nth-child(2)>.bar{box-shadow:0 0 0 6px rgba(var(--color-accent),.24)}}</style><div class="root" part="root"><input type="range" class="start" max="100" value="0" step="1"> <input type="range" class="end" max="100" value="0" step="1"><div class="body" part="body"><div class="line" part="line"></div><div class="block"><div class="text" value="0" part="text"><span></span></div><div class="bar" part="bar"></div></div><div class="block"><div class="text" value="0" part="text"><span></span></div><div class="bar" part="bar"></div></div></div></div>`
const props = ['disabled', 'range', 'startvalue', 'endvalue', 'max', 'min', 'step']

const setup = (shadow, node) => {
  const [start, end] = shadow.querySelectorAll('input')
  const [startBlock, endBlock] = shadow.querySelectorAll('.body>.block')
  const line = shadow.querySelector('.line')
  const [startText, endText] = shadow.querySelectorAll('.text')
  const on = (type) => node.dispatchEvent(new Event(type))

  const update = () => {
    const getVal = v => ((Number(v) - Number(start.min)) * 100) / (Number(start.max) - Number(start.min))
    const startVal = getVal(start.value)
    const endVal = getVal(end.value)
    startBlock.style.left = startVal + '%'
    endBlock.style.left = endVal + '%'
    line.style.width = (node.range ? Math.abs(endVal - startVal) : startVal) + '%'
    line.style.left = (node.range ? Math.min(startVal, endVal) : 0) + '%'
    const to = v => {
      if (v > 1000000000) return ~~(v / 1000000000) + 'G'
      if (v > 1000000) return ~~(v / 1000000) + 'M'
      if (v > 1000) return ~~(v / 1000) + 'K'
      return v
    }
    startText.setAttribute('value', to(endVal > startVal ? node.startvalue : node.endvalue))
    endText.setAttribute('value', to(endVal > startVal ? node.endvalue : node.startvalue))
  }

  const value = {
    get start() {
      return Math.min(start.value, end.value)
    },
    set start(v) {
      start.value = v
      update()
    },
    get end() {
      return Math.max(start.value, end.value)
    },
    set end(v) {
      end.value = v
      update()
    }
  }

  const input = e => {
    e.stopPropagation()
    update()
    on('input')
  }

  start.addEventListener('input', input)
  end.addEventListener('input', input)
  start.addEventListener('change', () => on('change'))
  end.addEventListener('change', () => on('change'))
  const startDown = () => startBlock.classList.add('active')
  const startUp = () => startBlock.classList.remove('active')
  start.addEventListener('mousedown', startDown)
  start.addEventListener('mouseup', startUp)
  start.addEventListener('touchstart', startDown)
  start.addEventListener('touchend', startUp)
  const endDown = () => endBlock.classList.add('active')
  const endUp = () => endBlock.classList.remove('active')
  end.addEventListener('mousedown', endDown)
  end.addEventListener('mouseup', endUp)
  end.addEventListener('touchstart', endDown)
  end.addEventListener('touchend', endUp)
  const set = (type, v) => {
    start[type] = v
    end[type] = v
    update()
  }
  return {
    disabled: false,
    range: {
      get: false,
      set: update
    },
    startvalue: {
      get: () => value.start,
      set: v => value.start = v
    },
    endvalue: {
      get: () => value.end,
      set: v => value.end = v
    },
    max: {
      get: () => Number(start.max),
      set: v => set('max', v)
    },
    min: {
      get: () => Number(start.min),
      set: v => set('min', v)
    },
    step: {
      get: () => Number(start.step),
      set: v => set('step', v)
    }
  }
}

define('slider', { template, props, setup })