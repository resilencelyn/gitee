import { Laya } from "Laya";
import { Script3D } from "laya/d3/component/Script3D";
import { BaseCamera } from "laya/d3/core/BaseCamera";
import { Camera } from "laya/d3/core/Camera";
import { Scene3D } from "laya/d3/core/scene/Scene3D";
import { Quaternion } from "laya/d3/math/Quaternion";
import { Vector3 } from "laya/d3/math/Vector3";
import { Event } from "laya/events/Event";
import { KeyBoardManager } from "laya/events/KeyBoardManager";

/**
 * ...
 * @author
 */
export class CameraMoveScript extends Script3D {

	/** @private */
	protected _tempVector3: Vector3 = new Vector3();
	protected lastMouseX: number;
	protected lastMouseY: number;
	protected yawPitchRoll: Vector3 = new Vector3();
	protected resultRotation: Quaternion = new Quaternion();
	protected tempRotationZ: Quaternion = new Quaternion();
	protected tempRotationX: Quaternion = new Quaternion();
	protected tempRotationY: Quaternion = new Quaternion();
	protected isMouseDown: boolean;
	protected rotaionSpeed: number = 0.00006;
	protected camera: BaseCamera;
	protected scene: Scene3D;

	constructor() {
		super();


	}

	/**
	 * @private
	 */
	protected _updateRotation(): void {
		if (Math.abs(this.yawPitchRoll.y) < 1.50) {
			Quaternion.createFromYawPitchRoll(this.yawPitchRoll.x, this.yawPitchRoll.y, this.yawPitchRoll.z, this.tempRotationZ);
			this.tempRotationZ.cloneTo(this.camera.transform.localRotation);
			this.camera.transform.localRotation = this.camera.transform.localRotation;
		}
	}

		/**
		 * @inheritDoc
		 */
		/*override*/  onAwake(): void {
		Laya.stage.on(Event.RIGHT_MOUSE_DOWN, this, this.mouseDown);
		Laya.stage.on(Event.RIGHT_MOUSE_UP, this, this.mouseUp);
		//Laya.stage.on(Event.RIGHT_MOUSE_OUT, this, mouseOut);
		this.camera = (<Camera>this.owner);
	}

		/**
		 * @inheritDoc
		 */
		/*override*/  onUpdate(): void {
		var elapsedTime: number = Laya.timer.delta;
		if (!isNaN(this.lastMouseX) && !isNaN(this.lastMouseY) && this.isMouseDown) {
			var scene: Scene3D = this.owner.scene;
			KeyBoardManager.hasKeyDown(87) && this.moveForward(-0.01 * elapsedTime);//W
			KeyBoardManager.hasKeyDown(83) && this.moveForward(0.01 * elapsedTime);//S
			KeyBoardManager.hasKeyDown(65) && this.moveRight(-0.01 * elapsedTime);//A
			KeyBoardManager.hasKeyDown(68) && this.moveRight(0.01 * elapsedTime);//D
			KeyBoardManager.hasKeyDown(81) && this.moveVertical(0.01 * elapsedTime);//Q
			KeyBoardManager.hasKeyDown(69) && this.moveVertical(-0.01 * elapsedTime);//E

			var offsetX: number = Laya.stage.mouseX - this.lastMouseX;
			var offsetY: number = Laya.stage.mouseY - this.lastMouseY;

			var yprElem: Vector3 = this.yawPitchRoll;
			yprElem.x -= offsetX * this.rotaionSpeed * elapsedTime;
			yprElem.y -= offsetY * this.rotaionSpeed * elapsedTime;
			this._updateRotation();
		}
		this.lastMouseX = Laya.stage.mouseX;
		this.lastMouseY = Laya.stage.mouseY;
	}

		/**
		 * @inheritDoc
		 */
		/*override*/  onDestroy(): void {
		Laya.stage.off(Event.RIGHT_MOUSE_DOWN, this, this.mouseDown);
		Laya.stage.off(Event.RIGHT_MOUSE_UP, this, this.mouseUp);
		//Laya.stage.off(Event.RIGHT_MOUSE_OUT, this, mouseOut);
	}

	protected mouseDown(e: Event): void {
		this.camera.transform.localRotation.getYawPitchRoll(this.yawPitchRoll);

		this.lastMouseX = Laya.stage.mouseX;
		this.lastMouseY = Laya.stage.mouseY;
		this.isMouseDown = true;
	}

	protected mouseUp(e: Event): void {
		this.isMouseDown = false;
	}

	protected mouseOut(e: Event): void {
		this.isMouseDown = false;
	}

	/**
	 * 向前移动。
	 * @param distance 移动距离。
	 */
	moveForward(distance: number): void {
		this._tempVector3.x = this._tempVector3.y = 0;
		this._tempVector3.z = distance;
		this.camera.transform.translate(this._tempVector3);
	}

	/**
	 * 向右移动。
	 * @param distance 移动距离。
	 */
	moveRight(distance: number): void {
		this._tempVector3.y = this._tempVector3.z = 0;
		this._tempVector3.x = distance;
		this.camera.transform.translate(this._tempVector3);
	}

	/**
	 * 向上移动。
	 * @param distance 移动距离。
	 */
	moveVertical(distance: number): void {
		this._tempVector3.x = this._tempVector3.z = 0;
		this._tempVector3.y = distance;
		this.camera.transform.translate(this._tempVector3, false);
	}

}


