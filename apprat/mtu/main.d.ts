declare interface Window {
  Mtu: {
    toast(text: string, options?: {
      action?: string
      onClose?(): void,
      duration?: number
    }): void
    dialog(option?: {
      title?: string
      message?: string
      positive?: string
      onPositive?(): void
      negative?: string
      onNegative?(): void
      onClose?(): void
      size?: 'normal' | 'large' | 'full'
    }): void
  }
}