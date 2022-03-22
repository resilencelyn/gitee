import path from 'path'
import fs from 'fs'

import { parseHTML } from './util.js'

const resolve = (...args) => path.resolve(path.dirname(process.argv[1]), '../../', ...args)

for (const item of fs.readdirSync(resolve('./dev/src'))) {
  if (item === 'demo') continue
  const toDir = resolve('./src/', item)
  !fs.existsSync(toDir) && fs.mkdirSync(toDir)
  for (const name of fs.readdirSync(resolve('./dev/src', item))) {
    if (name.endsWith('.html')) {
      fs.writeFileSync(resolve(toDir, `${name.slice(0, -5)}.js`), parseHTML(fs.readFileSync(resolve('./dev/src/', item, name)).toString()))
      console.log('Running ', `${item}/${name}`, 'OK')
      continue
    }
    if (!name.endsWith('.css')) fs.copyFileSync(resolve('./dev/src', item, name), resolve('./dist', name))
    fs.copyFileSync(resolve('./dev/src', item, name), `${toDir}/${name}`)
  }
}