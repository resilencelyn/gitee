// 用于生成 Blob 和 File 型数据的工具
import { paragraph, word } from './text/text_en.js';

export function genFile(anyData?: string) {
    return new File([anyData ?? paragraph()], word());
}
export function genBlob(anyData?: string) {
    return new Blob([anyData ?? paragraph()]);
}
