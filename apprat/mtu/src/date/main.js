import { define } from '../../core.js'
import { animationEnd } from '../../util.js'
import '../ripple/main.js'
import '../scrollbar/main.js'

class Dated extends Date {
  static fill(v) {
    return String(v).padStart(2, '0')
  }
  constructor(...args) {
    super(...args)
  }
  get Y() {
    return this.getFullYear()
  }
  get m() {
    return this.getMonth()
  }
  get d() {
    return this.getDate()
  }
  get timestamp() {
    return this.getTime()
  }
  format() {
    return `${Dated.fill(this.Y)}-${Dated.fill(this.m + 1)}-${Dated.fill(this.d)}`
  }
  setFullYear(...args) {
    super.setFullYear(...args)
    return this
  }
  setMonth(...args) {
    super.setMonth(...args)
    return this
  }
  setDate(...args) {
    super.setDate(...args)
    return this
  }
}

const template = `<style>:host{display:inline-block;vertical-align:middle;border-radius:2px;background:var(--color-background-card);user-select:none;width:100%;max-width:360px;box-shadow:0 2px 1px -1px rgb(0 0 0 / 20%),0 1px 1px 0 rgb(0 0 0 / 14%),0 1px 3px 0 rgb(0 0 0 / 12%)}.head{background:rgba(var(--color-primary));color:#fff;padding:16px 24px;border-radius:2px 2px 0 0;display:flex;flex-direction:column;line-height:1;min-width:196px;box-sizing:border-box}.head>.title{overflow:hidden;width:fit-content;--color-ripple:rgba(255,255,255,.4);color:rgba(255,255,255,0.7);font-size:1.125rem;cursor:pointer;display:inline-flex;align-items:center;padding:0 4px;border-radius:2px;height:32px}.head>.subtitle{color:#fff;font-size:1.5rem;margin-top:8px;letter-spacing:1px;overflow:hidden}.body{overflow:hidden}.switch{display:flex;align-items:center;height:56px}.switch>.but{display:flex;flex-shrink:0;border-radius:50%;height:40px;width:40px;cursor:pointer;align-items:center;justify-content:center;overflow:hidden;margin:0 8px}.switch>.but>svg{width:24px;height:24px;fill:var(--color-icon)}.switch>.text{flex-grow:1;overflow:hidden}.switch>.text>.but{display:flex;justify-content:center;align-items:center;cursor:pointer;height:36px;border-radius:2px;overflow:hidden;font-size:1rem;padding:0 16px}.day{display:flex;height:36px;align-items:center;padding:0 16px}.day>div{flex-grow:1;display:flex;justify-content:center;color:var(--color-text-secondary)}.days{display:flex;flex-wrap:wrap;padding:0 16px;margin:12px 0;counter-reset:day 0}.days>div{width:calc(100% / 7);display:flex;justify-content:center;align-items:center}.days .item{width:36px;overflow:hidden;height:36px;cursor:pointer;display:flex;justify-content:center;align-items:center;border-radius:50%;margin:2px 0}.days .item::after{content:counter(day);counter-increment:day}.days .checked{background:rgba(var(--color-accent));color:#fff;pointer-events:none}.days[date="28"]>div:nth-child(n+30),.days[date="29"]>div:nth-child(n+31),.days[date="30"]>div:nth-child(n+32){display:none}@keyframes left{0%{transform:translateX(-100%)}100%{transform:translateX(0%)}}@keyframes right{0%{transform:translateX(100%)}100%{transform:translateX(0%)}}@keyframes top{0%{transform:translateY(-100%)}100%{transform:translateY(0%)}}@keyframes bottom{0%{transform:translateY(100%)}100%{transform:translateY(0%)}}.left:not(.show-month) .switch>.text>.but,.left .days{animation:left .2s}.right:not(.show-month) .switch>.text>.but,.right .days{animation:right .2s}.head>.subtitle>.top{animation:top .2s;text-shadow:0 1.5rem #fff}.head>.subtitle>.bottom{animation:bottom .2s;text-shadow:0 -1.5rem #fff}.month{display:none;padding:0 0 8px 0;flex-wrap:wrap}.show-month .month{display:flex}.show-month .day,.show-month .days{display:none}.month>div{width:calc(100% / 3);padding:8px 16px;box-sizing:border-box}.month .item{overflow:hidden;height:36px;display:flex;align-items:center;justify-content:center;cursor:pointer;border-radius:2px}.month .item::after{content:'月'}.month .checked>.item{background:rgba(var(--color-accent));pointer-events:none;color:#fff}.year{max-height:420px;display:none;width:100%}.show-year .year{display:block}.show-year .body{display:none}.year>div{height:40px}.year>div>.item{display:flex;align-items:center;height:100%;justify-content:center;font-size:1rem;cursor:pointer}.year>div>.item::after{counter-increment:year;content:counter(year)}.year>.checked>.item{pointer-events:none;font-size:1.125rem;color:rgba(var(--color-accent))}</style><div class="root" part="root"><div class="head" part="head"><m-ripple class="title"></m-ripple><div class="subtitle"><div></div></div></div><div class="body" part="body"><div class="switch"><m-ripple class="but"><svg viewBox="0 0 1024 1024"><path d="M657.493333 707.413333L462.08 512l195.413333-195.84L597.333333 256l-256 256 256 256 60.16-60.586667z"></path></svg></m-ripple><div class="text"><m-ripple class="but"></m-ripple></div><m-ripple class="but"><svg viewBox="0 0 1024 1024"><path d="M366.506667 707.413333L561.92 512 366.506667 316.16 426.666667 256l256 256-256 256-60.16-60.586667z"></path></svg></m-ripple></div><div class="day"><div>日</div><div>一</div><div>二</div><div>三</div><div>四</div><div>五</div><div>六</div></div><div class="days"><div></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div></div><div class="month"><div><m-ripple class="item">一</m-ripple></div><div><m-ripple class="item">二</m-ripple></div><div><m-ripple class="item">三</m-ripple></div><div><m-ripple class="item">四</m-ripple></div><div><m-ripple class="item">五</m-ripple></div><div><m-ripple class="item">六</m-ripple></div><div><m-ripple class="item">七</m-ripple></div><div><m-ripple class="item">八</m-ripple></div><div><m-ripple class="item">九</m-ripple></div><div><m-ripple class="item">十</m-ripple></div><div><m-ripple class="item">十一</m-ripple></div><div><m-ripple class="item">十二</m-ripple></div></div></div><m-scrollbar class="year"><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div><div><m-ripple class="item"></m-ripple></div></m-scrollbar></div>`
const props = ['value', 'max', 'min']

