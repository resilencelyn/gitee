<template>
  <div style="min-height: 420px">
    <div style="display: flex; justify-content: space-around; width: 90%">
      <el-button type="primary" @click="create" circle>
        <el-icon><svg-plus/></el-icon>
      </el-button>
      <el-button type="primary" @click="save" circle>保存</el-button>
    </div>
    <el-table :data="decision.apiConfig" max-height="620px" :row-key="(row) => row._id" :expand-row-keys="expandRowKeys" @expand-change="expandChange">
      <el-table-column type="expand">
        <template #default="{row}">
          <el-form ref="form" v-model="row" label-width="100px" style="width: calc(100% - 10px)">
            <el-form-item label="参数名字" prop="code" :rules="rules.code(row)">
              <el-input v-model="row.code" :readonly="row._readonly"/>
            </el-form-item>
            <el-form-item label="参数说明" prop="name">
              <el-input v-model="row.name"/>
            </el-form-item>
            <template v-if="row.type === 'Str'">
              <el-row>
                <el-col :span="12">
                  <el-form-item label="固定值" prop="fixValue">
                    <el-input v-model="row.fixValue" :readonly="row._readonly && row.fixValue != null"/>
                  </el-form-item>
                </el-col>
                <el-col :span="12">
                  <el-form-item label="默认值" prop="defaultValue">
                    <el-input v-model="row.defaultValue" :readonly="row._readonly && row.fixValue != null"/>
                  </el-form-item>
                </el-col>
              </el-row>

              <el-form-item label="枚举值" v-if="row.fixValue == null || row.fixValue === ''" prop="enumValues">
                <el-tag v-for="v in row.enumValues" closable @close="() => row.enumValues.delete(v)" style="margin-right: 4px">{{v}}</el-tag>
                <el-input v-if="row._tagInputVisible" autofocus v-model="row._tagInputValue" style="width: 100px"
                          size="small" @keyup.enter="row._addEnumValue" @blur="row._addEnumValue" />
                <el-button v-else type="success" size="small" @click="row._tagInputVisible = true">
                  <el-icon><svg-plus /></el-icon> 添加
                </el-button>
              </el-form-item>
              <el-row>
                <el-col :span="8">
                  <el-form-item label="最小长度" prop="minLength" :rules="rules.minLength(row)">
                    <el-input-number v-model="row.minLength" clearable controls-position="right" style="width: 100%"/>
                  </el-form-item>
                </el-col>
                <el-col :span="8">
                  <el-form-item label="最大长度" prop="maxLength" :rules="rules.maxLength(row)">
                    <el-input-number v-model="row.maxLength" clearable controls-position="right" style="width: 100%"/>
                  </el-form-item>
                </el-col>
                <el-col :span="8">
                  <el-form-item label="固定长度" prop="fixLength">
                    <el-input-number v-model="row.fixLength" clearable controls-position="right" style="width: 100%"/>
                  </el-form-item>
                </el-col>
              </el-row>

              <el-form-item label="正则验证" prop="regex">
                <el-input v-model="row.regex"/>
              </el-form-item>
            </template>

            <template v-if="row.type === 'Int'">
              <el-row>
                <el-col :span="12">
                  <el-form-item label="固定值" prop="fixValue" :rules="rules.fixValue(row)">
                    <el-input-number v-model="row.fixValue" clearable :readonly="row._readonly && row.fixValue != null" style="width: 100%"/>
                  </el-form-item>
                </el-col>
                <el-col :span="12" v-if="row.fixValue === undefined || row.fixValue == null || Number.isNaN(row.fixValue) || row.fixValue === ''">
                  <el-form-item label="默认值" prop="defaultValue" :rules="rules.defaultValue(row)">
                    <el-input-number v-model="row.defaultValue" clearable style="width: 100%"/>
                  </el-form-item>
                </el-col>
              </el-row>
              <el-row
                  v-if="row.fixValue === undefined || row.fixValue == null || Number.isNaN(row.fixValue) || row.fixValue === ''">
                <el-col :span="12">
                  <el-form-item label="最小值" prop="min" :rules="rules.min(row)">
                    <el-input-number v-model="row.min" clearable style="width: 100%"/>
                  </el-form-item>
                </el-col>
                <el-col :span="12">
                  <el-form-item label="最大值" prop="max" :rules="rules.max(row)">
                    <el-input-number v-model="row.max" clearable style="width: 100%"/>
                  </el-form-item>
                </el-col>
              </el-row>
            </template>

            <template v-if="row.type === 'Decimal'">
              <el-row>
                <el-col :span="12">
                  <el-form-item label="固定值" prop="fixValue" :rules="rules.fixValue(row)">
                    <el-input-number v-model="row.fixValue" :precision="8" clearable
                                     :readonly="row._readonly && row.fixValue != null" style="width: 100%"/>
                  </el-form-item>
                </el-col>
                <el-col :span="12">
                  <el-form-item label="默认值" prop="defaultValue" :rules="rules.defaultValue(row)">
                    <el-input-number v-model="row.defaultValue" :precision="8" clearable style="width: 100%"/>
                  </el-form-item>
                </el-col>
              </el-row>
              <el-row>
                <el-col :span="12">
                  <el-form-item label="最小值" prop="min" :rules="rules.min(row)">
                    <el-input-number v-model="row.min" :precision="8" clearable style="width: 100%"/>
                  </el-form-item>
                </el-col>
                <el-col :span="12">
                  <el-form-item label="最大值" prop="max" :rules="rules.max(row)">
                    <el-input-number v-model="row.max" :precision="8" clearable style="width: 100%"/>
                  </el-form-item>
                </el-col>
              </el-row>
            </template>

            <el-row v-if="row.type === 'Bool'">
              <el-col :span="12">
                <el-form-item label="固定值">
                  <el-select v-model="row.fixValue" style="width: 100%">
                    <el-option label="是" value="true"/>
                    <el-option label="否" value="false"/>
                  </el-select>
                </el-form-item>
              </el-col>
              <el-col :span="12">
                <el-form-item label="默认值">
                  <el-select v-model="row.defaultValue" style="width: 100%">
                    <el-option label="是" value="true"/>
                    <el-option label="否" value="false"/>
                  </el-select>
                </el-form-item>
              </el-col>
            </el-row>

            <el-form-item v-if="row.type === 'Time'" label="时间格式">
              <el-input v-model="row.format" placeholder="例: yyyy-MM-dd HH:mm:ss"/>
            </el-form-item>

            <el-form-item label="验证函数" prop="validFun">
              <ace-groovy v-model="row.validFun" style="height: 140px" placeholder="// 函数例子
{value ->
  if (value == null) throw new IllegalArgumentException('Required')
  return true // 验证通过
}
                  "/>
            </el-form-item>
          </el-form>
        </template>
      </el-table-column>
      <el-table-column prop="code" >
        <template #header>
          <span style="color: red">*</span>参数名字
        </template>
      </el-table-column>
      <el-table-column width="120" >
        <template #header>
          <span style="color: red">*</span>类型
        </template>
        <template #default="{row}">
          <el-select v-model="row.type" :disabled="row._readonly" @change="typeChange(row)">
            <el-option v-for="t in $cfg.fieldType" :label="t.name" :value="t.key"/>
          </el-select>
        </template>
      </el-table-column>
      <el-table-column label="必须" width="68">
        <template #default="{row}">
          <el-switch v-model="row.require" :disabled="row._readonly"/>
        </template>
      </el-table-column>
      <el-table-column prop="name" label="参数说明" show-overflow-tooltip />
      <el-table-column label="操作" width="68" >
        <template #default="{row}">
          <el-space>
            <el-button v-if="!row._readonly" type="danger" circle size="mini" @click.stop="del(row)">
              <el-icon> <svg-delete/> </el-icon>
            </el-button>
          </el-space>
        </template>
      </el-table-column>
    </el-table>
  </div>
