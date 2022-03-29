import { DataSupport } from "../../core/DataSupport";
import { MaterialRule } from "./MaterialRule";
export class MaterialDataSupport extends DataSupport {
    constructor(data) {
        !data && (data = {});
        super(MaterialRule, data);
    }
}
//# sourceMappingURL=MaterialDataSupport.js.map