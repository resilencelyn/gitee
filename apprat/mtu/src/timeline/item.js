import { define } from '../../core.js'

const template = `<style>:host{display:block}.root{display:flex}.linear{display:flex;flex-direction:column;align-items:center}slot[name=icon]{display:flex;align-items:center;min-height:24px}.icon{width:12px;height:12px;border-radius:50%;background:#bdbdbd}.linear::after{content:'';flex-grow:1;display:block;width:1px;background:var(--color-border)}.body{flex-grow:1}.title{flex-shrink:0}::slotted([slot=title]){min-height:24px;display:flex;align-items:center;margin-right:8px;color:var(--color-text-secondary);justify-content:flex-end;user-select:none}::slotted(:not([slot])){margin-left:16px;margin-bottom:16px}::slotted(m-card:not([slot])){line-height:1.6;padding:12px}:host(:last-of-type) ::slotted(:not([slot])){margin-bottom:0}</style><div class="root" part="root"><div class="title"><slot name="title"></slot></div><div class="linear"><slot name="icon"><div class="icon" part="icon"></div></slot></div><div class="body" part="body"><slot></slot></div></div>`

define('timeline-item', { template })