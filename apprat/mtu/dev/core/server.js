import http from 'http'
import path from 'path'
import fs from 'fs'
import mimeTypes from 'mime-types'
import htmlParser from 'node-html-parser'
import { parseHTML } from './util.js'

const resolve = (...args) => path.resolve(path.dirname(process.argv[1]), '../../', ...args)
const document = fs.readFileSync(resolve('./dev/core/index.html')).toString()

const parse = (v) => {
  const html = htmlParser.parse(document)
  const dom = htmlParser.parse(v)
  const scripts = dom.querySelectorAll('script')
  const script = scripts[scripts.length - 1]
  script.innerHTML = parseHTML(v)
  dom.removeChild(dom.querySelector('template'))
  const body = html.querySelector('body')
  body.appendChild(dom)
  return html.innerHTML
}

class App {
  constructor() {
    this.call = []
  }
  use(f) {
    this.call.push(f)
  }
}

const app = new App()

//Index
app.use(async ({ url }, res) => {
  if (url !== '/') return
  res.setHeader('Content-Type', 'text/html')
  const html = htmlParser.parse(document)
  const body = html.querySelector('body')
  let str = ''
  for (const item of fs.readdirSync(resolve('./dev/src'))) {
    for (const key of fs.readdirSync(resolve('./dev/src', item))) {
      if (!key.endsWith('.html')) continue
      str += `<li><a href="./dev/${item}/${key}">➜ ${item}/${key}</a></li>`
    }
  }
  body.insertAdjacentHTML('beforeend', `<style>
  ul{
    margin:0;
    padding:0
  }
  li{
    list-style: none;
  }

  a{
    color: var(--color-text-secondary);
    text-decoration: none;
    display: inline-block;
    padding: 6px 12px;
  }

  a:hover{
    background:var(--color-background-bar);
    color: rgba(var(--color-accent));
  }
  </style>`)
  body.insertAdjacentHTML('beforeend', `<ul>${str}</ul>`)
  return html.innerHTML
})

//Pages
app.use(async ({ url }, res) => {
  if (!url.startsWith('/dev/') || !url.endsWith('.html')) return
  res.setHeader('Content-Type', 'text/html')
  const file = resolve('./dev/src/', url.slice(5))
  if (!fs.existsSync(file)) {
    res.statusCode = 404
    return 'Not Found'
  }
  return parse(fs.readFileSync(file).toString())
})

//Static CSS
const css = {}
const Css = (url, text) => {
  css[url] = text
  return `
    const link = document.createElement('link')
    link.rel = 'stylesheet'
    link.href = '${url}'
    document.head.append(link)
  `
}
app.use(async (req, res) => {
  const url = req.url
  if (!css[url] || req.headers.accept === '*/*') return
  res.setHeader('Content-Type', 'text/css')
  return css[url]
})

//Find File
app.use(async ({ url }, res) => {
  if (!url.startsWith('/dev/')) return
  const name = url.slice(5)
  const devFile = resolve('./dev/src/', name)
  const srcFile = resolve('./src/', name)
  const setJSHeader = () => res.setHeader('Content-Type', 'application/javascript')
  if (url.endsWith('.js')) {
    setJSHeader()
    const df = devFile.slice(0, -3) + '.html'
    if (fs.existsSync(df)) return parseHTML(fs.readFileSync(df).toString())
    if (fs.existsSync(srcFile)) return fs.readFileSync(srcFile).toString()
  }
  if (url.endsWith('.css')) {
    setJSHeader()
    if (fs.existsSync(devFile)) return Css(url, fs.readFileSync(devFile).toString())
    if (fs.existsSync(srcFile)) return Css(url, fs.readFileSync(srcFile).toString())
  }
  if (fs.existsSync(devFile)) {
    res.setHeader('Content-Type', mimeTypes.contentType(devFile))
    return fs.readFileSync(devFile)
  }
  if (fs.existsSync(srcFile)) {
    res.setHeader('Content-Type', mimeTypes.contentType(srcFile))
    return fs.readFileSync(srcFile)
  }
})

//Other
app.use(async ({ url }, res) => {
  const file = resolve('./', url.slice(1))
  if (!fs.existsSync(file)) return
  const buffer = fs.readFileSync(file)
  res.setHeader('Content-Type', mimeTypes.contentType(file))
  return buffer
})


const server = http.createServer(async (req, res) => {
  for (const item of app.call) {
    const out = await item(req, res)
    if (out !== undefined) {
      res.write(out)
      res.end()
      return
    }
  }
  res.setHeader('Content-Type', 'text/html')
  res.statusCode = 404
  res.write('Not Found')
  res.end()
})
const port = 9990
server.listen(port)
console.log('\x1B[36mhttp://127.0.0.1:%s\x1B[0m', port)