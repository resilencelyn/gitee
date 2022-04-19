import { Laya3D } from "Laya3D"
import { Laya } from "Laya";
import { Camera } from "laya/d3/core/Camera"
	import { Sprite3D } from "laya/d3/core/Sprite3D"
	import { DirectionLight } from "laya/d3/core/light/DirectionLight"
	import { Scene3D } from "laya/d3/core/scene/Scene3D"
	import { Vector3 } from "laya/d3/math/Vector3"
	import { Stage } from "laya/display/Stage"
	import { Handler } from "laya/utils/Handler"
	import { Stat } from "laya/utils/Stat"
	/**
	 * ...
	 * @author sss
	 */
	export class TestAbc 
	{
		
		constructor(){
			//初始化引擎
			Laya3D.init(720, 1280);
			
			//适配模式
			Laya.stage.scaleMode = Stage.SCALE_FULL;
			Laya.stage.screenMode = Stage.SCREEN_NONE;
			
			//开启统计信息
			Stat.show();
			
			//预加载所有资源
			var resource:any[] = ["res/threeDimen/skinModel/LayaMonkey/LayaMonkey.lh"];
			
			Laya.loader.create(resource, Handler.create(this, this.onLoadFinish));
		}
		
		 onLoadFinish():void 
		{
			//添加3D场景
			var scene:Scene3D = (<Scene3D>Laya.stage.addChild(new Scene3D()) );
			
			//添加照相机
			var camera:Camera = (<Camera>(scene.addChild(new Camera(0, 0.1, 100))) );
			camera.transform.translate(new Vector3(0, 3, 3));
			camera.transform.rotate(new Vector3(-30, 0, 0), true, false);
			
			//添加方向光
			var directionLight:DirectionLight = (<DirectionLight>scene.addChild(new DirectionLight()) );
			directionLight.color = new Vector3(0.6, 0.6, 0.6);
			directionLight.transform.worldMatrix.setForward(new Vector3(1, -1, 0));
			
			var sp:Sprite3D = Laya.loader.getRes("res/threeDimen/skinModel/LayaMonkey/LayaMonkey.lh");
			scene.addChild(sp);
		}
		
	}


