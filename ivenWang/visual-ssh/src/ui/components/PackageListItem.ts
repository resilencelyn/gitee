import { Package } from "../../parser/PkgParser";
import BaseElement from "../core/BaseElement";
import Icon from "../core/Icon";
//fa-linux
export default class PackageListItem extends BaseElement{
    package:Package
    constructor(props:Package){
        super()
        this.package=props
    }
    render(): HTMLElement {
        this.style.display="flex";
        this.style.height="34px";
        let icon =new Icon({
            name:"linux",
            color:"#FFF",
            bgcolor:"#CC99FF",
            size:24
        });
        let pkgInfoBox=new BaseElement();
        pkgInfoBox.style.padding="0 5px";
        let pkgtitle= new BaseElement();
        pkgtitle.setInnerElement(this.package.name)
        let pkgData= new BaseElement();
        pkgData.style.display="flex";
        pkgData.style.fontSize="14px";
        pkgData.style.color="#BBB";

        let pkgVersion=new BaseElement();
        pkgVersion.setInnerElement(this.package.version)
        let pkgSource= new BaseElement();
        pkgSource.setInnerElement(this.package.source)

        pkgInfoBox.appendChild(pkgtitle);
        pkgInfoBox.appendChild(pkgData);
        pkgData.appendChild(pkgVersion);
        pkgData.appendChild(pkgSource);
        this.setInnerElement(icon);
        this.appendChild(pkgInfoBox);
        return super.render();
    }
}