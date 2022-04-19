import { Laya } from "Laya";
import { Sprite } from "laya/display/Sprite";
import { Stage } from "laya/display/Stage";
import { Event } from "laya/events/Event";
import { Browser } from "laya/utils/Browser";
import { WebGL } from "laya/webgl/WebGL";
import { Main } from "./../Main";

export class Interaction_Rotate {
	private sp: Sprite;
	private preRadian: number = 0;

	Main: typeof Main = null;
	constructor(maincls: typeof Main) {
		this.Main = maincls;

		// 不支持WebGL时自动切换至Canvas
		Laya.init(Browser.clientWidth, Browser.clientHeight, WebGL);
		//
		Laya.stage.alignV = Stage.ALIGN_MIDDLE;
		Laya.stage.alignH = Stage.ALIGN_CENTER;
		//
		Laya.stage.scaleMode = "showall";
		Laya.stage.bgColor = "#232628";

		this.setup();
	}

	private setup(): void {
		this.createSprite();

		Laya.stage.on(Event.MOUSE_UP, this, this.onMouseUp);
		Laya.stage.on(Event.MOUSE_OUT, this, this.onMouseUp);
	}

	private createSprite(): void {
		this.sp = new Sprite();
		var w: number = 200, h: number = 300;
		this.sp.graphics.drawRect(0, 0, w, h, "#FF7F50");
		this.sp.size(w, h);
		this.sp.pivot(w / 2, h / 2);
		this.sp.pos(Laya.stage.width / 2, Laya.stage.height / 2);
		this.Main.box2D.addChild(this.sp);

		this.sp.on(Event.MOUSE_DOWN, this, this.onMouseDown);
	}

	private onMouseDown(e: Event = null): void {
		var touches: any[] = e.touches;

		if (touches && touches.length == 2) {
			this.preRadian = Math.atan2(
				touches[0].stageY - touches[1].stageY,
				touches[0].stageX - touches[1].stageX);

			Laya.stage.on(Event.MOUSE_MOVE, this, this.onMouseMove);
		}
	}

	private onMouseMove(e: Event = null): void {
		var touches: any[] = e.touches;
		if (touches && touches.length == 2) {
			var nowRadian: number = Math.atan2(
				touches[0].stageY - touches[1].stageY,
				touches[0].stageX - touches[1].stageX);

			this.sp.rotation += 180 / Math.PI * (nowRadian - this.preRadian);

			this.preRadian = nowRadian;
		}
	}

	private onMouseUp(e: Event = null): void {
		Laya.stage.off(Event.MOUSE_MOVE, this, this.onMouseMove);
	}

	dispose(): void {
		if (this.sp) {
			this.sp.off(Event.MOUSE_DOWN, this, this.onMouseDown);
		}
		Laya.stage.off(Event.MOUSE_UP, this, this.onMouseUp);
		Laya.stage.off(Event.MOUSE_OUT, this, this.onMouseUp);
		Laya.stage.off(Event.MOUSE_MOVE, this, this.onMouseMove);
	}
}

