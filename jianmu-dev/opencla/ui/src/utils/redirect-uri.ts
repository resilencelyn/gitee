// 动态构建redirectUri
export function getRedirectUri(email?: string, returnUrl?: string) {
  const address = `${location.protocol}//${location.host}/agreement`;
  if (email && returnUrl) {
    return `${address}?email=${encodeURIComponent(email)}&returnUrl=${encodeURIComponent(returnUrl)}`;
  }
  if (email && !returnUrl) {
    return `${address}?email=${encodeURIComponent(email)}`;
  }
  if (!email && returnUrl) {
    return `${address}?returnUrl=${encodeURIComponent(returnUrl)}`;
  }
  return `${address}`;
}