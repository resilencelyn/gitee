import card from "./src/card";

card.install = function(Vue) {
    Vue.component(card.name, card);
};

export default card;
