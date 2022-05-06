import Vue from 'vue';
import HiLoading from './loading';
let LoadingConstructor = Vue.extend(HiLoading);

let instance;

const loading = (opt) => {
    instance = new LoadingConstructor({
        data:opt
    })
    instance.$mount();
    document.body.appendChild(instance.$el);
    instance.visible = true;
    return instance;
};

loading.close =function () {
    instance.close();
}

export default loading;
