import { define } from '../../core.js'

const template = `<style>:host{z-index:2;display:flex;justify-content:center;background:var(--color-background-bar);position:relative;box-shadow:0 2px 4px -1px rgba(0,0,0,.2),0 4px 5px 0 rgba(0,0,0,.14),0 1px 10px 0 rgba(0,0,0,.12);user-select:none}:host([theme=dark]){background:rgba(var(--color-primary))}:host([theme=dark]) .nav{--color-icon:#fff;--color-ripple:rgba(255,255,255,.4)}:host([theme=dark]) .text{color:#fff;--color-text-secondary:rgba(255,255,255,.7)}:host([theme=dark]) .view{color:#fff;--color-ripple:rgba(255,255,255,.4)}:host([theme=dark]) ::slotted([slot=menu]){--menu-active-icon:#fff;--menu-active-ripple:rgba(255,255,255,.4)}.root{display:flex;height:inherit;align-items:center;min-height:56px;width:100%}.nav{flex-shrink:0}::slotted([slot=logo]){height:24px;margin-left:16px;flex-shrink:0}.text{margin:0 16px;letter-spacing:.0625rem;overflow:hidden;white-space:nowrap}::slotted([slot=title]),::slotted([slot=subtitle]){overflow:hidden;text-overflow:ellipsis;font-size:1.125rem;line-height:1}::slotted([slot=subtitle]){margin-top:4px;font-size:.875rem;color:var(--color-text-secondary)}.view{flex-grow:1;display:flex;justify-content:flex-end}.menu{flex-shrink:0;margin-right:8px}::slotted([slot=nav]){margin:8px 0 8px 8px}@media(min-width:720px){:host([theme=dark]) ::slotted([slot=menu][mode|=auto]){--color-icon:#fff;--color-ripple:rgba(255,255,255,.4);color:#fff}}</style><div class="root" part="root"><div class="nav"><slot name="nav"></slot></div><slot name="logo"></slot><div class="text" part="text"><slot name="title"></slot><slot name="subtitle"></slot></div><div class="view" part="view"><slot></slot></div><div class="menu" part="menu"><slot name="menu"></slot></div><div><slot name="end"></slot></div></div>`
const props = ['theme']

const setup = () => {
  return {
    theme: ['normal', 'dark']
  }
}

define('appbar', { template, props, setup })