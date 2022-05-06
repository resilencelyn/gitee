import Vue from 'vue';
import HiToast from "./toast";
let ToastConstructor = Vue.extend(HiToast);

let instance;

const toast = function (opt){
    instance = new ToastConstructor({
        data: opt
    });
    instance.$mount();
    document.body.appendChild(instance.$el);
    instance.visible = true;
    return instance;
}

export default toast;
