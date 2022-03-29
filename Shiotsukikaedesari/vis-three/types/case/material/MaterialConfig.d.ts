import { SymbolConfig } from "../common/CommonConfig";
export interface MaterialConfig extends SymbolConfig {
    alphaTest: number;
    colorWrite: boolean;
    depthTest: boolean;
    depthWrite: boolean;
    format: number;
    fog: boolean;
    name: string;
    needsUpdate: boolean;
    opacity: number;
    dithering: boolean;
    shadowSide: number | null;
    side: number;
    toneMapped: boolean;
    transparent: boolean;
    visible: boolean;
}
export interface MeshStandardMaterialConfig extends MaterialConfig {
    aoMapIntensity: number;
    bumpScale: number;
    color: string;
    displacementScale: number;
    displacementBias: number;
    emissive: string;
    emissiveIntensity: number;
    envMapIntensity: number;
    flatShading: boolean;
    lightMapIntensity: number;
    metalness: number;
    normalMapType: number;
    refractionRatio: number;
    roughness: number;
    wireframe: boolean;
    wireframeLinecap: string;
    wireframeLinejoin: string;
    roughnessMap: string;
    normalMap: string;
    metalnessMap: string;
    map: string;
    lightMap: string;
    envMap: string;
    emissiveMap: string;
    displacementMap: string;
    bumpMap: string;
    alphaMap: string;
    aoMap: string;
}
export declare type MaterialAllType = MeshStandardMaterialConfig;
export declare const getMaterialConfig: () => MaterialConfig;
export declare const getMeshStandardMaterialConfig: () => MeshStandardMaterialConfig;
