import dialog from "./src/dialog";

dialog.install = function (Vue){
    Vue.component(dialog.name,dialog);
}

export default dialog;
