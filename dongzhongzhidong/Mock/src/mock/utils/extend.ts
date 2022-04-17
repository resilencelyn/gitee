import { assignIn } from 'lodash-es';
export function extend(this: any, ...args: any) {
    return assignIn(this, ...args);
}
