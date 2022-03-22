export const global = window.Mtu || (window.Mtu = {})

const useAttr = (node, attrs) => {
  for (const attr in attrs) {
    if (['onConnected', 'onDisconnected', 'onAdopted'].includes(attr)) continue
    const item = attrs[attr]
    const info = {
      type: typeof item,
      value: item
    }
    const proxy = {
      get: () => info.value,
      set: v => info.value = v
    }
    const toType = v => {
      if (info.type === 'string') return String(v)
      if (info.type === 'boolean') return v === 'false' ? false : Boolean(v)
      if (info.type === 'number') return Number(v)
      return v
    }
    Object.defineProperty(node, attr, {
      get: () => proxy.get(),
      set: v => {
        if (node.getAttribute(attr) !== String(v)) return node.setAttribute(attr, v)
        v = toType(v)
        if (v === proxy.get()) return
        proxy.set(v)
      },
      enumerable: true
    })
    if (Array.isArray(item)) {
      const v = item[0]
      info.type = typeof v
      info.value = v
      proxy.vlaue = v
      proxy.set = val => {
        if (!item.includes(val)) val = v
        info.value = val
      }
      continue
    }
    //object
    if (typeof item !== 'object') continue
    info.type = typeof item.get
    info.value = item.get
    if (typeof item.get === 'function') {
      const v = item.get()
      info.type = typeof v
      info.value = v
      proxy.get = item.get
    }
    if (item.set) {
      proxy.set = v => {
        info.value = v
        item.set(v)
      }
    }
    if (Array.isArray(item.get)) {
      const v = item.get[0]
      info.type = typeof v
      info.value = v
      proxy.vlaue = v
      proxy.set = val => {
        if (!item.includes(val)) val = v
        info.value = val
      }
    }
  }
}

export const define = (name, {
  template = '',
  props = [],
  setup = new Function(),
  options = {},
  shadowRootInit: { mode = 'closed', delegatesFocus = false } = {}
}) => {
  const custom = window.customElements
  const n = 'm-' + name
  if (custom.get(n)) return
  const map = new Map()
  custom.define(n, class extends HTMLElement {
    static observedAttributes = props
    constructor() {
      super()
      const shadow = this.attachShadow({ mode, delegatesFocus })
      shadow.innerHTML = template
      const attrs = setup(shadow, this) || {}
      map.set(this, attrs)
      useAttr(this, attrs)
    }
    attributeChangedCallback(name, old, value) {
      this[name] = value
    }
    connectedCallback() {
      const call = map.get(this).onConnected
      call && call()
    }
    disconnectedCallback() {
      const call = map.get(this).onDisconnected
      call && call()
    }
    adoptedCallback() {
      const call = map.get(this).onAdopted
      call && call()
    }
  }, options)
}