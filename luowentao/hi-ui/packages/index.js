import card from "./component/card";
import col from "./component/col";
import dialog from "./component/dialog";
import dropdown from "./component/dropdown";
import header from "./component/header";
import loading from "./component/loading";
import timePicker from "./component/picker";
import product from "./component/product";
import progress from "./component/progress";
import row from "./component/row/src/row";
import swipe from "./component/swipe";
import ElSwitch from "./component/switch"
import toast from "./component/toast";
import messageBox from "./component/message-box"

const components = [
    card,
    col,
    dialog,
    dropdown,
    header,
    loading,
    timePicker,
    product,
    progress,
    row,
    swipe,
    ElSwitch,
    toast
]

function install(Vue){
    components.forEach(component=>{
        Vue.component(component.name,component);
    })

    Vue.prototype.$toast = toast;
    Vue.prototype.$loading = loading;
    Vue.prototype.$switchTheme = switchTheme;
    Vue.prototype.$messageBox = messageBox;
    function switchTheme(type){
        window.document.documentElement.setAttribute('data-theme', type);
        let themeEvent = new CustomEvent('selectTheme',{
            detail:{type:type}
        });
        window.dispatchEvent(themeEvent);
        console.log(window.document.documentElement.getAttribute('data-theme'))
    }
}

export default {
    install,
    row,
    col,
    header,
    product,
    toast,
    card,
    loading,
    dropdown,
    progress,
    swipe,
    dialog,
    timePicker,
    ElSwitch,
    messageBox
}
