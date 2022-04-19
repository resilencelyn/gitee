import { Laya } from "Laya";
import { MovieClip } from "laya/ani/swf/MovieClip";
import { Stage } from "laya/display/Stage";
import { Browser } from "laya/utils/Browser";
import { WebGL } from "laya/webgl/WebGL";
import { Main } from "./../Main";

export class Animation_SWF {
	private SWFPath: string = "res/swf/dragon.swf";

	private MCWidth: number = 318;
	private MCHeight: number = 406;

	Main: typeof Main = null;
	constructor(maincls: typeof Main) {
		this.Main = maincls;

		// 不支持WebGL时自动切换至Canvas
		Laya.init(Browser.clientWidth, Browser.clientHeight, WebGL);

		Laya.stage.alignV = Stage.ALIGN_MIDDLE;
		Laya.stage.alignH = Stage.ALIGN_CENTER;

		Laya.stage.scaleMode = "showall";
		Laya.stage.bgColor = "#232628";

		this.createMovieClip();
	}

	private createMovieClip(): void {
		var mc: MovieClip = new MovieClip();
		mc.load(this.SWFPath);

		mc.x = (Laya.stage.width - this.MCWidth) / 2;
		mc.y = (Laya.stage.height - this.MCHeight) / 2;

		this.Main.box2D.addChild(mc);
	}
}

