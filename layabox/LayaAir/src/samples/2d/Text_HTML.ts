import { Laya } from "Laya";
import { Stage } from "laya/display/Stage";
import { HTMLDivElement } from "laya/html/dom/HTMLDivElement";
import { Browser } from "laya/utils/Browser";
import { WebGL } from "laya/webgl/WebGL";
import { Main } from "../Main";

export class Text_HTML {
	Main: typeof Main = null;
	constructor(maincls: typeof Main) {
		this.Main = maincls;

		// 不支持WebGL时自动切换至Canvas
		Laya.init( Browser.clientWidth, Browser.clientHeight, WebGL);

		Laya.stage.alignV = Stage.ALIGN_MIDDLE;
		Laya.stage.alignH = Stage.ALIGN_CENTER;

		Laya.stage.scaleMode = "showall";
		Laya.stage.bgColor = "#232628";

		this.setup();
	}

	private setup(): void {
		this.createParagraph();	// 代码创建
	}
	private createParagraph(): void {
		var p: HTMLDivElement = new HTMLDivElement();
		this.Main.box2D.addChild(p);
		p.zOrder = 90000;
		p.style.font = "Impact";
		p.style.fontSize = 30;

		var html: string = "<span color='#e3d26a'>使用</span>";
		html += "<span style='color:#FFFFFF;font-weight :bold'>HTMLDivElement</span>";
		html += "<span color='#6ad2e3'>创建的</span><br/>";
		html += "<span color='#d26ae3'>HTML文本</span>";

		p.innerHTML = html;
	}
}

