import { Config3D } from "Config3D";
import { Laya } from "Laya";
import { Scene3D } from "laya/d3/core/scene/Scene3D";
import { Shader3D } from "laya/d3/shader/Shader3D";
import { Stage } from "laya/display/Stage";
import { Handler } from "laya/utils/Handler";
import { Stat } from "laya/utils/Stat";
import { Laya3D } from "Laya3D";
import { CameraMoveScript } from "../common/CameraMoveScript";

export class StaticBatchTest {
	constructor() {
		var config: Config3D = new Config3D();
		//config.debugFrustumCulling = true;
		//config.octreeCulling = true;
		Laya3D.init(0, 0, config);
		Stat.show();
		Laya.stage.scaleMode = Stage.SCALE_FULL;
		Laya.stage.screenMode = Stage.SCREEN_NONE;

		Scene3D.load("res/threeDimen/scene/LayaScene_city01/Conventional/city01.ls", Handler.create(null, function (scene: Scene3D): void {
			(<Scene3D>Laya.stage.addChild(scene));
			scene.getChildAt(0).addComponent(CameraMoveScript);
		}))

	}

}

