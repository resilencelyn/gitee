import { define } from '../../core.js'

const template = `<style>:host{display:block;padding:8px 0}:host(:not(:nth-of-type(1))){border-top:solid 1px var(--color-border)}::slotted([slot=title]){margin-top:-8px;height:48px;display:flex;align-items:center;padding:0 16px;color:var(--color-text-secondary);user-select:none}</style><div class="root" part="root"><slot name="title"></slot><slot></slot></div>`

define('list', { template })