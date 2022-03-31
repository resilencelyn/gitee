// 判断设备是否为手机端
export function isWap(): boolean {
  // eslint-disable-next-line prefer-regex-literals
  return new RegExp(/(Android|iP(ad|hone|od)|Kindle|Blackberry|Windows\s(ce|phone))/i).test(navigator.userAgent);
}
