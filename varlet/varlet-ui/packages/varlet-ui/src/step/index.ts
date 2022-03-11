import type { App } from 'vue'
import Step from './Step.vue'

Step.install = function (app: App) {
  app.component(Step.name, Step)
}

export const _StepComponent = Step

export default Step
