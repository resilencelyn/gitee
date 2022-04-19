import { Laya } from "Laya";
import { Camera } from "laya/d3/core/Camera";
import { DirectionLight } from "laya/d3/core/light/DirectionLight";
import { BaseMaterial } from "laya/d3/core/material/BaseMaterial";
import { MeshSprite3D } from "laya/d3/core/MeshSprite3D";
import { Scene3D } from "laya/d3/core/scene/Scene3D";
import { SkinnedMeshSprite3D } from "laya/d3/core/SkinnedMeshSprite3D";
import { Sprite3D } from "laya/d3/core/Sprite3D";
import { VertexMesh } from "laya/d3/graphics/Vertex/VertexMesh";
import { Vector3 } from "laya/d3/math/Vector3";
import { PrimitiveMesh } from "laya/d3/resource/models/PrimitiveMesh";
import { Shader3D } from "laya/d3/shader/Shader3D";
import { ShaderPass } from "laya/d3/shader/ShaderPass";
import { SubShader } from "laya/d3/shader/SubShader";
import { Stage } from "laya/display/Stage";
import { Texture2D } from "laya/resource/Texture2D";
import { Handler } from "laya/utils/Handler";
import { Stat } from "laya/utils/Stat";
import { Laya3D } from "Laya3D";
import { CameraMoveScript } from "../common/CameraMoveScript";
import { CustomMaterial } from "./customMaterials/CustomMaterial";
import GlowingEdgeShaderFS from "./customShader/glowingEdgeShader.fs";
import GlowingEdgeShaderVS from "./customShader/glowingEdgeShader.vs";
import { GlowingEdgeMaterial } from "./customMaterials/GlowingEdgeMaterial";
import { Material } from "laya/d3/core/material/Material";



/**
 * ...
 * @author
 */
export class Shader_GlowingEdge {
	private rotation: Vector3 = new Vector3(0, 0.01, 0);

