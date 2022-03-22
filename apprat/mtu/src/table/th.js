import { define } from '../../core.js'

const template = `<style>:host{display:table-cell;vertical-align:middle;padding:19px 24px;text-align:right}:host(:first-of-type){text-align:left}</style><slot></slot>`

define('th', { template })