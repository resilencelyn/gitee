import { terser } from 'rollup-plugin-terser'
import css from '@henrikjoreteg/rollup-plugin-css'

export default {
  input: 'rollup.js',
  output: {
    file: 'dist/mtu.min.js',
    format: 'umd',
    plugins: [
      terser(),
    ]
  },
  plugins: [
    css({ output: 'dist/mtu.min.css', minify: true })
  ]
}