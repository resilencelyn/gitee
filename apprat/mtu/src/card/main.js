import { define } from '../../core.js'

const template = `<style>:host{background:var(--color-background-card);box-shadow:0 2px 1px -1px rgb(0 0 0 / 20%),0 1px 1px 0 rgb(0 0 0 / 14%),0 1px 3px 0 rgb(0 0 0 / 12%);border-radius:4px;display:inline-block;width:100%;max-width:320px;box-sizing:border-box}:host([theme=outlined]){box-shadow:none;border:solid 1px var(--color-border)}[name=media]{border-radius:inherit}::slotted([slot=media]){border-top-left-radius:inherit;border-top-right-radius:inherit;width:100%;user-select:none}::slotted([slot=media]:only-child){border-bottom-left-radius:inherit;border-bottom-right-radius:inherit}.primary{user-select:none}::slotted([slot=title]){margin:16px 16px 0 16px;font-size:1.25rem;line-height:1.6}::slotted([slot=subtitle]){margin:0 16px;color:var(--color-text-secondary)}::slotted([slot=content]){margin:16px 16px 0 16px;color:var(--color-text-secondary);line-height:1.6}::slotted([slot=action]){margin:8px;font-size:0;letter-spacing:8px;user-select:none}::slotted([slot]:not([slot=action]):not([slot=media]):last-child){margin-bottom:16px}</style><slot name="media"></slot><div class="primary"><slot name="title"></slot><slot name="subtitle"></slot></div><slot name="content"></slot><slot></slot><slot name="action"></slot>`
const props = ['theme']

const setup = () => {
  return {
    theme: ['normal', 'outlined']
  }
}

define('card', { template, props, setup })