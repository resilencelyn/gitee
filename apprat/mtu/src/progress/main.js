import { define } from '../../core.js'

const template = `<style>:host{display:inline-block;vertical-align:middle;position:relative;height:40px;width:40px}:host([mode=linear]){width:auto;height:4px;display:block}:host([indeterminate=true]) .none,:host([mode=linear]) .normal{display:none}:host([indeterminate=true]) .un,:host([mode=linear]) .linear{display:block}.normal,.linear{height:100%;width:100%}.normal{transform:rotate(-90deg)}.normal>.none>svg{stroke-dashoffset:565.4866776461628}.normal>.none>svg>circle{transition:stroke-dashoffset .2s}.normal circle{fill:none;stroke:rgba(var(--color-accent));stroke-width:20px;transform-origin:center;stroke-dasharray:565.4866776461628}@keyframes un-load{0%{stroke-dashoffset:554.1769440932395}12.5%{stroke-dashoffset:138.54423602330988;transform:rotate(0deg)}25%{stroke-dashoffset:554.1769440932395;transform:rotate(270deg)}37.5%{stroke-dashoffset:138.54423602330988;transform:rotate(270deg)}50%{stroke-dashoffset:554.1769440932395;transform:rotate(540deg)}62.5%{stroke-dashoffset:138.54423602330988;transform:rotate(540deg)}75%{stroke-dashoffset:554.1769440932395;transform:rotate(810deg)}87.5%{stroke-dashoffset:138.54423602330988;transform:rotate(810deg)}100%{stroke-dashoffset:554.1769440932395;transform:rotate(1080deg)}}.normal>.un circle{stroke-dasharray:565.4866776461628;animation:un-load 4s linear infinite}.un,.linear{display:none}.linear>.none,.linear>.un{background:rgba(var(--color-accent),.24);height:4px;overflow:hidden;position:relative}.linear>.none>span{display:block;height:100%;background:rgba(var(--color-accent));transform:scaleX(0);transform-origin:left;transition:transform .2s}.linear>.un::before,.linear>.un::after{top:0;left:0;content:'';position:absolute;height:100%;width:100%;background:rgba(var(--color-accent));transform-origin:left top}@keyframes normal-before{0%{transform:translateX(0) scaleX(0)}50%{transform:translateX(30%) scaleX(.7)}100%{transform:translateX(100%) scaleX(0)}}@keyframes normal-after{0%{transform:translateX(0) scaleX(0)}50%{transform:translateX(0) scaleX(0)}75%{transform:translateX(0) scaleX(.25)}100%{transform:translateX(100%) scaleX(0)}}.linear>.un::before{animation:normal-before 2s linear infinite}.linear>.un::after{animation:normal-after 2s linear infinite}</style><div class="normal"><div class="none"><svg viewBox="0 0 200 200"><circle cx="100" cy="100" r="90"></circle></svg></div><div class="un"><svg viewBox="0 0 200 200"><animatetransform attributeType="XML" attributeName="transform" dur="1335ms" type="rotate" from="-90" to="270" repeatCount="indefinite" calcMode="linear" /><circle cx="100" cy="100" r="90"></circle></svg></div></div><div class="linear"><div class="none"><span></span></div><div class="un"></div></div>`
const props = ['value', 'max', 'mode', 'indeterminate']

const setup = (shadow, node) => {
  const normal = shadow.querySelector('.normal>.none>svg')
  const linear = shadow.querySelector('.linear>.none>span')
  const update = (max, value) => {
    normal.style.strokeDashoffset = 565.4866776461628 * (1 - value / max)
    linear.style.transform = `scaleX(${value / max})`
  }
  return {
    value: {
      get: 0,
      set: v => update(node.max, v),
    },
    max: {
      get: 100,
      set: v => update(v, node.value)
    },
    mode: ['normal', 'linear'],
    indeterminate: false
  }
}

define('progress', { template, props, setup })