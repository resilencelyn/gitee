import HtmlMinifier from 'html-minifier'
import Beautify from 'js-beautify'
import HtmlParser from 'node-html-parser'

const minifyHTML = (v) => HtmlMinifier.minify(v, { collapseWhitespace: true, minifyCSS: true })
const beautify = (v) => Beautify.js_beautify(v, { brace_style: 'preserve-inline', indent_size: 2 })

export const parseHTML = (v) => {
  try {
    const html = HtmlParser.parse(v)
    const template = minifyHTML(html.querySelector('template').innerHTML)
    const scripts = html.querySelectorAll('script')
    const script = scripts[scripts.length - 1]
    return beautify(script.innerHTML.replace('$template', '`' + template + '`'))
  } catch (error) {
    console.log(error)
    return ''
  }
}