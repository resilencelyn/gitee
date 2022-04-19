import { Laya } from "Laya";
import { Camera, CameraEventFlags } from "laya/d3/core/Camera";
import { CommandBuffer } from "laya/d3/core/render/command/CommandBuffer";
import { Scene3D } from "laya/d3/core/scene/Scene3D";
import { Vector4 } from "laya/d3/math/Vector4";
import { Stage } from "laya/display/Stage";
import { FilterMode } from "laya/resource/FilterMode";
import { RenderTextureDepthFormat, RenderTextureFormat } from "laya/resource/RenderTextureFormat";
import { Handler } from "laya/utils/Handler";
import { Stat } from "laya/utils/Stat";
import { Laya3D } from "Laya3D";
import { UnlitMaterial } from "laya/d3/core/material/UnlitMaterial";
import { Viewport } from "laya/d3/math/Viewport";
import { RenderTexture } from "laya/d3/resource/RenderTexture";
import { Shader3D } from "laya/d3/shader/Shader3D";
import { Material } from "laya/d3/core/material/Material";
import { MeshSprite3D } from "laya/d3/core/MeshSprite3D";
import { ShuriKenParticle3D } from "laya/d3/core/particleShuriKen/ShuriKenParticle3D";
import { ShurikenParticleMaterial } from "laya/d3/core/particleShuriKen/ShurikenParticleMaterial";
import { SkinnedMeshSprite3D } from "laya/d3/core/SkinnedMeshSprite3D";
import { BlurEffect, BlurMaterial } from "../LayaAir3D_PostProcess/BlurShader/BlurEffect";
import { BaseRender } from "laya/d3/core/render/BaseRender";
import { CameraMoveScript } from "../common/CameraMoveScript";
import { Button } from "laya/ui/Button";
import { Browser } from "laya/utils/Browser";
import { Event } from "laya/events/Event";
import { Utils } from "laya/utils/Utils";
import Client from "../../Client";

export class CommandBuffer_Outline {
	private commandBuffer:CommandBuffer;
	private cameraEventFlag:CameraEventFlags = CameraEventFlags.BeforeImageEffect;
	private camera:Camera;
	private enableCommandBuffer:boolean = false;
	private button:Button;

	/**实例类型*/
	private btype:any = "CommandBuffer_Outline";
	/**场景内按钮类型*/
	private stype:any = 0;
	constructor() {
		//初始化引擎
		Laya3D.init(0, 0);
		Stat.show();
		Laya.stage.scaleMode = Stage.SCALE_FULL;
		Laya.stage.screenMode = Stage.SCREEN_NONE;
		BlurEffect.init();
		var unlitMaterial = new UnlitMaterial();
		unlitMaterial.albedoColor = new Vector4(255,0,0,255);
		var shurikenMaterial:ShurikenParticleMaterial = new ShurikenParticleMaterial();
		shurikenMaterial.color = new Vector4(255,0,0,255);

		Shader3D.debugMode = true;
		//加载场景
		Scene3D.load("res/threeDimen/OutlineEdgeScene/Conventional/OutlineEdgeScene.ls", Handler.create(this, function (scene: Scene3D): void {
			(<Scene3D>Laya.stage.addChild(scene));
			//获取场景中的相机
			this.camera = (<Camera>scene.getChildByName("Main Camera"));

			// //加入摄像机移动控制脚本
			this.camera.addComponent(CameraMoveScript);
			
			var renders:BaseRender[]  = [];
			var materials:Material[] = [];
			renders.push((scene.getChildByName("Cube") as MeshSprite3D).meshRenderer);
			materials.push(unlitMaterial);
            renders.push((scene.getChildByName("Particle") as ShuriKenParticle3D).particleRenderer);
			materials.push(shurikenMaterial);
            renders.push((scene.getChildByName("LayaMonkey").getChildByName("LayaMonkey") as SkinnedMeshSprite3D).skinnedMeshRenderer);
			materials.push(unlitMaterial);
		
			//创建commandBuffer
			this.commandBuffer =  this.createDrawMeshCommandBuffer(this.camera,renders,materials);
			//将commandBuffer加入渲染流程
			this.camera.addCommandBuffer(this.cameraEventFlag,this.commandBuffer);


			
			//加载UI
			this.loadUI();
		}));
	}

