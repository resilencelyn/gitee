export default {
    name: 'HiCol',
    props: {
        span: {
            type: Number,
            default: 1
        },
        tag: {
            type: String,
            default: 'div'
        }
    },
    computed: {
        padding() {
            let parent = this.$parent
            while (parent && parent.$options._componentTag !== 'hi-row') {
                parent = parent.$parent
            }
            return parent ? (typeof parent.padding == 'number'?parent.padding+'px':parent.padding) : '0';
        }
    },
    render(createElement, context) {
        let style = {};
        if (this.padding){
            style.paddingLeft = `${this.padding}`;
        }
        return createElement(
            this.tag,
            {
                class: ['hi-col',`hi-col-${this.span}`],
                style
            },
            this.$slots.default
        )
    }
}
