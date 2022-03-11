import { VarComponent } from './varComponent'

export type TimeData = {
  days: number
  hours: number
  minutes: number
  seconds: number
  milliseconds: number
}

export interface CountdownProps {
  time?: string | number
  format?: string
  autoStart?: boolean
  onEnd?: () => void
  onChange?: (value: TimeData) => void
}

export class Countdown extends VarComponent {
  $props: CountdownProps

  start(): void

  pause(): void

  reset(): void
}

export class _CountdownComponent extends Countdown {}
