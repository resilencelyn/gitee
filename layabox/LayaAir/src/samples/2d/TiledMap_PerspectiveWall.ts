import { Laya } from "Laya";
import { Stage } from "laya/display/Stage";
import { TiledMap } from "laya/map/TiledMap";
import { Rectangle } from "laya/maths/Rectangle";
import { Resource } from "laya/resource/Resource";
import { Handler } from "laya/utils/Handler";
import { Main } from "./../Main";
export class TiledMap_PerspectiveWall {
	private tiledMap: TiledMap;
	Main: typeof Main = null;
	constructor(maincls: typeof Main) {
		this.Main = maincls;
		Laya.init(700, 500);
		Laya.stage.alignV = Stage.ALIGN_MIDDLE;
		Laya.stage.alignH = Stage.ALIGN_CENTER;

		Laya.stage.bgColor = "#232628";

		this.createMap();
	}

	private createMap(): void {
		this.tiledMap = new TiledMap();
		this.tiledMap.createMap("res/tiledMap/perspective_walls.json", new Rectangle(0, 0, Laya.stage.width, Laya.stage.height), Handler.create(this, this.onLoaded));
	}

	private onLoaded(): void {
		this.tiledMap.mapSprite().removeSelf();
		this.Main.box2D.addChild(this.tiledMap.mapSprite());
	}

	dispose(): void {
		if (this.tiledMap) {
			this.tiledMap.mapSprite().removeChildren();
			this.tiledMap.destroy();
			// Resource.destroyUnusedResources();
		}
	}
}

