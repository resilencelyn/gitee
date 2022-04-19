import { Laya } from "Laya";
import { BaseCamera } from "laya/d3/core/BaseCamera";
import { Camera } from "laya/d3/core/Camera";
import { DirectionLight } from "laya/d3/core/light/DirectionLight";
import { EffectMaterial } from "laya/d3/core/material/EffectMaterial";
import { MeshSprite3D } from "laya/d3/core/MeshSprite3D";
import { Scene3D } from "laya/d3/core/scene/Scene3D";
import { Vector3 } from "laya/d3/math/Vector3";
import { Vector4 } from "laya/d3/math/Vector4";
import { PrimitiveMesh } from "laya/d3/resource/models/PrimitiveMesh";
import { Stage } from "laya/display/Stage";
import { Texture2D } from "laya/resource/Texture2D";
import { Handler } from "laya/utils/Handler";
import { Stat } from "laya/utils/Stat";
import { Laya3D } from "Laya3D";


/**
 * ...
 * @author ...
 */
export class EffectMaterialDemo {
	private rotation: Vector3 = new Vector3(0, 0.01, 0);

	constructor() {
		Laya3D.init(0, 0);
		Laya.stage.scaleMode = Stage.SCALE_FULL;
		Laya.stage.screenMode = Stage.SCREEN_NONE;
		Stat.show();

		var scene: Scene3D = (<Scene3D>Laya.stage.addChild(new Scene3D()));

		var camera: Camera = (<Camera>(scene.addChild(new Camera(0, 0.1, 100))));
		camera.transform.translate(new Vector3(0, 0.5, 1.5));
		camera.transform.rotate(new Vector3(-15, 0, 0), true, false);
		camera.clearFlag = BaseCamera.CLEARFLAG_SKY;

		var directionLight: DirectionLight = (<DirectionLight>scene.addChild(new DirectionLight()));
		directionLight.color.setValue(1, 1, 1);

		var earth: MeshSprite3D = (<MeshSprite3D>scene.addChild(new MeshSprite3D(PrimitiveMesh.createSphere())));
		earth.transform.position = new Vector3(0, 0, 0);
		//创建EffectMaterial材质
		var material: EffectMaterial = new EffectMaterial();
		Texture2D.load("res/threeDimen/texture/earth.png", Handler.create(this, function (texture: Texture2D): void {
			//设置纹理
			material.texture = texture;
			//设置材质颜色
			material.color = new Vector4(0.6, 0.6, 0.6, 1);
		}));
		earth.meshRenderer.material = material;

		Laya.timer.frameLoop(1, this, function (): void {
			earth.transform.rotate(this.rotation, false);
		});
	}

}


