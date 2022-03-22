export const animationEnd = (v, call) => {
  const on = ['animationend', 'animationcancel']
  const called = () => {
    call()
    v.removeEventListener(on[0], called)
    v.removeEventListener(on[1], called)
  }
  v.addEventListener(on[0], called)
  v.addEventListener(on[1], called)
}

export const transitionEnd = (v, call) => {
  const on = ['transitionend', 'transitioncancel']
  const called = () => {
    call()
    v.removeEventListener(on[0], called)
    v.removeEventListener(on[1], called)
  }
  v.addEventListener(on[0], called)
  v.addEventListener(on[1], called)
}

export const clickAway = (v, call) => {
  let click
  const close = () => {
    document.removeEventListener('mousedown', click)
    document.removeEventListener('touchstart', click)
    window.removeEventListener('resize', click)
  }
  click = (e) => {
    if (e.target !== window && v.contains(e.target)) return
    call(e)
    close()
  }
  document.addEventListener('mousedown', click)
  document.addEventListener('touchstart', click)
  window.addEventListener('resize', click)
  return close
}