import type { ExtractPropTypes } from 'vue'
import type { INode, IMenu, IKeysProps, IMousePosition } from '@/utils/types'
import { buildProps, definePropType } from '@/utils/props'

export const menus = [
  { name: '复制文本', command: 'copy' },
  { name: '新增节点', command: 'add' },
  { name: '编辑节点', command: 'edit' },
  { name: '删除节点', command: 'delete' }
]

export const treeProps = buildProps({
  data: {
    type: Object,
    required: true
  },
  props: {
    type: definePropType<IKeysProps>(Object),
    default: () => ({
      id: 'id',
      pid: 'pid',
      label: 'label',
      expand: 'expand',
      children: 'children'
    })
  },
  toolBar: {
    type: [Object, Boolean],
    default: () => ({
      expand: true,
      scale: true,
      zoom: true,
      restore: true,
      fullscreen: true
    })
  },
  disabled: {
    // 是否禁用编辑
    type: Boolean,
    default: false
  },
  scalable: {
    // 是否可缩放
    type: Boolean,
    default: true
  },
  draggable: {
    // 是否可拖拽移动位置
    type: Boolean,
    default: true
  },
  draggableOnNode: {
    // 是否可拖拽节点移动位置
    type: Boolean,
    default: false
  },
  nodeDraggable: {
    // 节点是否可拖拽
    type: Boolean,
    default: true
  },
  cloneNodeDrag: {
    // 拷贝并拖拽节点
    type: Boolean,
    default: true
  },
  onlyOneNode: {
    // 是否仅拖动当前节点
    type: Boolean,
    default: true
  },
  clickDelay: {
    // 是否仅拖动当前节点
    type: Number,
    default: 260
  },
  nodeDragStart: Function,
  nodeDraging: Function,
  nodeDragEnd: Function,
  horizontal: Boolean,
  selectedKey: String,
  collapsable: Boolean,
  renderContent: Function,
  labelStyle: Object,
  labelClassName: {
    type: [Function, String]
  },
  selectedClassName: {
    type: [Function, String]
  },
  defineMenus: {
    type: definePropType<IMenu[]>(Array),
    default () {
      return menus
    }
  },
  nodeAdd: Function,
  nodeDelete: Function,
  nodeEdit: Function,
  nodeCopy: Function
} as const)

export type TreeProps = ExtractPropTypes<typeof treeProps>

export const treeEmits = {
  'on-drag': ({ x, y }:IMousePosition) => {},
  'on-drag-stop': ({ x, y }:IMousePosition) => {},
  'on-zoom': (val:number) => val,
  'on-expand': (e:MouseEvent, data:INode) => {},
  'on-node-blur': (e:MouseEvent, data:INode) => {},
  'on-node-click': (e:MouseEvent, data:INode) => {},
  'on-node-dblclick': (e:MouseEvent, data:INode) => {},
  'on-node-mouseenter': (e:MouseEvent, data:INode) => {},
  'on-node-mouseleave': (e:MouseEvent, data:INode) => {},
  'on-contextmenu': (data: any) => {},
  'on-node-copy': (str: string) => {},
  'on-node-delete': (node: INode) => {},
  'on-node-focus': (e:MouseEvent, data:INode) => {}

}
export type TreeEmits = typeof treeEmits
