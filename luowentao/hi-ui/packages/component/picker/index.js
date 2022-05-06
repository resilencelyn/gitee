import time from "./src/time";

time.install = function (Vue){
    Vue.component(time.name,time)
}

export default time;
