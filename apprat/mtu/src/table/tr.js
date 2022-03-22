import { define } from '../../core.js'

const template = `<style>:host{display:table-row}:host(:not(:last-of-type)){border-bottom:1px solid var(--color-border)}::slotted(div){width:32px;padding-left:12px;display:table-cell;vertical-align:middle}</style><slot></slot>`

define('tr', { template })