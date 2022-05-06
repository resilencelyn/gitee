import swipe from "./src/swipe";

swipe.install = function (Vue){
    Vue.component(swipe.name,swipe);
}

export default swipe;
