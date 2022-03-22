import { define } from '../../core.js'

const template = `<style>:host{display:inline-block;vertical-align:middle;position:relative;height:48px;user-select:none}:host([disabled=true]){pointer-events:none;filter:grayscale(1);opacity:.4}input{height:100%;width:100%;margin:0;cursor:pointer;opacity:0;position:absolute}.icon{display:none}svg{height:100%;fill:inherit;flex-shrink:0}div{height:inherit;display:flex;pointer-events:none}.line{fill:var(--color-border)}.bar{position:absolute;left:0;top:0;overflow:hidden;fill:rgba(var(--color-accent));width:50%}</style><svg viewBox="0 0 1024 1024" class="icon"><path id="icon" d="M512 736.853333L775.68 896l-69.973333-299.946667L938.666667 394.24l-306.773334-26.453333L512 85.333333 392.106667 367.786667 85.333333 394.24l232.533334 201.813333L248.32 896 512 736.853333z"></path></svg><input type="range" value="5" max="10" min="0" step="1"><div class="line"><svg viewBox="0 0 1024 1024"><use xlink:href="#icon"></use></svg><svg viewBox="0 0 1024 1024"><use xlink:href="#icon"></use></svg><svg viewBox="0 0 1024 1024"><use xlink:href="#icon"></use></svg><svg viewBox="0 0 1024 1024"><use xlink:href="#icon"></use></svg><svg viewBox="0 0 1024 1024"><use xlink:href="#icon"></use></svg></div><div class="bar"><svg viewBox="0 0 1024 1024"><use xlink:href="#icon"></use></svg><svg viewBox="0 0 1024 1024"><use xlink:href="#icon"></use></svg><svg viewBox="0 0 1024 1024"><use xlink:href="#icon"></use></svg><svg viewBox="0 0 1024 1024"><use xlink:href="#icon"></use></svg><svg viewBox="0 0 1024 1024"><use xlink:href="#icon"></use></svg></div>`
const props = ['disabled', 'value', 'max', 'min', 'step']

const setup = (shadow, node) => {
  const input = shadow.querySelector('input')
  const bar = shadow.querySelector('.bar')
  const on = t => node.dispatchEvent(new Event(t))
  const update = () => {
    const v = ((input.value - input.min) * 100) / (input.max - input.min)
    bar.style.width = v + '%'
  }

  input.addEventListener('input', e => {
    e.stopPropagation()
    update()
    on('input')
  })
  input.addEventListener('change', () => on('change'))

  return {
    disabled: false,
    value: {
      get: () => Number(input.value),
      set: v => {
        input.value = v
        update()
      }
    },
    max: {
      get: () => Number(input.max),
      set: v => {
        input.max = v
        update()
      }
    },
    min: {
      get: () => Number(input.min),
      set: v => {
        input.min = v
        update()
      }
    },
    step: {
      get: () => Number(input.step),
      set: v => {
        input.step = v
        update()
      }
    },
  }
}

define('rating', { template, props, setup })