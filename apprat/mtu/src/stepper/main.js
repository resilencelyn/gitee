import { define } from '../../core.js'

const template = `<style>::slotted(:nth-child(-n+1)){--stepper-item-color:rgba(var(--color-accent));--stepper-item-done-display:block;--stepper-item-start-display:none;color:var(--color-text)}::slotted(:nth-child(1)){--stepper-item-done-display:none;--stepper-item-start-display:flex}</style><style>:host{display:flex;background:var(--color-background-card);justify-content:center;box-shadow:0 2px 1px -1px rgb(0 0 0 / 20%),0 1px 1px 0 rgb(0 0 0 / 14%),0 1px 3px 0 rgb(0 0 0 / 12%);overflow:hidden}</style><slot></slot>`
const props = ['value']

const setup = (shadow) => {
  const style = shadow.querySelector('style')
  const styleText = style.textContent
  return {
    value: {
      get: 0,
      set: v => style.textContent = styleText.replace(new RegExp('1', 'g'), v + 1)
    }
  }
}

define('stepper', { template, props, setup })