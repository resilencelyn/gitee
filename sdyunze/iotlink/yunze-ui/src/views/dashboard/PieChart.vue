<template>
  <div :class="className" :style="{height:height,width:width}" />
</template>

<script>
  import tools from "@/utils/yunze/tools";
  import echarts from 'echarts'
  require('echarts/theme/macarons') // echarts theme
  import resize from './mixins/resize'

export default {
  mixins: [resize],
  props: {
    className: {
      type: String,
      default: 'chart'
    },
    width: {
      type: String,
      default: '100%'
    },
    height: {
      type: String,
      default: '300px'
    },
    P_legend:{type: Object,default: null },//类别
    P_series:{type: Object,default: null },//
    P_color:{type: Array,default: [] },//
  },
  data() {
    return {
      chart: null
    }
  },
  mounted() {
    this.$nextTick(() => {
      this.initChart()
    })
  },
  beforeDestroy() {
    if (!this.chart) {
      return
    }
    this.chart.dispose()
    this.chart = null
  },
  methods: {
    initChart() {
      this.chart = echarts.init(this.$el, 'macarons')
      //初始化属性
      let P_legend = this.P_legend;
      let P_series = this.P_series;
      let P_color = this.P_color;
      let color = [];
      let left = 'center';
      let bottom = '10';
      let legend_data = [];
      let name = 'WEEKLY WRITE ARTICLES';
      let type = 'pie';
      let roseType = 'radius';
      let radius = [15, 95];
      let center = ['50%', '38%'];
      let animationEasing = 'cubicInOut';
      let animationDuration = 600;
      let data = [
        { value: 320, name: 'Industries' },
        { value: 240, name: 'Technology' },
        { value: 149, name: 'Forex' },
        { value: 100, name: 'Gold' },
        { value: 59, name: 'Forecasts' }
      ];


      if(P_legend!=null){
         left = tools.Is_null(P_legend.left)?P_legend.left:left;
          bottom = tools.Is_null(P_legend.bottom)?P_legend.left:bottom;

      }
      color = P_color!=null && P_color.length>0?P_color:color;

      if(P_series!=null){
        name = tools.Is_null(P_series.name)?P_series.name:name;
        type = tools.Is_null(P_series.type)?P_series.type:type;
        roseType = tools.Is_null(P_series.roseType)?P_series.roseType:roseType;
        radius = P_series.radius!=null && P_series.radius.length>0?P_series.radius:radius;
        center = P_series.center!=null && P_series.center.length>0?P_series.center:center;
        data = P_series.data!=null && P_series.data.length>0?P_series.data:data;
        animationEasing = tools.Is_null(P_series.animationEasing)?P_series.animationEasing:animationEasing;
        animationDuration = tools.Is_null(P_series.animationDuration)?P_series.animationDuration:animationDuration;
      }

      if(data!=null){//放入类别
        for (let i = 0; i < data.length; i++) {
          legend_data.push(data[i].name);
        }
      }
      //console.log(legend_data)


      this.chart.setOption({
        color: color,
        tooltip: {
          trigger: 'item',
          formatter: '{a} <br/>{b} : {c} ({d}%)'
        },
        legend: {
          left: left,//是否 局中类型
          bottom: bottom,//距离底部高度
          data: legend_data //类别
        },
        series: [//鼠标事件 触发
          {
            name: name,
            type: type,
            roseType: roseType,
            radius: radius,
            center: center,
            data: data,
            animationEasing: animationEasing,
            animationDuration: animationDuration,
            emphasis: {
              label: {
                show: true,
                fontSize: '20',
                fontWeight: 'bold'
              }
            },
          }
        ]
      })
    }
  }
}
</script>
