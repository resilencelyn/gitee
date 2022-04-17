import PageBase from "../pages/PageBase";
import TabBar from "../core/TabBar";

/**
 * 全局标签页
 */
export default class GlobalTabBar extends TabBar{
    setPage(page:PageBase){
        super.setPage(page);
    }
}