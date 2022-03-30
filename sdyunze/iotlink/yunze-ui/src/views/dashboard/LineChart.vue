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
      default: '350px'
    },
    autoResize: {
      type: Boolean,
      default: true
    },
    chartData: {
      type: Object,
      required: true
    },
    P_xAxis:{type: Array,default: ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun'] },//X 轴列
    P_colorArr:{type: Array,default: null },//线条 颜色数组
    P_ShowLable:{type: Object,default: null },//数据 key 对应显示名称
    CardFlow_config:{type: Object,default: null },//样式 配置
  },
  data() {
    return {
      chart: null
    }
  },
  watch: {
    chartData: {
      deep: true,
      handler(val) {
        this.setOptions(val)
      }
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
      this.setOptions(this.chartData)
    },
    setOptions(chartData) {

      let series_data = [];
      let legend_data = [];

      let i = 0 ;
      let CardFlow_config = this.CardFlow_config;
      let smooth = false;//是否平滑过渡
      let areaStyle_show = "false";//是否 显示阴影部分

      if(CardFlow_config!=null){
        smooth = tools.Is_null(CardFlow_config.smooth)?CardFlow_config.smooth:smooth;
        areaStyle_show = tools.Is_null(CardFlow_config.areaStyle_show)?CardFlow_config.areaStyle_show:areaStyle_show;
      }


      //赋值数据
      for (let key in chartData) {
        //console.log(key);
        //console.log(chartData[key]);
        let color = this.P_colorArr[i];
        color = tools.Is_null(color)?color:"#3c97fd";

        let lable = this.P_ShowLable[key];
        let Obj_map = {
          name: lable, itemStyle: {
            normal: {
              color: color,
              lineStyle: {
                color: color,
                width: 2
              }
            }
          },
          label: {
            show: true,
            position: 'top'
          },
          smooth: smooth,
          type: 'line',
          data: chartData[key],
          animationDuration: 600,//动画执行时间
          animationEasing: 'cubicInOut'
        };
        if(areaStyle_show=="true"){
          Obj_map.areaStyle = {};
        }

        i+=1;
        legend_data.push(lable);
        series_data.push(Obj_map);
      }

      this.chart.setOption({
        xAxis: {
          data: this.P_xAxis,
          type: 'category',
          boundaryGap: false,
          axisTick: {
            show: false
          }
        },
        grid: {
          left: 10,
          right: 10,
          bottom: 20,
          top: 30,
          containLabel: true
        },
        tooltip: {
          trigger: 'axis',
          axisPointer: {
            type: 'cross'
          },
          padding: [5, 10]
        },
        yAxis: {
          axisTick: {
            show: false
          }
        },
        legend: {
          data: legend_data
        },
        series: series_data
      })
    }
  }
}
</script>
