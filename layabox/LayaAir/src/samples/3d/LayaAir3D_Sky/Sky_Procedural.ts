import { Laya } from "Laya";
import { BaseCamera } from "laya/d3/core/BaseCamera";
import { Camera, CameraClearFlags } from "laya/d3/core/Camera";
import { DirectionLight } from "laya/d3/core/light/DirectionLight";
import { SkyProceduralMaterial } from "laya/d3/core/material/SkyProceduralMaterial";
import { Scene3D } from "laya/d3/core/scene/Scene3D";
import { Matrix4x4 } from "laya/d3/math/Matrix4x4";
import { Vector3 } from "laya/d3/math/Vector3";
import { SkyDome } from "laya/d3/resource/models/SkyDome";
import { SkyRenderer } from "laya/d3/resource/models/SkyRenderer";
import { Shader3D } from "laya/d3/shader/Shader3D";
import { Stage } from "laya/display/Stage";
import { Stat } from "laya/utils/Stat";
import { Laya3D } from "Laya3D";
import { CameraMoveScript } from "../common/CameraMoveScript";

export class Sky_Procedural {

	constructor() {
		Laya3D.init(0, 0);
		Laya.stage.scaleMode = Stage.SCALE_FULL;
		Laya.stage.screenMode = Stage.SCREEN_NONE;
		Stat.show();

		//初始化3D场景
		var scene: Scene3D = (<Scene3D>Laya.stage.addChild(new Scene3D()));

		//初始化天空渲染器
		var skyRenderer: SkyRenderer = scene.skyRenderer;
		//创建天空盒mesh
		skyRenderer.mesh = SkyDome.instance;
		//使用程序化天空盒
		skyRenderer.material = new SkyProceduralMaterial();

		//初始化相机并设置清除标记为天空
		var camera: Camera = (<Camera>scene.addChild(new Camera(0, 0.1, 100)));
		camera.addComponent(CameraMoveScript);
		//设置相机的清除标识为天空盒(这个参数必须设置为CLEARFLAG_SKY，否则无法使用天空盒)
		camera.clearFlag = CameraClearFlags.Sky;

		//初始化平行光
		var directionLight: DirectionLight = (<DirectionLight>scene.addChild(new DirectionLight()));
		//设置平行光的方向
		var mat: Matrix4x4 = directionLight.transform.worldMatrix;
		mat.setForward(new Vector3(-1.0, -1.0, -1.0));
		directionLight.transform.worldMatrix = mat;
		var rotation: Vector3 = new Vector3(-0.01, 0, 0);

		//旋转平行光,模拟太阳轨迹
		Laya.timer.frameLoop(1, this, function (): void {
			directionLight.transform.rotate(rotation);
		});

	}
}

