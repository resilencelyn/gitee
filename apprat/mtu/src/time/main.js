import { define } from '../../core.js'
import '../ripple/main.js'

const template = `<style>:host{display:inline-block;vertical-align:middle;border-radius:2px;background:var(--color-background-card);user-select:none;width:100%;max-width:320px;box-shadow:0 2px 1px -1px rgb(0 0 0 / 20%),0 1px 1px 0 rgb(0 0 0 / 14%),0 1px 3px 0 rgb(0 0 0 / 12%)}.head{background:rgba(var(--color-primary));color:rgba(255,255,255,.7);padding:16px 16px;border-radius:2px 2px 0 0;display:flex;justify-content:center;line-height:1;min-width:196px;box-sizing:border-box;--color-ripple:rgba(255,255,255,.4)}.head>.title{display:flex;align-items:center;font-size:3rem}.head>.title>m-ripple{margin:0 4px;cursor:pointer;border-radius:2px;padding:4px;overflow:hidden}:not(.show-minute) .head>.title>.hour,.show-minute .head>.title>.minute,:not(.show-am) .head .pm,.show-am .head .am{color:#fff;pointer-events:none}.head>.pa{display:flex;flex-direction:column;justify-content:center}.head>.pa>m-ripple{overflow:hidden;padding:5px 6px;cursor:pointer;border-radius:2px}.body{position:relative;background:var(--color-background-card);padding:24px}.body>.bg{border-radius:50%;padding-top:100%;background:var(--color-background)}.body>.hour,.body>.minute{position:absolute;width:calc(100% - 48px);height:calc(100% - 48px);margin:24px;top:0;left:0;box-sizing:border-box;display:flex;justify-content:center;align-items:center;cursor:pointer}.show-minute .body>.hour{display:none}.show-minute .body>.minute{display:flex}.body .block{width:100%;display:flex;justify-content:space-between;position:absolute}.body .block.li1{transform:rotate(30deg)}.body .block.li1>.item{transform:rotate(330deg)}.body .block.li2{transform:rotate(60deg)}.body .block.li2>.item{transform:rotate(300deg)}.body .block.li3{transform:rotate(90deg)}.body .block.li3>.item{transform:rotate(270deg)}.body .block.li4{transform:rotate(120deg)}.body .block.li4>.item{transform:rotate(240deg)}.body .block.li5{transform:rotate(150deg)}.body .block.li5>.item{transform:rotate(210deg)}.body .item{font-size:1rem;display:flex;width:36px;height:36px;justify-content:center;align-items:center;border-radius:50%}.body .checked{color:#fff;background:rgba(var(--color-accent))}.body .line{width:100%;display:flex;justify-content:flex-end}.body .line>div{width:50%;display:flex;align-items:center}.body .line>div::after{content:'';width:8px;height:8px;background:rgba(var(--color-accent));border-radius:50%;margin-right:12px}.body .line>div::before{content:'';height:2px;background:rgba(var(--color-accent));flex-grow:1}.body>.minute{display:none}</style><div class="root" part="root"><div class="head" part="head"><div class="title"><m-ripple class="hour"></m-ripple>:<m-ripple class="minute"></m-ripple></div><div class="pa"><m-ripple class="pm">PM</m-ripple><m-ripple class="am">AM</m-ripple></div></div><div class="body" part="body"><div class="bg"></div><div class="hour"><div class="line"><div></div></div><div class="block"><div class="item num9">9</div><div class="item num3">3</div></div><div class="block li1"><div class="item num10">10</div><div class="item num4">4</div></div><div class="block li2"><div class="item num11">11</div><div class="item num5">5</div></div><div class="block li3"><div class="item num0 num12">12</div><div class="item num6">6</div></div><div class="block li4"><div class="item num1">1</div><div class="item num7">7</div></div><div class="block li5"><div class="item num2">2</div><div class="item num8">8</div></div></div><div class="minute"><div class="line"><div></div></div><div class="block"><div class="item num45">45</div><div class="item num15">15</div></div><div class="block li1"><div class="item num50">50</div><div class="item num20">20</div></div><div class="block li2"><div class="item num55">55</div><div class="item num25">25</div></div><div class="block li3"><div class="item num0">0</div><div class="item num30">30</div></div><div class="block li4"><div class="item num5">5</div><div class="item num35">35</div></div><div class="block li5"><div class="item num10">10</div><div class="item num40">40</div></div></div></div></div>`
const props = ['value']

