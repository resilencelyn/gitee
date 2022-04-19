import { Config3D } from "Config3D";
import { Camera } from "laya/d3/core/Camera"
	import { Sprite3D } from "laya/d3/core/Sprite3D"
	import { DirectionLight } from "laya/d3/core/light/DirectionLight"
	import { Scene3D } from "laya/d3/core/scene/Scene3D"
	import { Vector3 } from "laya/d3/math/Vector3"
	import { Stage } from "laya/display/Stage"
	import { Handler } from "laya/utils/Handler"
import { Laya3D } from "Laya3D";
import { Laya } from "Laya";
	
	export class Laya3DCombineHtml
	{
		constructor(){
			var div = document.createElement('div');
			div.innerHTML = '<h1 style=\'color: red;\'>此内容来源于HTML网页, 可直接在html代码中书写 - h1标签</h1>';
			document.body.appendChild(div);
			
			//1.开启第四个参数
			var config3D:Config3D = new Config3D();
			config3D.isAlpha = true;
			Laya3D.init(0, 0, config3D);
			Laya.stage.scaleMode = Stage.SCALE_FULL;
			Laya.stage.screenMode = Stage.SCREEN_NONE;
			//2.设置舞台背景色为空
			Laya.stage.bgColor = null;
			
			var scene:Scene3D = (<Scene3D>Laya.stage.addChild(new Scene3D()) );
			
			var camera:Camera = (<Camera>scene.addChild(new Camera(0, 0.1, 100)) );
			camera.transform.translate(new Vector3(0, 0.5, 1));
			camera.transform.rotate(new Vector3( -15, 0, 0), true, false);
			//3.清除照相机颜色
			camera.clearColor = null;
			
			var directionLight:DirectionLight = (<DirectionLight>scene.addChild(new DirectionLight()) );
			directionLight.color = new Vector3(1, 1, 1);
			
			Sprite3D.load("res/threeDimen/skinModel/LayaMonkey/LayaMonkey.lh",Handler.create(this,function(layaMonkey:Sprite3D):void{
				scene.addChild(layaMonkey);
			}));
		}
	}

