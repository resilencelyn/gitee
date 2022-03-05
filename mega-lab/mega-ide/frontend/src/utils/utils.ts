export const changeHardwareSize = (value: number, from: string, to: string) => {
  if (value === 0) return 0;
  const k = 1024;
  const sizes = ['B', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'];
  const fromIndex = sizes.findIndex((item) => item === from);
  const toIndex = sizes.findIndex((item) => item === to);
  if (fromIndex === -1 || toIndex === -1) {
    return value;
  }
  return value / Math.pow(k, toIndex - fromIndex);
};

export const autoFormatHardwareSize = (data: { value: number; unit?: string; }) => {
  const { value, unit = 'B' } = data;
  let to = '';
  const k = 1024;
  const sizes = ['B', 'KB', 'MB', 'GB', 'TB'];
  for (let i = 0; i < sizes.length; i++) {
    if (value < Math.pow(k, i + 1)) {
      to = sizes[i];
      break;
    }
  }
  if (!to) to = 'TB';
  const fromIndex = sizes.findIndex((item) => item === unit);
  const toIndex = sizes.findIndex((item) => item === to);
  if (fromIndex === -1 || toIndex === -1) {
    return { value, unit: unit };
  }
  return {
    value: value / Math.pow(k, toIndex - fromIndex),
    unit: to,
  };
};