	createDrawMeshCommandBuffer(camera:Camera,renders:BaseRender[],materials:Material[]):CommandBuffer{
		var buf:CommandBuffer = new CommandBuffer();
		//当需要在流程中拿摄像机渲染效果的时候 设置true
		camera.enableBuiltInRenderTexture = true;
		//创建和屏幕一样大的Rendertexture
		var viewPort:Viewport = camera.viewport;
		var renderTexture = RenderTexture.createFromPool(viewPort.width,viewPort.height,RenderTextureFormat.R8G8B8A8,RenderTextureDepthFormat.DEPTHSTENCIL_NONE);
		//将RenderTexture设置为渲染目标
		buf.setRenderTarget(renderTexture);
		//清楚渲染目标的颜色为黑色，不清理深度
		buf.clearRenderTarget(true,false,new Vector4(0,0,0,0));
		
		//将传入的Render渲染到纹理上
		for(var i = 0,n = renders.length;i<n;i++){
			buf.drawRender(renders[i],materials[i],0);
		}
		//创建新的RenderTexture
		 var subRendertexture = RenderTexture.createFromPool(viewPort.width,viewPort.height,RenderTextureFormat.R8G8B8A8,RenderTextureDepthFormat.DEPTHSTENCIL_NONE);
		//将renderTexture的结果复制到subRenderTexture
		 buf.blitScreenQuad(renderTexture,subRendertexture);
		//设置模糊的参数
		 var downSampleFactor:number = 2;
		 var downSampleWidth:number = viewPort.width/downSampleFactor;
		 var downSampleheigh:number = viewPort.height/downSampleFactor;
		var texSize:Vector4 = new Vector4(1.0/viewPort.width,1.0/viewPort.height,viewPort.width,downSampleheigh);
		//创建模糊材质
		var blurMaterial:BlurMaterial = new BlurMaterial(texSize,1);
		
		//创建降采样RenderTexture1
		 var downRenderTexture = RenderTexture.createFromPool(downSampleWidth,downSampleheigh,RenderTextureFormat.R8G8B8,RenderTextureDepthFormat.DEPTHSTENCIL_NONE);
		//降采样  使用blurMaterial材质的0SubShader将Rendertexture渲染到DownRendertexture
		 buf.blitScreenQuadByMaterial(renderTexture,downRenderTexture,null,blurMaterial,0);

		 //创建降采样RenderTexture2
		var blurTexture:RenderTexture = RenderTexture.createFromPool(downSampleWidth,downSampleheigh,RenderTextureFormat.R8G8B8,RenderTextureDepthFormat.DEPTHSTENCIL_NONE);
		blurTexture.filterMode = FilterMode.Bilinear;

		//Horizontal blur 使用blurMaterial材质的1SubShader
		buf.blitScreenQuadByMaterial(downRenderTexture,blurTexture,null,blurMaterial,1);
		//vertical blur	使用blurMaterial材质的2SubShader
		buf.blitScreenQuadByMaterial(blurTexture,downRenderTexture,null,blurMaterial,2);
		//Horizontal blur 使用blurMaterial材质的1SubShader
		buf.blitScreenQuadByMaterial(downRenderTexture,blurTexture,null,blurMaterial,1);
		//vertical blur   使用blurMaterial材质的2SubShader
		buf.blitScreenQuadByMaterial(blurTexture,downRenderTexture,null,blurMaterial,2);
		//在命令流里面插入设置图片命令流，在调用的时候会设置blurMaterial的图片数据
		buf.setShaderDataTexture(blurMaterial._shaderValues,BlurMaterial.SHADERVALUE_SOURCETEXTURE0,downRenderTexture);
		buf.setShaderDataTexture(blurMaterial._shaderValues,BlurMaterial.ShADERVALUE_SOURCETEXTURE1,subRendertexture);
		//caculate edge计算边缘图片
		buf.blitScreenQuadByMaterial(blurTexture,renderTexture,null,blurMaterial,3);
		//重新传入图片
		buf.setShaderDataTexture(blurMaterial._shaderValues,BlurMaterial.SHADERVALUE_SOURCETEXTURE0,renderTexture);
		//将camera渲染结果复制到subRendertexture，使用blurMaterial的4通道shader
		buf.blitScreenQuadByMaterial(null,subRendertexture,null,blurMaterial,4);
		//将subRenderTexture重新赋值到camera的渲染结果上面
		buf.blitScreenQuadByMaterial(subRendertexture,null);
		return buf;
	}

		/**
	 *@private
	 */
	loadUI(): void {
		Laya.loader.load(["res/threeDimen/ui/button.png"], Handler.create(this, function (): void {
			this.button = Laya.stage.addChild(new Button("res/threeDimen/ui/button.png", "关闭描边"));
			this.button.size(200, 40);
			this.button.labelBold = true;
			this.button.labelSize = 30;
			this.button.sizeGrid = "4,4,4,4";
			this.button.scale(Browser.pixelRatio, Browser.pixelRatio);
			this.button.pos(Laya.stage.width / 2 - this.button.width * Browser.pixelRatio / 2, Laya.stage.height - 60 * Browser.pixelRatio);
			this.button.on(Event.CLICK, this, this.stypeFun0);

		}));
	}

	stypeFun0(label:string = "关闭描边"): void {
		this.enableCommandBuffer = !this.enableCommandBuffer;
		if (this.enableCommandBuffer)
		{
			this.button.label = "开启描边";
			this.camera.removeCommandBuffer(this.cameraEventFlag,this.commandBuffer);

		}
		else{
			this.button.label = "关闭描边";
			this.camera.addCommandBuffer(this.cameraEventFlag,this.commandBuffer);

		}
		
		label = this.button.label;
		Client.instance.send({type:"next",btype:this.btype,stype:0,value:label});		
	}
}

