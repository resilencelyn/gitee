import progress from "./src/progress";

progress.install = function (Vue){
    Vue.install(progress.name,progress)
}

export default progress;
