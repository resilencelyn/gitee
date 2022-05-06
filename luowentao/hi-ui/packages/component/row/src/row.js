export default {
    name:'HiRow',
    props:{
        padding:{
            type:[Number,String],
            default:0
        },
        tag:{
            type:String,
            default: 'div'
        },
        isVerticalLine:{
            type:Boolean,
            default:false
        }
    },
    render(createElement, context) {
        return createElement(
            this.tag,
            {
                class:['hi-row',this.isVerticalLine?'hi-row-line':'']
            },
            this.$slots.default
        )
    }
}
