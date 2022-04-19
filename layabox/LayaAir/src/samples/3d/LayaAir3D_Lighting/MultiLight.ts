import { Config3D } from "Config3D";
import { Laya } from "Laya";
import { Script3D } from "laya/d3/component/Script3D";
import { Camera } from "laya/d3/core/Camera";
import { LightSprite } from "laya/d3/core/light/LightSprite";
import { PointLight } from "laya/d3/core/light/PointLight";
import { SpotLight } from "laya/d3/core/light/SpotLight";
import { Scene3D } from "laya/d3/core/scene/Scene3D";
import { Transform3D } from "laya/d3/core/Transform3D";
import { Vector3 } from "laya/d3/math/Vector3";
import { Stage } from "laya/display/Stage";
import { Handler } from "laya/utils/Handler";
import { Stat } from "laya/utils/Stat";
import { Laya3D } from "Laya3D";
import { CameraMoveScript } from "../common/CameraMoveScript";

class LightMoveScript extends Script3D {
	forward: Vector3 = new Vector3();
	lights: LightSprite[] = [];
	offsets: Vector3[] = [];
	moveRanges: Vector3[] = [];

	onUpdate(): void {
		var seed: number = Laya.timer.currTimer * 0.002;
		for (var i: number = 0, n: number = this.lights.length; i < n; i++) {
			var transform: Transform3D = this.lights[i].transform;
			var pos: Vector3 = transform.localPosition;
			var off: Vector3 = this.offsets[i];
			var ran: Vector3 = this.moveRanges[i];
			pos.x = off.x + Math.sin(seed) * ran.x;
			pos.y = off.y + Math.sin(seed) * ran.y;
			pos.z = off.z + Math.sin(seed) * ran.z;
			transform.localPosition = pos;
		}
	}
}

export class MultiLight {

	constructor() {
		var c = new Config3D();
		c.maxLightCount = 16;
		Laya3D.init(0, 0, c);
		Laya.stage.scaleMode = Stage.SCALE_FULL;
		Laya.stage.screenMode = Stage.SCREEN_NONE;
		Stat.show();
		Config3D.useCannonPhysics = false;
		Scene3D.load("res/threeDimen/scene/MultiLightScene/InventoryScene_Forest.ls", Handler.create(this, function (scene: Scene3D): void {
			Laya.stage.addChild(scene);

			var camera: Camera = <Camera>scene.getChildByName("Main Camera");
			camera.addComponent(CameraMoveScript);
			camera.transform.localPosition = new Vector3(8.937199060699333, 61.364798067809126, -66.77836086472654);

			var moveScript: LightMoveScript = camera.addComponent(LightMoveScript);
			var moverLights: LightSprite[] = moveScript.lights;
			var offsets: Vector3[] = moveScript.offsets;
			var moveRanges: Vector3[] = moveScript.moveRanges;
			moverLights.length = 15;
			for (var i: number = 0; i < 15; i++) {
				var pointLight: PointLight = (<PointLight>scene.addChild(new PointLight()));
				pointLight.range = 2.0 + Math.random() * 8.0;
				pointLight.color.setValue(Math.random(), Math.random(), Math.random());
				pointLight.intensity = 6.0 + Math.random() * 8;
				moverLights[i] = pointLight;
				offsets[i] = new Vector3((Math.random() - 0.5) * 10, pointLight.range * 0.75, (Math.random() - 0.5) * 10);
				moveRanges[i] = new Vector3((Math.random() - 0.5) * 40, 0, (Math.random() - 0.5) * 40);
			}

			var spotLight: SpotLight = (<SpotLight>scene.addChild(new SpotLight()));
			spotLight.transform.localPosition = new Vector3(0.0, 9.0, -35.0);
			spotLight.transform.localRotationEuler = new Vector3(-15.0, 180.0, 0.0);
			spotLight.color.setValue(Math.random(), Math.random(), Math.random());
			spotLight.range = 50;
			spotLight.intensity = 15;
			spotLight.spotAngle = 60;
		}));

	}
}

