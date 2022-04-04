import { Camera, Scene, WebGLRenderer } from "three";
import { OrbitControls } from "three/examples/jsm/controls/OrbitControls";
import { EffectComposer } from "three/examples/jsm/postprocessing/EffectComposer";
import { EventManager } from "../manager/EventManager";
import { PointerManager } from "../manager/PointerManager";
import { RenderManager } from "../manager/RenderManager";
import { EngineSupport, EngineSupportParameters } from "./EngineSupport";
export declare class DisplayEngineSupport extends EngineSupport {
    dom: HTMLElement;
    webGLRenderer: WebGLRenderer;
    currentCamera: Camera;
    scene: Scene;
    orbitControls: OrbitControls;
    effectComposer: EffectComposer;
    renderManager: RenderManager;
    pointerManager: PointerManager;
    eventManager: EventManager;
    transing: boolean;
    setSize: (width: number, height: number) => this;
    setCamera: (camera: Camera) => this;
    setDom: (dom: HTMLElement) => this;
    play: () => this;
    stop: () => this;
    render: () => this;
    constructor(parameters?: EngineSupportParameters);
}