const onItem = (el, call) => {
  el.addEventListener('click', e => {
    const item = e.composedPath()[0]
    if (!item.classList.contains('item')) return
    call(Array.prototype.slice.call(el.children).indexOf(item.parentNode))
  })
}

const setup = (shadow, node) => {
  const root = shadow.querySelector('.root')
  const title = shadow.querySelector('.head>.title')
  const subtitle = shadow.querySelector('.head>.subtitle>div')
  const text = shadow.querySelector('.switch>.text>.but')
  const seat = shadow.querySelector('.days>div')
  const days = shadow.querySelector('.days')
  const month = shadow.querySelector('.month')
  const year = shadow.querySelector('.year')

  let date = new Dated(node.getAttribute('value') || new Date())
  const scroll = () => {
    if (!root.classList.contains('show-year')) return
    const check = year.querySelector('.checked')
    check.scrollIntoViewIfNeeded ? check.scrollIntoViewIfNeeded() : scrollIntoView()
  }
  const render = v => {
    date = new Dated(v)
    const { Y, m, d } = date
    title.innerHTML = `${Y} 年`
    subtitle.innerText = `${Dated.fill(m + 1)}月${Dated.fill(d)}日 周${['日', '一', '二', '三', '四', '五', '六'][date.getDay()]}`
    text.innerText = `${Y}年 ${Dated.fill(m + 1)}月`
    const seats = new Date(Y, m, 1).getDay()
    seat.style.width = seats === 0 ? 0 : `calc((100% / 7) * ${seats})`
    days.querySelector('.checked')?.classList.remove('checked')
    days.children[d].children[0].classList.add('checked')
    days.setAttribute('date', new Date(Y, m + 1, 0).getDate())
    month.querySelector('.checked')?.classList.remove('checked')
    month.children[m].classList.add('checked')
    const index = date.Y - 20
    year.style.counterReset = 'year ' + (index - 1)
    year.querySelector('.checked')?.classList.remove('checked')
    const check = year.children[Y - index]
    check.classList.add('checked')
    scroll()
  }
  const change = v => {
    if (node.min !== '' && new Dated(node.min).timestamp > v.timestamp) return false
    if (node.max !== '' && new Dated(node.max).timestamp < v.timestamp) return false
    const val = v.format()
    if (val === date.format()) return false
    const is = v.timestamp > date.timestamp
    const dtY = is ? 'top' : 'bottom'
    animationEnd(subtitle, () => subtitle.classList.remove(dtY))
    subtitle.classList.add(dtY)
    node.value = val
    node.dispatchEvent(new Event('change'))
  }
  title.addEventListener('click', () => root.classList.toggle('show-year') && scroll())
  const toggle = v => {
    const dt = v ? 'right' : 'left'
    const is = change(new Dated(date).setMonth(date.m + (v ? 1 : -1)))
    if (is === false) return
    animationEnd(root, () => root.classList.remove(dt))
    root.classList.add(dt)
  }
  shadow.querySelector('.switch>.but').addEventListener('click', () => toggle(false))
  shadow.querySelector('.switch>.but:last-child').addEventListener('click', () => toggle(true))
  text.addEventListener('click', () => root.classList.toggle('show-month'))
  onItem(days, index => change(new Dated(date).setDate(index)))
  onItem(month, index => change(new Dated(date).setMonth(index)))
  onItem(year, index => {
    change(new Dated(date).setFullYear((date.Y - 20) + index))
    root.classList.remove('show-year')
  })
  render(date)
  return {
    value: {
      get: () => date.format(),
      set: v => render(v)
    },
    min: {
      get: '',
      set: v => node.min = v === '' ? '' : new Dated(v).format()
    },
    max: {
      get: '',
      set: v => node.max = v === '' ? '' : new Dated(v).format()
    }
  }
}

define('date', { template, props, setup })