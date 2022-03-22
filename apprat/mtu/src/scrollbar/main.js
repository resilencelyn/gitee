import { define } from '../../core.js'

const template = `<style>:host{display:block;overflow:auto;overflow:overlay;scrollbar-width:thin}@media(pointer:fine){:host::-webkit-scrollbar{width:6px;height:6px}:host::-webkit-scrollbar-thumb{background:transparent;border-radius:4px}:host(:hover)::-webkit-scrollbar-thumb{background:#dbdbdb}}</style><slot></slot>`

define('scrollbar', { template })