# Mtu UI组件库

```
$ npm install mtu
```

> 响应式跨框架的UI组件库，基于 [Web Components](https://developer.mozilla.org/en-US/docs/Web/Web_Components) 封装，兼容 `React` 、 `Vue` 等前端框架，亦或者在 `electron` 中引入，设计规范参考了 Google  [Material Design](https://material-io.cn/)，并针对移动端做了优化，适用于需要同时兼容移动端的Web项目，门户网站、后台管理系统等。

## 预览

![review](https://cdn.jsdelivr.net/npm/mtu/review.png)

## 开发文档

[https://mtu.desgin](https://apprat.gitee.io/mtu-docs)

## 优势

- [x] 响应式；自适应不同分辨率设备，并在窗口或组件发生变化时更新样式(横屏竖屏)。
- [x] 跨框架；兼容所有框架，无论是原生开发还是任何前端框架均可引入使用。
- [x] 零依赖；不依赖任何第三方库。
- [x] 按需引入；借助 `webpack` 或 `vite`，你可以按需引入组件，减少体积。
- [x] 本地化；完全中文的开发文档支持。
- [x] 无门槛；基于 `Web Components` 封装，如同常规的 `div`、`select` 的布局使用。
- [x] SEO；支持SEO，后端渲染的MVC应用。

## 引入 Mtu

完全引入：
```js
import 'mtu'
import 'mtu/style'
```

## CDN

```html
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/mtu/dist/mtu.min.css">
<script src="https://cdn.jsdelivr.net/npm/mtu/dist/mtu.min.js"></script>
```

## 兼容性

| 浏览器           | 版本 |
| ------------------- | ---- |
| Chrome              | 50+  |
| Edge                | 79+  |
| Firefox             | 63+  |
| Internet Explorer   | None |
| Opera               | 37+  |
| Safari              | 10+  |
| Android webview     | 50+  |
| Chrome for Android  | 50+  |
| Firefox for Android | 84+  |
| Opera for Android   | 10+  |
| Safari on iOS       | 10+  |
| Samsung Internet    | 5+   |

---