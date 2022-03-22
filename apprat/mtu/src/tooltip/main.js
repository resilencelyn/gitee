import { define } from '../../core.js'

const template = `<style>:host{display:inline-block;vertical-align:middle}.root{z-index:3;display:block;user-select:none;position:fixed;left:0;top:0;background:rgba(97,97,97,.9);color:#fff;border-radius:2px;padding:6px 8px;font-size:.75rem;line-height:1.6;width:fit-content;max-width:112px;word-wrap:break-word;word-break:break-all;pointer-events:none;transform:scale(.9);filter:opacity(0);transition:transform .2s,filter .2s}@media(pointer:fine){:host(:hover) .root{transform:scale(1)!important;filter:opacity(1)!important}}@media(pointer:coarse){:host(:active) .root{transform:scale(1)!important;filter:opacity(1)!important}}</style><slot></slot><slot class="root" part="root" name="text"></slot>`

const setup = (shadow, node) => {
  const root = shadow.querySelector('.root')
  const position = () => {
    const rect = node.getBoundingClientRect()
    const w = window.innerWidth,
      h = window.innerHeight,
      m = 8
    let top = rect.top + node.offsetHeight + m,
      left = rect.left - ((root.offsetWidth - node.offsetWidth) / 2)
    if (h - (rect.top + node.offsetHeight + m + root.offsetHeight) < 0) top = (rect.top - (m + root.offsetHeight))
    if (left + root.offsetWidth > w) left = (rect.left + node.offsetWidth) - root.offsetWidth
    if (left < 0) left = rect.left
    root.style.top = top + 'px'
    root.style.left = left + 'px'
  }
  node.addEventListener('mouseover', position)
  node.addEventListener('touchstart', position)
}

define('tooltip', { template, setup })