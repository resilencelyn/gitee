import Vue from 'vue'
import {Search,Button,Tag,Card,Row,Col,Progress,
        Circle,Overlay,Loading,Cell,CellGroup,Grid,GridItem,
        List,Popup,Dialog,Lazyload,NavBar,Tabbar,TabbarItem,
        Form,Field,Tab,Tabs,Empty,Swipe,SwipeItem ,Toast,
        Step,Steps,SubmitBar,Collapse,CollapseItem,Icon,Sidebar,SidebarItem,Image,PasswordInput,
        NumberKeyboard
} from 'vant'

Vue.use(NumberKeyboard );
Vue.use(PasswordInput);
Vue.use(Toast);
Vue.prototype.$toast=Toast
Vue.use(Image);
Vue.use(Sidebar);
Vue.use(SidebarItem);
Vue.use(Icon);
Vue.use(CollapseItem);
Vue.use(Collapse);
Vue.use(SubmitBar);
Vue.use(Popup);
Vue.use(Step);
Vue.use(Steps);
Vue.use(SwipeItem);
Vue.use(Swipe);
Vue.use(Empty);
Vue.use(Tab);
Vue.use(Tabs);
Vue.use(Field);
Vue.use(Form);
Vue.use(TabbarItem);
Vue.use(Tabbar);
Vue.use(NavBar);
Vue.use(Lazyload);
Vue.use(Dialog);
Vue.prototype.$dialog=Dialog
Vue.use(List);
Vue.use(Grid);
Vue.use(GridItem);
Vue.use(Cell);
Vue.use(CellGroup);
Vue.use(Loading);
Vue.use(Overlay);
Vue.use(Circle);
Vue.use(Progress);
Vue.use(Col);
Vue.use(Row);
Vue.use(Card);
Vue.use(Tag);
Vue.use(Search);
Vue.use(Button);
