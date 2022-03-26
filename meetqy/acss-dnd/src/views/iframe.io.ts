// import type { VNode } from "@/stores/editor";

import type { CheckedElement } from "@/types";

let iframeElement: HTMLIFrameElement;

// 初始化 iframe
export const initIframeElement = (el: HTMLIFrameElement) => {
  iframeElement = el;
};

export const getIframe = () => iframeElement;

export enum IframeIoType {
  menu,
  mainToEditor,
  tempToEditor,
  editorToSide,
  sideToEditor,
}

interface PostParam<T> {
  type: IframeIoType;
  data: T;
}

// 父传子
function parentToChild<T>(params: PostParam<T>) {
  return iframeElement.contentWindow?.postMessage(params);
}

// 子传父
function childToParent<T>(_win: Window, params: PostParam<T>) {
  return _win.parent.postMessage(params);
}

export const iframeIo = {
  // main通知到editor
  mainToEditor: (data: "notice"): void => {
    parentToChild({ type: IframeIoType.mainToEditor, data });
  },

  tempToEditor: (data: string): void => {
    parentToChild({
      type: IframeIoType.tempToEditor,
      data,
    });
  },

  editorToSide: (_win: Window, data: CheckedElement): void => {
    childToParent(_win, {
      type: IframeIoType.editorToSide,
      data,
    });
  },

  // null: 点击sideBar关闭按钮
  sideToEditor: (data: CheckedElement | null): void => {
    parentToChild({ type: IframeIoType.sideToEditor, data });
  },

  on: (
    type: IframeIoType,
    callback: (data: CheckedElement | string) => void
  ) => {
    window.addEventListener("message", (e) => {
      if (e.data.type === type) {
        callback(e.data.data);
      }
    });
  },
};
