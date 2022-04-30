<template>
  <div ref="main" style="width: 100%; height: 100%; display: flex; flex-flow: column nowrap; overflow: hidden">
    <div ref="header" style="display: flex; justify-content: flex-start; ">
      <decision-autocomplete v-model="query.decisionId" style="width: 250px; margin-right: 8px" />
      <el-date-picker v-model="query.startTime" type="datetime" placeholder="开始时间" :editable="false" :clearable="false" :disabledDate="(d) => d.getTime() > Date.now()" @blur="update"/>
      -
      <el-date-picker v-model="query.endTime" type="datetime" placeholder="结束时间" :editable="false" :disabledDate="(d) => d.getTime() < startTime" @blur="update"/>
    </div>
    <div ref="chart" style="width: 100%; flex: 1; margin-top: 16px"></div>
  </div>
</template>

<script>
import resizer from '../../js/lib/element-resize-detector.min.js'
import * as echarts from '../../js/lib/echarts.min.js'
import DecisionAutocomplete from "../component/DecisionAutocomplete.vue";

export default {
  name: "RuleCountChart",
  components: {DecisionAutocomplete},
  data() {
    const d = new Date()
    return {
      query: {
        startTime: new Date(d.getFullYear(), d.getMonth(), d.getDate()),
        endTime: null,
      }
    }
  },
  created() {
    this.option = {
      color: [
        '#ee6666', '#fac858', '#91cc75',
        '#5470c6', '#fc8452', '#73c0de',
        '#3ba272', '#9a60b4', '#ea7ccc'
      ],
      tooltip: {
        trigger: 'axis',
        axisPointer: {            // 坐标轴指示器，坐标轴触发有效
          type: 'shadow'        // 默认为直线，可选为：'line' | 'shadow'
        },
      },
      legend: {
        data: ['通过', '人工', '拒绝', ]
      },
      grid: {
        left: '3%', right: '4%', bottom: '3%', containLabel: true
      },
      xAxis: {
        type: 'value'
      },
    }
  },
  mounted() {
    resizer().listenTo(this.$refs.header, () => {
      if (!this.chart) {
        this.chart = echarts.init(this.$refs.chart)
        this.update()
      }
    })
    resizer().listenTo(this.$refs.chart, () => {
      if (!this.chart) return
      this.chart.resize()
    })
  },
  watch: {
    'query.decisionId'() {
      this.update()
    }
  },
  methods: {
    update() {
      const buildOpt = (resp) => {
        const cate = Array.from(new Set(resp.data.map(o => {
          o._label = (this.query.decisionId ? '' : o.decisionName + ' || ') + o.policyName + (o.ruleName ? ' || ' + o.ruleName : '')
          return o._label
        }))).reverse();
        this.option.yAxis = {type: 'category', data: cate};
        this.option.series = [
          {
            name: '拒绝', type: 'bar', stack: '总量',
            label: {
              show: true, position: 'insideRight',
              normal: {
                show: true,
                formatter(params) {
                  if (params.value > 0) {
                    return params.value;
                  } else {
                    return '';
                  }
                }
              }
            },
            data: (() => {
              const arr = [];
              cate.forEach((value, index, array) => {
                const item = resp.data.find(o => o.result === 'Reject' && o._label === value);
                arr.push(item ? item.total : 0);
              });
              return arr;
            })()
          },
          {
            name: '人工', type: 'bar', stack: '总量',
            label: {
              show: true, position: 'insideRight',
              normal: {
                show: true,
                formatter(params) {
                  if (params.value > 0) {
                    return params.value;
                  } else {
                    return '';
                  }
                }
              }
            },
            data: (() => {
              const arr = [];
              cate.forEach((value, index, array) => {
                const item = resp.data.find(o => o.result === 'Review' && o._label === value);
                arr.push(item ? item.total : 0);
              });
              return arr;
            })()
          },
          {
            name: '通过', type: 'bar', stack: '总量',
            label: {
              show: true, position: 'insideRight',
              normal: {
                show: true,
                formatter(params) {
                  if (params.value > 0) {
                    return params.value;
                  } else {
                    return '';
                  }
                }
              }
            },
            data: (() => {
              const arr = [];
              cate.forEach((value, index, array) => {
                const item = resp.data.find(o => (o.result === 'Accept' || !o.result) && o._label === value);
                arr.push(item ? item.total : 0);
              });
              return arr;
            })()
          },
        ];
        this.chart.setOption(this.option)
      }
      const loading = this.$loading({target: this.$refs.chart})
      axios.get(`decision/countRuleResult?${toQuery(this.query)}`).then(resp => {
        loading.close()
        if (resp.code === '00') buildOpt(resp)
      }).catch(e => loading.close())
    }
  }
}
</script>

<style scoped>
</style>