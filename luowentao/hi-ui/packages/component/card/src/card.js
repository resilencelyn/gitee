export default {
    name:'HiCard',
    props:{
        padding:{
            type:[Number,String],
            default:0
        },
        height:{
            type:[String,Number],
            default: '4rem'
        },
    },
    render(createElement, context) {
        let style = {
            marginTop:'0.5rem',
            height: typeof this.height == "number"?this.height+'px':this.height,
            padding: typeof this.padding == "number"?this.padding+'px':this.padding,
        };
        return createElement('div',{
            class:['card'],
            style
        },this.$slots.default)
    }
}
