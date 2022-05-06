import row from "./src/row";

row.install = function (Vue){
    Vue.component(row.name,row)
}

export default row;
