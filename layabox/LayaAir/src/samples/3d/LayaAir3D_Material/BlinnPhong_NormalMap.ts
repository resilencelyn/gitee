import { Laya } from "Laya";
import { Camera } from "laya/d3/core/Camera";
import { DirectionLight } from "laya/d3/core/light/DirectionLight";
import { BlinnPhongMaterial } from "laya/d3/core/material/BlinnPhongMaterial";
import { MeshSprite3D } from "laya/d3/core/MeshSprite3D";
import { Scene3D } from "laya/d3/core/scene/Scene3D";
import { Sprite3D } from "laya/d3/core/Sprite3D";
import { Matrix4x4 } from "laya/d3/math/Matrix4x4";
import { Vector3 } from "laya/d3/math/Vector3";
import { Stage } from "laya/display/Stage";
import { Texture2D } from "laya/resource/Texture2D";
import { Handler } from "laya/utils/Handler";
import { Stat } from "laya/utils/Stat";
import { Laya3D } from "Laya3D";


export class BlinnPhong_NormalMap {
	private scene: Scene3D;
	private rotation: Vector3 = new Vector3(0, 0.01, 0);
	private normalMapUrl: any[] = ["res/threeDimen/staticModel/lizard/Assets/Lizard/lizardeye_norm.png", "res/threeDimen/staticModel/lizard/Assets/Lizard/lizard_norm.png", "res/threeDimen/staticModel/lizard/Assets/Lizard/rock_norm.png"];

	constructor() {
		Laya3D.init(0, 0);
		Laya.stage.scaleMode = Stage.SCALE_FULL;
		Laya.stage.screenMode = Stage.SCREEN_NONE;
		Stat.show();

		this.scene = (<Scene3D>Laya.stage.addChild(new Scene3D()));

		var camera: Camera = (<Camera>(this.scene.addChild(new Camera(0, 0.1, 100))));
		camera.transform.translate(new Vector3(0, 0.6, 1.1));
		camera.transform.rotate(new Vector3(-30, 0, 0), true, false);

		var directionLight: DirectionLight = (<DirectionLight>this.scene.addChild(new DirectionLight()));
		//设置平行光的方向
		var mat: Matrix4x4 = directionLight.transform.worldMatrix;
		mat.setForward(new Vector3(0.0, -0.8, -1.0));
		directionLight.transform.worldMatrix = mat;
		directionLight.color.setValue(1, 1, 1);

		Laya.loader.create("res/threeDimen/staticModel/lizard/lizard.lh", Handler.create(this, this.onComplete), null, Laya3D.HIERARCHY);
	}

	onComplete(s: any): void {
		Sprite3D.load("res/threeDimen/staticModel/lizard/lizard.lh", Handler.create(this, function (sprite: Sprite3D): void {
			var monster1: Sprite3D = (<Sprite3D>this.scene.addChild(sprite));
			monster1.transform.position = new Vector3(-0.6, 0, 0);
			monster1.transform.localScale = new Vector3(0.075, 0.075, 0.075);
			var monster2: Sprite3D = Sprite3D.instantiate(monster1, this.scene, false, new Vector3(0.6, 0, 0));
			monster2.transform.localScale = new Vector3(0.075, 0.075, 0.075);
			for (var i: number = 0; i < monster2.getChildByName("lizard").numChildren; i++) {
				var meshSprite3D: MeshSprite3D = (<MeshSprite3D>monster2.getChildByName("lizard").getChildAt(i));
				var material: BlinnPhongMaterial = (<BlinnPhongMaterial>meshSprite3D.meshRenderer.material);
				//法线贴图
				Texture2D.load(this.normalMapUrl[i], Handler.create(this, function (mat: BlinnPhongMaterial, texture: Texture2D): void {
					mat.normalTexture = texture;
				}, [material]));
			}

			Laya.timer.frameLoop(1, this, function (): void {
				monster1.transform.rotate(this.rotation);
				monster2.transform.rotate(this.rotation);
			});
		}));

	}

}