class Time {
  static fill = v => String(v).padStart(2, '0')
  get Hour() {
    return this.hour
  }
  get Minute() {
    return this.minute
  }
  set Hour(v) {
    if (v >= 24) v = 0
    this.hour = v
  }
  set Minute(v) {
    if (v >= 60) v = 0
    this.minute = v
  }
  constructor(arg) {
    if (!arg) {
      const date = new Date()
      arg = `${date.getHours()}:${date.getMinutes()}`
    }
    const [h, i] = arg.split(':')
    this.hour = Number(h)
    this.minute = Number(i)
  }
  format() {
    return `${Time.fill(this.Hour)}:${Time.fill(this.Minute)}`
  }
}

const setup = (shadow, node) => {
  const root = shadow.querySelector('.root')
  const titleHour = shadow.querySelector('.head>.title>.hour')
  const titleMinute = shadow.querySelector('.head>.title>.minute')
  const hour = shadow.querySelector('.body>.hour')
  const hourLine = shadow.querySelector('.body>.hour>.line')
  const minute = shadow.querySelector('.body>.minute')
  const minuteLine = shadow.querySelector('.body>.minute>.line')

  let time = new Time()

  titleHour.addEventListener('click', () => root.classList.remove('show-minute'))
  titleMinute.addEventListener('click', () => root.classList.add('show-minute'))

  const isAm = () => root.classList.contains('show-am')

  const render = () => {
    root.classList[time.Hour >= 12 ? 'add' : 'remove']('show-am')
    titleHour.innerHTML = Time.fill(time.Hour)
    titleMinute.innerHTML = Time.fill(time.Minute)
    const H = time.Hour >= 12 ? time.Hour - 12 : time.Hour
    hour.querySelector('.checked')?.classList.remove('checked')
    hour.querySelector('.num' + H).classList.add('checked')
    hourLine.style.transform = `rotate(${(H - 3) * 30}deg)`
    minute.querySelector('.checked')?.classList.remove('checked')
    minute.querySelector('.num' + time.Minute)?.classList.add('checked')
    minuteLine.style.transform = `rotate(${(time.Minute - 15) * 6}deg)`
  }

  const change = () => {
    node.dispatchEvent(new Event('change'))
    render()
  }

  shadow.querySelector('.head>.pa>.pm').addEventListener('click', () => {
    time.Hour = time.Hour - 12
    change()
  })
  shadow.querySelector('.head>.pa>.am').addEventListener('click', () => {
    time.Hour = time.Hour + 12
    change()
  })

  const getAtan = (e, tag) => {
    const { clientX, clientY } = e
    const { width, height, left, top } = tag.getBoundingClientRect()
    const [x, y] = [clientX - left, clientY - top]
    const r = Math.atan2(y - (height / 2), x - (width / 2)) / Math.PI * 180
    return r < 0 ? r + 360 : r
  }

  hour.addEventListener('click', e => {
    let r = getAtan(e, hour)
    let h = Math.floor(r / 30)
    if ((r % 30) >= 15) h += 1
    h = h * 30
    if (h === 360) h = 0
    h = ((h / 3) + 30) / 10
    if (h > 12) h = h - 12
    if (isAm()) h = h + 12
    if (h === time.Hour) return
    time.Hour = h
    change()
  })

  minute.addEventListener('click', e => {
    let r = ~~(((~~getAtan(e, minute)) / 6) + 15)
    if (r > 60) r = r - 60
    if (r == 60) r = 0
    time.Minute = r
    change()
  })

  render()

  return {
    value: {
      get: () => time.format(),
      set: v => {
        time = new Time(v)
        render()
      }
    }
  }
}

define('time', { template, props, setup })