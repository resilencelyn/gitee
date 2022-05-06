import Vue from 'vue';
import HiConfirm from './confirm';
let ConfirmConstructor = Vue.extend(HiConfirm);

let confirmInstance;

const confirm = opt => {
    return new Promise((resolve, reject) => {
        confirmInstance = new ConfirmConstructor({
            data:opt,
            el:document.createElement('div')
        });
        confirmInstance.isShow = true;
        confirmInstance.$mount();
        document.body.appendChild(confirmInstance.$el);
        confirmInstance.confirm = () => {
            resolve();
            confirmInstance.isShow = false;
        }
        confirmInstance.cancel = () => {
            reject();
            confirmInstance.isShow = false;
        }
    });
}

export default {
    confirm
}
