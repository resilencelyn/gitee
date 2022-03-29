import { Group } from "three";
import { ObjectDataSupport } from "../object/ObjectDataSupport";
import { GroupCompiler, GroupCompilerTarget } from "./GroupCompiler";
import { GroupConfig } from "./GroupConfig";
import { GroupRule } from "./GroupRule";

export class GroupDataSupport extends ObjectDataSupport<
GroupRule,
GroupCompiler,
GroupConfig,
GroupCompilerTarget,
Group
>{
  constructor (data?: GroupCompilerTarget) {
    !data && (data = {})
    super(GroupRule, data)
  }
}