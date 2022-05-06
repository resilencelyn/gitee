import product from "./src/product";

product.install = function (Vue){
    Vue.component(product.name,product);
}

export default product;
