import { Laya } from "Laya";
import { Camera, CameraEventFlags } from "laya/d3/core/Camera";
import { Scene3D } from "laya/d3/core/scene/Scene3D";
import { Vector4 } from "laya/d3/math/Vector4";
import { Stage } from "laya/display/Stage";
import { Handler } from "laya/utils/Handler";
import { Stat } from "laya/utils/Stat";
import { Laya3D } from "Laya3D";
import { Shader3D } from "laya/d3/shader/Shader3D";
import { CommandBuffer } from "laya/d3/core/render/command/CommandBuffer";
import { RenderTexture } from "laya/d3/resource/RenderTexture";
import { Viewport } from "laya/d3/math/Viewport";
import { RenderTextureDepthFormat, RenderTextureFormat } from "laya/resource/RenderTextureFormat";
import { ShaderData } from "laya/d3/shader/ShaderData";
import { FilterMode } from "laya/resource/FilterMode";
import { MeshSprite3D } from "laya/d3/core/MeshSprite3D";
import { PBRStandardMaterial } from "laya/d3/core/material/PBRStandardMaterial";

import { BlurEffect } from "../LayaAir3D_PostProcess/BlurShader/BlurEffect";
import { CameraMoveScript } from "../common/CameraMoveScript";
import { GlassWithoutGrabMaterial } from "./CommandBufferDemo/GlassWithoutGrabMaterial";

export class CommandBuffer_BlurryGlass {
	mat:GlassWithoutGrabMaterial;
	texture:RenderTexture;
	constructor() {
		//初始化引擎
		Laya3D.init(100, 100);
		Stat.show();
		Shader3D.debugMode = true;
		Laya.stage.scaleMode = Stage.SCALE_FULL;
		Laya.stage.screenMode = Stage.SCREEN_NONE;
		
		//材质初始化
		BlurEffect.init();
		GlassWithoutGrabMaterial.init();

		//加载场景
		Scene3D.load("res/threeDimen/BlurryRefraction/Conventional/BlurryGlass.ls", Handler.create(this, function (scene: Scene3D): void {
			(<Scene3D>Laya.stage.addChild(scene));
			//获取场景中的相机
			var camera: Camera = (<Camera>scene.getChildByName("Main Camera"));
			//增加移动脚本
			camera.addComponent(CameraMoveScript);
			
			var glass01:MeshSprite3D = scene.getChildByName("glass01") as MeshSprite3D;
			var glass02:MeshSprite3D = scene.getChildByName("glass02") as MeshSprite3D; 
			//在这里切换了材质
			var pbrStandard:PBRStandardMaterial = glass01.meshRenderer.sharedMaterial as PBRStandardMaterial;
			//将图片设置到玻璃材质
			var glassMaterial = new GlassWithoutGrabMaterial(pbrStandard.albedoTexture);
			//给模型赋毛玻璃材质
			glass01.meshRenderer.sharedMaterial = glassMaterial;
			glass02.meshRenderer.sharedMaterial = glassMaterial;
			this.mat = glassMaterial;
			//创建使用CommandBuffer
			this.createCommandBuffer(camera);
		}));
	}

	/**
	 * 创建CommandBuffer命令缓存流
	 * @param camera 
	 */
	createCommandBuffer(camera:Camera){
		//当需要在流程中拿摄像机渲染效果的时候 设置true
		camera.enableBuiltInRenderTexture = true;
		//创建渲染命令流
		var buf:CommandBuffer = new CommandBuffer();
		//创建需要模糊使用的屏幕RenderTexture
		var viewPort:Viewport = camera.viewport;
		var renderTexture = RenderTexture.createFromPool(viewPort.width,viewPort.height,RenderTextureFormat.R8G8B8,RenderTextureDepthFormat.DEPTHSTENCIL_NONE);
		//将当前渲染的结果拷贝到创建好的RenderTexture
		this.texture = renderTexture; 
		buf.blitScreenTriangle(null,renderTexture);
		//获得shader
		var shader:Shader3D = Shader3D.find("blurEffect");
		var shaderValue:ShaderData = new ShaderData();
		//down Sample level设置降采样等级
		var downSampleFactor:number = 4;
		var downSampleWidth:number = viewPort.width/downSampleFactor;
		var downSampleheigh:number = viewPort.height/downSampleFactor;
		//设置模糊材质参数
		var texSize:Vector4 = new Vector4(1.0/viewPort.width,1.0/viewPort.height,viewPort.width,downSampleheigh);//材质所在坐标位置
		shaderValue.setNumber(BlurEffect.SHADERVALUE_DOWNSAMPLEVALUE,1);
		shaderValue.setVector(BlurEffect.SHADERVALUE_TEXELSIZE,texSize);
		//创建降采样RenderTexture1
		var downRenderTexture = RenderTexture.createFromPool(downSampleWidth,downSampleheigh,RenderTextureFormat.R8G8B8,RenderTextureDepthFormat.DEPTHSTENCIL_NONE);
		//降采样命令流
		buf.blitScreenTriangle(renderTexture,downRenderTexture,null,shader,shaderValue,0);
		//创建降采样RenderTexture2
		var blurTexture:RenderTexture = RenderTexture.createFromPool(downSampleWidth,downSampleheigh,RenderTextureFormat.R8G8B8,RenderTextureDepthFormat.DEPTHSTENCIL_NONE);
		blurTexture.filterMode = FilterMode.Bilinear;
		//Horizontal blur
		buf.blitScreenTriangle(downRenderTexture,blurTexture,null,shader,shaderValue,1);
		//vertical blur
		buf.blitScreenTriangle(blurTexture,downRenderTexture,null,shader,shaderValue,2);
		//Horizontal blur
		buf.blitScreenTriangle(downRenderTexture,blurTexture,null,shader,shaderValue,1);
		//vertical blur
		buf.blitScreenTriangle(blurTexture,downRenderTexture,null,shader,shaderValue,2);
		
		//设置全局uniform变量  
		var globalUniformNameID:number = Shader3D.propertyNameToID("u_screenTexture");
		buf.setGlobalTexture(globalUniformNameID,downRenderTexture);
		//将commandBuffer加入渲染流程
		camera.addCommandBuffer(CameraEventFlags.BeforeTransparent,buf);
		//回收用过的RenderTexture
		RenderTexture.recoverToPool(downRenderTexture);
		RenderTexture.recoverToPool(blurTexture);
		return;
	}

}

