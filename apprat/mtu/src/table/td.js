import { define } from '../../core.js'

const template = `<style>:host{display:table-cell;vertical-align:middle;max-width:256px;padding:16px 24px;text-align:right}:host(:first-of-type){text-align:left}</style><slot></slot>`

define('td', { template })