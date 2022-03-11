import type { App } from 'vue'
import DatePicker from './DatePicker.vue'

DatePicker.install = function (app: App) {
  app.component(DatePicker.name, DatePicker)
}

export const _DatePickerComponent = DatePicker

export default DatePicker
