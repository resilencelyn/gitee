<template>


</template>

<script>
    export default {
        name:"rightBottom",
        props:{
            iccid:String,
            openid:String,
            appId:String
        },
        data() {
            return {
                activeKey:sessionStorage.getItem("activeKey")===""?"":sessionStorage.getItem("activeKey"),
                show:false,
                flags: false,
                position: {
                    x: 0,
                    y: 0
                },
                nx: '',
                ny: '',
                dx: '',
                dy: '',
                xPum: '',
                yPum: '',
                isShow: false,
                moveBtn: {}
            }
        },
        mounted() {
            this.moveBtn = this.$refs.moveBtn
        },
        methods: {
            showPop(){
                let _this=this
                _this.show=true
            },
            // 实现移动端拖拽
            down() {
                this.flags = true;
                var touch;
                if (event.touches) {
                    touch = event.touches[0];
                } else {
                    touch = event;
                }
                this.position.x = touch.clientX;
                this.position.y = touch.clientY;
                this.dx = this.moveBtn.offsetLeft;
                this.dy = this.moveBtn.offsetTop;
            },
            move() {
                if (this.flags) {
                    var touch;
                    if (event.touches) {
                        touch = event.touches[0];
                    } else {
                        touch = event;
                    }
                    this.nx = touch.clientX - this.position.x;
                    this.ny = touch.clientY - this.position.y;
                    this.xPum = this.dx + this.nx;
                    this.yPum = this.dy + this.ny;
                    var clientWidth = document.documentElement.clientWidth;
                    var clientHeight = document.documentElement.clientHeight;
                    if (this.xPum > 0 && this.xPum < (clientWidth - this.moveBtn.offsetWidth)) {
                        this.moveBtn.style.left = this.xPum + "px";
                    }
                    if (this.yPum > 0 && this.yPum < (clientHeight - this.moveBtn.offsetHeight)) {
                        this.moveBtn.style.top = this.yPum + "px";
                    }

                    //阻止页面的滑动默认事件
                    document.addEventListener("touchmove", this.handler, {
                        passive: false
                    });
                }
            },
            //鼠标释放时候的函数
            end() {
                this.flags = false;
                document.addEventListener('touchmove', this.handler, {
                    passive: false
                });
            },
            handler(e) {
                if(this.flags){
                    event.preventDefault();
                }else{
                    return true
                }
            }
        }
    }
</script>

<style scoped>
    .back-home {
        height: 3rem;
        width: 3rem;
        position: absolute;
        right: 0;
        bottom:22%;
        z-index: 99999;
    }

    .back-home .el_button {
        height: 100%;
        width: 100%;
        box-shadow:#4768f3 0px 0px 10px;
    }
</style>

