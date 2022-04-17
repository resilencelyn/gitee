/*
    ## Color

    http://llllll.li/randomColor/
        A color generator for JavaScript.
        randomColor generates attractive colors by default. More specifically, randomColor produces bright colors with a reasonably high saturation. This makes randomColor particularly useful for data visualizations and generative art.

    http://randomcolour.com/
        var bg_colour = Math.floor(Math.random() * 16777215).toString(16);
        bg_colour = "#" + ("000000" + bg_colour).slice(-6);
        document.bgColor = bg_colour;

    http://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/
        Creating random colors is actually more difficult than it seems. The randomness itself is easy, but aesthetically pleasing randomness is more difficult.
        https://github.com/devongovett/color-generator

    http://www.paulirish.com/2009/random-hex-color-code-snippets/
        Random Hex Color Code Generator in JavaScript

    http://chancejs.com/#color
        chance.color()
        // => '#79c157'
        chance.color({format: 'hex'})
        // => '#d67118'
        chance.color({format: 'shorthex'})
        // => '#60f'
        chance.color({format: 'rgb'})
        // => 'rgb(110,52,164)'

    http://tool.c7sky.com/webcolor
        网页设计常用色彩搭配表

    https://github.com/One-com/one-color
        An OO-based JavaScript color parser/computation toolkit with support for RGB, HSV, HSL, CMYK, and alpha channels.
        API 很赞

    https://github.com/harthur/color
        JavaScript color conversion and manipulation library

    https://github.com/leaverou/css-colors
        Share & convert CSS colors
    http://leaverou.github.io/css-colors/#slategray
        Type a CSS color keyword, #hex, hsl(), rgba(), whatever:

    色调 hue
        http://baike.baidu.com/view/23368.htm
        色调指的是一幅画中画面色彩的总体倾向，是大的色彩效果。
    饱和度 saturation
        http://baike.baidu.com/view/189644.htm
        饱和度是指色彩的鲜艳程度，也称色彩的纯度。饱和度取决于该色中含色成分和消色成分（灰色）的比例。含色成分越大，饱和度越大；消色成分越大，饱和度越小。
    亮度 brightness
        http://baike.baidu.com/view/34773.htm
        亮度是指发光体（反光体）表面发光（反光）强弱的物理量。
    照度 luminosity
        物体被照亮的程度,采用单位面积所接受的光通量来表示,表示单位为勒[克斯](Lux,lx) ,即 1m / m2 。

    http://stackoverflow.com/questions/1484506/random-color-generator-in-javascript
        var letters = '0123456789ABCDEF'.split('')
        var color = '#'
        for (var i = 0; i < 6; i++) {
            color += letters[Math.floor(Math.random() * 16)]
        }
        return color

        // 随机生成一个无脑的颜色，格式为 '#RRGGBB'。
        // _brainlessColor()
        var color = Math.floor(
            Math.random() *
            (16 * 16 * 16 * 16 * 16 * 16 - 1)
        ).toString(16)
        color = "#" + ("000000" + color).slice(-6)
        return color.toUpperCase()
*/
import Chroma from 'chroma-js'; // 使用 color 这个库进行颜色转换
import DICT from './color_dict';
import RandomColor from './color_dict_cn.json';
import { random, sample } from 'game-random';

let _hue: number = 0;
export function _goldenRatioColor(saturation: number = 0.5, value: number = 0.95) {
    const _goldenRatio = 0.618033988749895;
    _hue = _hue || Math.random();
    _hue += _goldenRatio;
    _hue %= 1;

    return Chroma.hsv(_hue * 360, saturation * 100, value * 100);
}

// 使用随机中国色代替随机 数据来自于 http://zhongguose.com
// 随机生成一个有吸引力的颜色，格式为 '#RRGGBB' hex。
export function color(name: string) {
    if (name && name in DICT) return (DICT as any)[name].nicer;
    return sample(RandomColor).hex;
}

// #DAC0DE
export function hex() {
    return _goldenRatioColor().hex();
}

// rgb(128,255,255)
export function rgb() {
    const hsv = _goldenRatioColor();
    return hsv.css();
}
// rgba(128,255,255,0.3)
export function rgba() {
    const hsv = _goldenRatioColor();
    return hsv.alpha(random(0, 1, true)).css();
}

// hsl(300,0.8,0.9)
export function hsl() {
    const hsv = _goldenRatioColor();
    return hsv.css();
}
