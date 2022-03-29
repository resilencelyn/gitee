import { DodecahedronBufferGeometry, Points, PointsMaterial } from "three";
import { Compiler } from "../../core/Compiler";
import { MODULETYPE } from "../constants/MODULETYPE";
import { ObjectCompiler } from "../object/ObjectCompiler";
export class PointsCompiler extends ObjectCompiler {
    COMPILER_NAME = MODULETYPE.POINTS;
    replaceMaterial = new PointsMaterial({ color: 'rgb(150, 150, 150)' });
    replaceGeometry = new DodecahedronBufferGeometry(5);
    constructor(parameters) {
        super(parameters);
    }
    getReplaceMaterial() {
        return this.replaceMaterial;
    }
    getReplaceGeometry() {
        return this.replaceGeometry;
    }
    add(vid, config) {
        const object = new Points(this.getGeometry(config.geometry), this.getMaterial(config.material));
        Compiler.applyConfig(config, object, {
            geometry: true,
            material: true,
            lookAt: true
        });
        this.map.set(vid, object);
        this.weakMap.set(object, vid);
        this.setLookAt(vid, config.lookAt);
        this.scene.add(object);
        return this;
    }
    set(vid, path, key, value) {
        if (!this.map.has(vid)) {
            console.warn(`PointsCompiler: can not found this vid mapping object: '${vid}'`);
            return this;
        }
        let mesh = this.map.get(vid);
        if (key === 'lookAt') {
            return this.setLookAt(vid, value);
        }
        if (key === 'material') {
            mesh.material = this.getMaterial(value);
            return this;
        }
        for (let key of path) {
            mesh = mesh[key];
        }
        mesh[key] = value;
        return this;
    }
    dispose() {
        super.dispose();
        this.replaceGeometry.dispose();
        this.replaceMaterial.dispose();
        return this;
    }
}
//# sourceMappingURL=PointsCompiler.js.map