import { Context } from '../handle/gen';

export class VirtualValue<T> {
    parent: any;
    key: string = '';

    constructor(public value: T, public context: Context) {}
    replaceToReal() {
        this.parent[this.key] = this.value;
        this.parent = null;
    }
}
