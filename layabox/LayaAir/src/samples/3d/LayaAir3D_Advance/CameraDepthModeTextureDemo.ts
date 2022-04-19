import { Laya } from "Laya";
import { Camera } from "laya/d3/core/Camera";
import { MeshSprite3D } from "laya/d3/core/MeshSprite3D";
import { Scene3D } from "laya/d3/core/scene/Scene3D";
import { DepthTextureMode } from "laya/d3/depthMap/DepthPass";
import { Vector3 } from "laya/d3/math/Vector3";
import { TextureCube } from "laya/d3/resource/TextureCube";
import { Shader3D } from "laya/d3/shader/Shader3D";
import { Stage } from "laya/display/Stage";
import { Loader } from "laya/net/Loader";
import { Handler } from "laya/utils/Handler";
import { Stat } from "laya/utils/Stat";
import { Laya3D } from "Laya3D";
import { DepthMaterial } from "./DepthNormalShader/DepthMaterial";
import { DepthNormalsMaterial } from "./DepthNormalShader/DepthNormalsMaterial";

/**
 * 示例用来展示获得的深度、深度法线贴图
 * @author miner 
 */
export class CameraDepthModeTextureDemo{
    private scene:Scene3D;
    private depthPlane:MeshSprite3D;
    private depthNormalPlane:MeshSprite3D;
    constructor(){
        //初始化引擎
		Laya3D.init(0, 0);
		Laya.stage.scaleMode = Stage.SCALE_FULL;
		Laya.stage.screenMode = Stage.SCREEN_NONE;
		//显示性能面板
		Stat.show();
        Shader3D.debugMode = true;
        DepthMaterial.init();
        DepthNormalsMaterial.init();
        this.PreloadingRes();
    }

    //批量预加载方式
	PreloadingRes() {
		//预加载所有资源
        var resource: any[] = ["res/threeDimen/LayaScene_depthNormalScene/Conventional/depthNormalPlane.lh",
        "res/threeDimen/LayaScene_depthNormalScene/Conventional/depthPlane.lh",
        "res/threeDimen/LayaScene_depthNormalScene/Conventional/depthscene.lh",
        "res/threeDimen/LayaScene_depthNormalScene/Conventional/Main Camera.lh",
        "res/threeDimen/LayaScene_depthNormalScene/Conventional/Assets/Scenes/depthNormalSceneGIReflection.ltcb.ls"
		];
		Laya.loader.create(resource, Handler.create(this, this.onPreLoadFinish));
    }
    
    onPreLoadFinish(){
        this.scene = Laya.stage.addChild(new Scene3D()) as Scene3D;
        this.scene.ambientColor = new Vector3(0.858,0.858,0.858);
        this.scene.reflection = Loader.getRes("res/threeDimen/LayaScene_depthNormalScene/Conventional/Assets/Scenes/depthNormalSceneGIReflection.ltcb.ls") as TextureCube;
        this.scene.reflectionDecodingFormat  = 1;
        this.scene.reflectionIntensity = 1;
        this.depthNormalPlane =this.scene.addChild(Loader.getRes("res/threeDimen/LayaScene_depthNormalScene/Conventional/depthNormalPlane.lh"))as MeshSprite3D;
        this.depthPlane = this.scene.addChild(Loader.getRes("res/threeDimen/LayaScene_depthNormalScene/Conventional/depthPlane.lh")) as MeshSprite3D;
        this.scene.addChild(Loader.getRes("res/threeDimen/LayaScene_depthNormalScene/Conventional/depthscene.lh")) ;
        var camera = this.scene.addChild(Loader.getRes("res/threeDimen/LayaScene_depthNormalScene/Conventional/Main Camera.lh")) as Camera;
        camera.depthTextureMode|=DepthTextureMode.Depth;
        this.depthPlane.meshRenderer.sharedMaterial = new DepthMaterial();

        camera.depthTextureMode|=DepthTextureMode.DepthNormals;
        this.depthNormalPlane.meshRenderer.sharedMaterial = new DepthNormalsMaterial();
    }
}