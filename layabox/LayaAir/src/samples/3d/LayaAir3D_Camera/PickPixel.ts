import { Laya } from "Laya";
import { BaseCamera } from "laya/d3/core/BaseCamera";
import { Camera, CameraClearFlags } from "laya/d3/core/Camera";
import { Scene3D } from "laya/d3/core/scene/Scene3D";
import { Ray } from "laya/d3/math/Ray";
import { Vector3 } from "laya/d3/math/Vector3";
import { RenderTexture } from "laya/d3/resource/RenderTexture";
import { Stage } from "laya/display/Stage";
import { Text } from "laya/display/Text";
import { Event } from "laya/events/Event";
import { MouseManager } from "laya/events/MouseManager";
import { Loader } from "laya/net/Loader";
import { Button } from "laya/ui/Button";
import { Browser } from "laya/utils/Browser";
import { Handler } from "laya/utils/Handler";
import { Stat } from "laya/utils/Stat";
import { Laya3D } from "Laya3D";
import { CameraMoveScript } from "../common/CameraMoveScript";
import { Config } from "Config";
import { Sprite } from "laya/display/Sprite";
import { Utils } from "laya/utils/Utils";
import Client from "../../Client";

export class PickPixel {
	private isPick: boolean = false;
	private changeActionButton: Button;
	private ray: Ray;
	private text: Text = new Text();
	private renderTargetCamera: Camera;
	private _sp:Sprite;

	/**实例类型*/
	private btype:any = "PickPixel";
	/**场景内按钮类型*/
	private stype:any = 0;
	constructor() {
		//初始化引擎
		Config.useRetinalCanvas = true;
		Laya3D.init(750, 1334);
		Laya.stage.scaleMode = Stage.SCALE_FULL;
		Laya.stage.screenMode = Stage.SCREEN_NONE;
		//显示性能面板
		Stat.show();
		//射线初始化（必须初始化）
		this.ray = new Ray(new Vector3(0, 0, 0), new Vector3(0, 0, 0));
		this._sp = new Sprite();
		Laya.stage.addChild(this._sp);
		this._sp.zOrder = 10;
		//预加载资源
		Laya.loader.create(["res/threeDimen/scene/CourtyardScene/Courtyard.ls", "res/threeDimen/texture/earth.png"], Handler.create(this, this.onComplete));
	}

	private onMouseDown(): void {
		this._sp.graphics.clear();
		this._sp.x = Laya.stage.mouseX;
		this._sp.y = Laya.stage.mouseY;
		var posX: number = MouseManager.instance.mouseX / Laya.stage.clientScaleX; 
		var posY: number = MouseManager.instance.mouseY / Laya.stage.clientScaleY;
		var out: Uint8Array = new Uint8Array(4);
		this.renderTargetCamera.renderTarget.getData(posX, posY, 1, 1, out);
		this.text.text = out[0] + " " + out[1] + " " + out[2] + " " + out[3];
		let r = out[0].toString(16);
		let g = out[1].toString(16);
		let b = out[2].toString(16);
		if (r.length<2) {
			r = 0 + r;
		}
		if (g.length<2) {
			g = 0 + g;
		}
		if (b.length<2) {
			b = 0 + b;
		}
		let color = `#${r}${g}${b}`
		console.log(color)
		this._sp.alpha = out[3]/255;
		this._sp.graphics.drawRect(0,0,100,100,color,"#ffffff");
		Client.instance.send({type:"next",btype:this.btype,stype:1})
	}

	private onResize(): void {
		var stageHeight: number = Laya.stage.height;
		var stageWidth: number = Laya.stage.width;
		this.renderTargetCamera.renderTarget.destroy();
		this.renderTargetCamera.renderTarget = new RenderTexture(stageWidth, stageHeight);
		this.text.x = Laya.stage.width / 2;
		this.changeActionButton.pos(Laya.stage.width / 2 - this.changeActionButton.width * Browser.pixelRatio / 2, Laya.stage.height - 100 * Browser.pixelRatio);
	}
	private _thisscene:Scene3D ;
	private onComplete(): void {
		this._thisscene = (<Scene3D>Laya.stage.addChild(Loader.getRes("res/threeDimen/scene/CourtyardScene/Courtyard.ls")));
		//加载场景
		var scene: Scene3D = this._thisscene; 
		//添加相机
		var camera: Camera = (<Camera>scene.addChild(new Camera(0, 0.1, 1000)));
		camera.transform.translate(new Vector3(57, 2.5, 58));
		camera.transform.rotate(new Vector3(-10, 150, 0), true, false);
		//设置相机清除标识
		camera.clearFlag = CameraClearFlags.Sky;
		//相机添加视角控制组件(脚本)
		camera.addComponent(CameraMoveScript);

		//渲染到纹理的相机
		this.renderTargetCamera = (<Camera>scene.addChild(new Camera(0, 0.1, 1000)));
		this.renderTargetCamera.transform.translate(new Vector3(57, 2.5, 58));
		this.renderTargetCamera.transform.rotate(new Vector3(-10, 150, 0), true, false);
		//选择渲染目标为纹理
		var stageHeight: number = Laya.stage.height;
		var stageWidth: number = Laya.stage.width;
		this.renderTargetCamera.renderTarget = new RenderTexture(stageWidth, stageHeight);
		//渲染顺序
		this.renderTargetCamera.renderingOrder = -1;
		//相机添加视角控制组件(脚本)
		this.renderTargetCamera.addComponent(CameraMoveScript);


		Laya.loader.load(["res/threeDimen/ui/button.png"], Handler.create(this, function (): void {
			this.changeActionButton = (<Button>Laya.stage.addChild(new Button("res/threeDimen/ui/button.png", "拾取像素")));
			this.changeActionButton.size(160, 40);
			this.changeActionButton.labelBold = true;
			this.changeActionButton.labelSize = 30;
			this.changeActionButton.sizeGrid = "4,4,4,4";
			this.changeActionButton.scale(Browser.pixelRatio, Browser.pixelRatio);
			this.changeActionButton.pos(Laya.stage.width / 2 - this.changeActionButton.width * Browser.pixelRatio / 2, Laya.stage.height - 100 * Browser.pixelRatio);
			this.changeActionButton.on(Event.CLICK, this, this.stypeFun0);
		}));
		this.text.x = Laya.stage.width / 2 - 50;
		this.text.y = 50;
		this.text.overflow = Text.HIDDEN;
		this.text.color = "#FFFFFF";
		this.text.font = "Impact";
		this.text.fontSize = 20;
		this.text.borderColor = "#FFFF00";
		this.text.x = Laya.stage.width / 2;
		this.text.text = "选中的颜色：";
		Laya.stage.addChild(this.text);
		//添加舞台RESIZE事件
		Laya.stage.on(Event.RESIZE, this, this.onResize);
		
	}

	stypeFun0(label:string = "拾取像素"): void {
		if (this.isPick) {
			Laya.stage.off(Event.MOUSE_DOWN, this, this.onMouseDown);
			this.changeActionButton.label = "拾取像素";
			this.isPick = false;
		}
		else {
			//鼠标事件监听
			Laya.stage.on(Event.MOUSE_DOWN, this, this.onMouseDown);
			this.changeActionButton.label = "结束拾取";
			this.isPick = true;
		}
		
		label = this.changeActionButton.label;
		Client.instance.send({type:"next",btype:this.btype,stype:0,value:label})
	}
}

