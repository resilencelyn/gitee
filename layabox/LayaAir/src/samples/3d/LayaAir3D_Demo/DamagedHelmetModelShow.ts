
import { Laya } from "Laya";
import { Script3D } from "laya/d3/component/Script3D";
import { MeshSprite3D } from "laya/d3/core/MeshSprite3D";
import { Scene3D } from "laya/d3/core/scene/Scene3D";
import { Sprite3D } from "laya/d3/core/Sprite3D";
import { Vector3 } from "laya/d3/math/Vector3";
import { Stage } from "laya/display/Stage";
import { Text } from "laya/display/Text";
import { Event } from "laya/events/Event";
import { MouseManager } from "laya/events/MouseManager";
import { Browser } from "laya/utils/Browser";
import { Handler } from "laya/utils/Handler";
import { Laya3D } from "Laya3D";

/**
 * model rotation script.
 */
class RotationScript extends Script3D {
	private _lastMouseX: number;
	private _mouseDown: boolean = false;
	private _rotate: Vector3 = new Vector3();
	private _autoRotateSpeed: Vector3 = new Vector3(0, 0.25, 0);

	model: Sprite3D;

	constructor() {
		super();
		Laya.stage.on(Event.MOUSE_DOWN, this, function (): void {
			this._mouseDown = true;
			this._lastMouseX = MouseManager.instance.mouseX;
		});
		Laya.stage.on(Event.MOUSE_UP, this, function (): void {
			this._mouseDown = false;
		});

	}
	onUpdate(): void {
		if (this._mouseDown) {
			var deltaX: number = MouseManager.instance.mouseX - this._lastMouseX;
			this._rotate.y = deltaX * 0.2;
			this.model.transform.rotate(this._rotate, false, false);
			this._lastMouseX = MouseManager.instance.mouseX;
		}
		else {
			this.model.transform.rotate(this._autoRotateSpeed, false, false);
		}
	}
}

export class DamagedHelmetModelShow {
	constructor() {
		Laya3D.init(0, 0);
		Laya.stage.scaleMode = Stage.SCALE_FULL;
		Laya.stage.screenMode = Stage.SCREEN_NONE;

		Scene3D.load("res/threeDimen/scene/LayaScene_DamagedHelmetScene/Conventional/DamagedHelmetScene.ls", Handler.create(this, function (scene: Scene3D): void {
			Laya.stage.addChild(scene);

			var damagedHelmet: MeshSprite3D = <MeshSprite3D>scene.getChildAt(1).getChildAt(0);
			var rotationScript: RotationScript = damagedHelmet.addComponent(RotationScript);
			rotationScript.model = damagedHelmet;

			var size: number = 20;
			this.addText(size, size * 4, "Drag the screen to rotate the model.", "#F09900");
			size = 10;
			this.addText(size, Laya.stage.height - size * 4, "Battle Damaged Sci-fi Helmet by theblueturtle_    www.leonardocarrion.com", "#FFFF00");
		}));
	}

	/**
	 * add text.
	 */
	addText(size: number, y: number, text: string, color: string): void {
		var cerberusText: Text = new Text();
		cerberusText.color = color;
		cerberusText.fontSize = size * Browser.pixelRatio;
		cerberusText.x = size;
		cerberusText.y = y;
		cerberusText.text = text;
		Laya.stage.addChild(cerberusText);
	}
}



