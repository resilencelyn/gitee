import { define } from '../../core.js'
import '../scrollbar/main.js'

const template = `<style>:host{display:block;box-shadow:0 2px 1px -1px rgba(0,0,0,.2),0 1px 1px 0 rgba(0,0,0,.14),0 1px 3px 0 rgba(0,0,0,.12);word-break:break-word;background:var(--color-background-card);border-radius:2px}.table{display:table;min-width:100%;border-collapse:collapse;border-spacing:0;white-space:nowrap}::slotted([slot=header]){height:56px}::slotted([slot=footer]){border-top:1px solid var(--color-border);height:48px}</style><slot name="header"></slot><m-scrollbar class="root" part="root"><div class="table" part="table"><slot></slot></div></m-scrollbar><slot name="footer"></slot>`

define('table', { template })