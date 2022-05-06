<template>
  <div style="width: 100%;">
    <canvas :width="width" :height="height" class="annular"></canvas>
  </div>
</template>

<script>
export default {
  name: "HiAnnular",
  props:{
    width:{
      type:[Number],
      default:300
    },
    height:{
      type:[Number],
      default:300
    }
  },
  data(){
    return{
      x0:0,//圆心x轴
      y0:0,//圆心y轴
      r1:0,//外园半径
      r2:0,//内圆半径
      startAng:135,//起始角度
      endAng:45,//结束角度
    }
  },
  mounted() {
    this.x0 = (this.width/2)/2*2;
    this.y0 = (this.height/2)/2*2;
    this.r1 = ((this.width/2)/2-16)*2;
    this.r2 = ((this.width/2)/2-16)*2;

    const canvas = this.$el.getElementsByClassName('annular').item(0);
    let ctx = canvas.getContext("2d");

    ctx.beginPath();
    // ---------1 画大圆------------
    ctx.arc(this.x0, this.y0,this.r1, (Math.PI / 180) * this.startAng, (Math.PI / 180) * this.endAng, false);
    ctx.lineCap = "round";
    ctx.lineWidth = '0';
    ctx.strokeStyle = '#ebeff0';
    ctx.stroke();

    // 获取大圆上的点 与 边界圆交点
    let x1 = this.getPointX(this.r1, this.startAng)
    let y1 = this.getPointY(this.r1, this.startAng)

    ctx.moveTo(x1, y1)
    let temp = this.startAng
    window.requestAnimationFrame(() => {
      return this.animDraw(ctx, temp)
    });




  },
  methods:{
    getPointX(r, ao){
      return this.x0 + r * Math.cos(ao * Math.PI / 180);
    },
    getPointY(r, ao) {
      return this.y0 + r * Math.sin(ao * Math.PI / 180)
    },
    animDraw(ctx, temp) {
      temp++
      if (temp < 270) {
        window.requestAnimationFrame(() => {
          return this.animDraw(ctx, temp)
        });
      }
      this.drawColorArc(ctx, temp)
    },
    drawColorArc(ctx, endAng) {
      ctx.beginPath();
      ctx.arc(this.x0, this.y0, this.r1, (Math.PI / 180) * this.startAng, (Math.PI / 180) * this.endAng, false);
      ctx.lineCap = "round";
      ctx.lineWidth = '18';
      ctx.strokeStyle = '#1dbdc4';
      ctx.stroke();
    }
  }
}
</script>

<style scoped>

</style>
