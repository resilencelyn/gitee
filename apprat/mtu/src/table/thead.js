import { define } from '../../core.js'

const template = `<style>:host{display:table-header-group;user-select:none;color:var(--color-text-secondary);border-bottom:1px solid var(--color-border)}</style><slot></slot>`

define('thead', { template })