	constructor() {
		//初始化引擎
		Laya3D.init(0, 0);
		Laya.stage.scaleMode = Stage.SCALE_FULL;
		Laya.stage.screenMode = Stage.SCREEN_NONE;
		Stat.show();
		//初始化shader
		this.initShader();

		//创建场景
		var scene: Scene3D = (<Scene3D>Laya.stage.addChild(new Scene3D()));

		//创建相机
		var camera: Camera = (<Camera>(scene.addChild(new Camera(0, 0.1, 1000))));
		camera.transform.translate(new Vector3(0, 0.85, 1.7));
		camera.transform.rotate(new Vector3(-15, 0, 0), true, false);
		camera.addComponent(CameraMoveScript);

		//创建平行光
		var directionLight: DirectionLight = new DirectionLight();
		scene.addChild(directionLight);
		directionLight.color = new Vector3(1, 1, 1);

		//加载精灵
		Sprite3D.load("res/threeDimen/skinModel/dude/dude.lh", Handler.create(this, function (dude: Sprite3D): void {
			scene.addChild(dude);

			//使用自定义的材质
			var glowingEdgeMaterial1: GlowingEdgeMaterial = new GlowingEdgeMaterial();
			//加载纹理
			Texture2D.load("res/threeDimen/skinModel/dude/Assets/dude/head.png", Handler.create(this, function (tex: Texture2D): void {
				glowingEdgeMaterial1.diffuseTexture = tex;
			}));
			//设置边缘颜色
			glowingEdgeMaterial1.marginalColor = new Vector3(1, 0.7, 0);

			var glowingEdgeMaterial2: GlowingEdgeMaterial = new GlowingEdgeMaterial();
			Texture2D.load("res/threeDimen/skinModel/dude/Assets/dude/jacket.png", Handler.create(this, function (tex: Texture2D): void {
				glowingEdgeMaterial2.diffuseTexture = tex;
			}));
			glowingEdgeMaterial2.marginalColor = new Vector3(1, 0.7, 0);

			var glowingEdgeMaterial3: GlowingEdgeMaterial = new GlowingEdgeMaterial();
			Texture2D.load("res/threeDimen/skinModel/dude/Assets/dude/pants.png", Handler.create(this, function (tex: Texture2D): void {
				glowingEdgeMaterial3.diffuseTexture = tex;
			}));
			glowingEdgeMaterial3.marginalColor = new Vector3(1, 0.7, 0);

			var glowingEdgeMaterial4: GlowingEdgeMaterial = new GlowingEdgeMaterial();
			Texture2D.load("res/threeDimen/skinModel/dude/Assets/dude/upBodyC.png", Handler.create(this, function (tex: Texture2D): void {
				glowingEdgeMaterial4.diffuseTexture = tex;
			}))
			glowingEdgeMaterial4.marginalColor = new Vector3(1, 0.7, 0);

			var baseMaterials: Material[] = [];
			baseMaterials[0] = glowingEdgeMaterial1;
			baseMaterials[1] = glowingEdgeMaterial2;
			baseMaterials[2] = glowingEdgeMaterial3;
			baseMaterials[3] = glowingEdgeMaterial4;

			(<SkinnedMeshSprite3D>dude.getChildAt(0).getChildAt(0)).skinnedMeshRenderer.sharedMaterials = baseMaterials;
			dude.transform.position = new Vector3(0, 0.5, 0);
			dude.transform.setWorldLossyScale(new Vector3(0.2, 0.2, 0.2)); 
			dude.transform.rotate(new Vector3(0, 180, 0), false, false);
		}));

		//加载地球精灵
		var earth: MeshSprite3D = (<MeshSprite3D>scene.addChild(new MeshSprite3D(PrimitiveMesh.createSphere(0.5, 128, 128))));

		var glowingEdgeMaterial: GlowingEdgeMaterial = new GlowingEdgeMaterial();
		Texture2D.load("res/threeDimen/texture/earth.png", Handler.create(this, function (tex: Texture2D): void {
			glowingEdgeMaterial.diffuseTexture = tex;
		}));
		glowingEdgeMaterial.marginalColor = new Vector3(0.0, 0.3, 1.0);
		earth.meshRenderer.sharedMaterial = glowingEdgeMaterial;

		Laya.timer.frameLoop(1, this, function (): void {
			earth.transform.rotate(this.rotation, true);
		});
	}

	//初始化shader
	private initShader(): void {

		var attributeMap: any = { 
			'a_Position': VertexMesh.MESH_POSITION0, 
			'a_Normal': VertexMesh.MESH_NORMAL0, 
			'a_Texcoord': VertexMesh.MESH_TEXTURECOORDINATE0, 
			'a_BoneWeights': VertexMesh.MESH_BLENDWEIGHT0, 
			'a_BoneIndices': VertexMesh.MESH_BLENDINDICES0 
		};
		var uniformMap: any = { 
			'u_Bones': Shader3D.PERIOD_CUSTOM, 
			'u_CameraPos': Shader3D.PERIOD_CAMERA, 
			'u_MvpMatrix': Shader3D.PERIOD_SPRITE, 
			'u_WorldMat': Shader3D.PERIOD_SPRITE, 
			'u_texture': Shader3D.PERIOD_MATERIAL, 
			'u_marginalColor': Shader3D.PERIOD_MATERIAL, 
			'u_SunLight.color': Shader3D.PERIOD_SCENE,
		};
		//创建自定义shader
		var glowingEdgeShader: Shader3D = Shader3D.add("GlowingEdgeMaterial");
		//为当前自定义的shader添加SubShader
		var subShader: SubShader = new SubShader(attributeMap, uniformMap);
		glowingEdgeShader.addSubShader(subShader);
		//SubShader添加ShaderPass
		(<ShaderPass>subShader.addShaderPass(GlowingEdgeShaderVS, GlowingEdgeShaderFS));
	}
}