</template>

<script>
import SvgPlus from "../../../component/svg/SvgPlus.vue";
import AceGroovy from "../../../component/AceGroovy.vue";
import SvgDelete from "../../../component/svg/SvgDelete.vue";

export default {
  name: "ApiConfig",
  props: {
    decision: {type: Object, required: true}
  },
  components: {SvgDelete, AceGroovy, SvgPlus},
  data() {
    return {
      expandRowKeys: [],
      rules: {
        code(item) {
          return [
            {
              type: 'string', trigger: 'blur',
              validator(rule, value, callback) {
                if (item.code == null || item.code === '') {
                  callback(new Error('必填'))
                } callback()
              }
            }
          ]
        },
        min(item) {
          return [{
            type: item.type === 'Int' ? 'integer' : 'number', trigger: ['change', 'blur'],
            validator(rule, value, callback) {
              if (item.max == null || item.min == null) callback()
              else if (item.min > item.max) callback(new Error('不能大于 最大值'))
              else callback()
            }
          }]
        },
        max(item) {
          return [{
            trigger: ['change', 'blur'],
            validator(rule, value, callback) {
              if (item.max == null || item.min == null) callback()
              else if (item.min > item.max) callback(new Error('不能小于 最小值'))
              else callback()
            }
          }]
        },
        minLength(item) {
          return [{
            trigger: ['change', 'blur'],
            validator(rule, value, callback) {
              if (item.maxLength == null || item.minLength == null) callback()
              else if (item.minLength > item.maxLength) callback(new Error('不能大于 最大长度'))
              else callback()
            }
          }]
        },
        maxLength(item) {
          return [{
            trigger: ['change', 'blur'],
            validator(rule, value, callback) {
              if (item.maxLength == null || item.minLength == null) callback()
              else if (item.minLength > item.maxLength) callback(new Error('不能小于 最小长度'))
              else callback()
            }
          }]
        },
        defaultValue(item) {
          return [{
            trigger: ['change', 'blur'],
            validator(rule, value, callback) {
              if (item.type === 'Int' || item.type === 'Decimal') {
                if (value > item.max) callback(new Error('不能大于 最大值'))
                else if (value < item.min) callback(new Error('不能小于 最小值'))
              }
            }
          }]
        },
        fixValue(item) {
          return [{
            trigger: ['change', 'blur'],
            validator(rule, value, callback) {
              if (item.fixValue != null && item.fixValue !== '') {
                item.enumValues = null
              }
              callback()
            }
          }]
        },
      }
    }
  },
  created() {
    this.decision.apiConfig.map(item => {
      item._id = uuid()
      item._addEnumValue = () => {
        if (item._tagInputValue != null && item._tagInputValue !== '') {
          item.enumValues = item.enumValues || new Set()
          item.enumValues.add(item._tagInputValue);
          item._tagInputValue = null;
          item._tagInputVisible = false
        }
      }
    })
  },
  methods: {
    expandChange(row, o) {
      if (o.length) {
        this.expandRowKeys = o.map(i => i._id)
      } else {
        this.expandRowKeys.splice(this.expandRowKeys.indexOf(o._id), 1)
      }
    },
    typeChange(item) {
      if (item.type === 'Str') {
        item.minLength = NaN
        item.maxLength = NaN
        item.fixValue = null
        item.defaultValue = null
        item.enumValues = null
        item.regex = null
        item.validFun = null
        delete item.min
        delete item.max
      } else if (item.type === 'Int' || item.type === 'Decimal') {
        item.min = NaN
        item.max = NaN
        item.fixValue = NaN
        item.defaultValue = NaN
        item.enumValues = null
        delete item.minLength
        delete item.maxLength
        delete item.regex
      } else if (item.type === 'Bool') {
        item.fixValue = null
        item.defaultValue = null
        delete item.min
        delete item.max
        delete item.enumValues
        delete item.minLength
        delete item.maxLength
        delete item.regex
      } else if (item.type === 'Time') {
        item.format = null
        item.defaultValue = null
        item.enumValues = null
        delete item.min
        delete item.max
        delete item.fixValue
        delete item.minLength
        delete item.maxLength
        delete item.regex
      }
    },
    create() {
      const p = {
        _id: uuid(),
        code: `param_${this.decision.apiConfig.length + 1}`,
        name: `参数${this.decision.apiConfig.length + 1}`,
        type: 'Str',
      }
      this.typeChange(p)
      this.decision.apiConfig.push(p)
      if (this.expandRowKeys.length < 1) this.expandRowKeys.push(p._id)
    },
    save() {
      if (this.decision.id) {
        const cfgItems = clone(this.decision.apiConfig)
        for (let i in cfgItems) {
          // console.log('=====key: ', cfgItems[i])
          delete cfgItems[i]['_id']
          delete cfgItems[i]['_addEnumValue']
          delete cfgItems[i]['_tagInputValue']
          delete cfgItems[i]['_tagInputVisible']
        }
        axios.post(`decision/${this.decision.id}/updateApiConfig`, {apiConfig: cfgItems}).then(resp => {
          if (resp.code === '00') {
            this.decision.updaterName = this.$user.name
            this.$message.success(`更新新决策(${this.decision.name})API配置成功`)
          }
        })
      }
      else { // add new
        const d = clone(this.decision)
        for (let i in this.decision.apiConfig) {
          delete this.decision.apiConfig[i]['_id']
          delete this.decision.apiConfig[i]['_addEnumValue']
          delete this.decision.apiConfig[i]['_tagInputValue']
          delete this.decision.apiConfig[i]['_tagInputVisible']
        }
        axios.put('decision/', d).then(resp => {
          if (resp.code === '00') {
            this.$message.success(`添加新决策: ${this.decision.name} 成功`)
            this.decision = clone(resp.data)
            if (typeof this.decision.apiConfig === 'string') {
              this.decision.apiConfig = toJSON(this.decision.apiConfig)
            }
          }
        })
      }
    },
    del(item) {
      this.decision.apiConfig.splice(this.decision.apiConfig.indexOf(item), 1);
    },
  }
}
</script>

<style scoped>

</style>