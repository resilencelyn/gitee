<script lang="ts">
export default {
  name: 'JmMarkdown',
};
</script>

<script lang="ts" setup>
import { defineEmits, defineProps, nextTick, onUpdated, PropType, ref, computed } from 'vue';
import Editor, { StaticTextDefaultValue } from 'md-editor-v3';
import 'md-editor-v3/lib/style.css';

type Mode = 'readonly' | 'plaintext' | 'editable';
const props = defineProps({
  modelValue: {
    type: String,
    default: '',
  },
  mode: {
    type: String as PropType<Mode>,
    required: true,
  },
});

const emit = defineEmits(['save']);

// 只读/编辑模式状态
const modeStatus = computed<boolean>(() => (props.mode === 'readonly' || props.mode === 'plaintext'));
// 获取组件的html元素
const mdEditorRef = ref<HTMLElement>();
// 模式状态切换
const modeFlag = ref<boolean>(true);
// 编辑模式在updated的判断条件
const editFlag = ref<boolean>(false);
// markdown内容
const markdownStr = ref<string>('');
// 纯文本模式内容
const plaintextStr = ref<string>('');
/**
 * 封装清除html格式方法
 * @param htmlStr
 */
const delHtmlTag = (htmlStr: string) => {
  return htmlStr
    // 去掉html标签
    .replace(/<[^>]+>/g, '')
    // 去掉内容中的缩进
    .replace(/&emsp;/ig, '')
    .replace(/&nbsp;/ig, '')
    .replace(/\n/g, '');
};

// 自定义toolbar移入效果
const languageUserDefined: { 'zh-NB': StaticTextDefaultValue } = {
  'zh-NB': {
    toolbarTips: {
      bold: '加粗',
      underline: '下划线',
      italic: '斜体',
      strikeThrough: '删除线',
      title: '标题',
      sub: '下标',
      sup: '上标',
      quote: '引用',
      unorderedList: '无序列表',
      orderedList: '有序列表',
      codeRow: '行内代码',
      code: '块级代码',
      link: '链接',
      image: '图片',
      table: '表格',
      revoke: '后退',
      next: '前进',
      save: '保存',
      prettier: '美化',
      pageFullscreen: '浏览器全屏',
      fullscreen: '屏幕全屏',
      preview: '预览',
      htmlPreview: 'html代码预览',
      github: '源码地址',
    },
    titleItem: {
      h1: '一级标题',
      h2: '二级标题',
      h3: '三级标题',
      h4: '四级标题',
      h5: '五级标题',
      h6: '六级标题',
    },
    imgTitleItem: {
      link: '添加链接',
      // upload: '上传图片',
      // clip2upload: '裁剪上传',
    },
    // // 添加链接或图片时弹窗提示
    linkModalTips: {
      title: '添加图片',
      descLable: '链接地址',
      descLablePlaceHolder: '请输入描述...',
      urlLable: '链接描述',
      UrlLablePlaceHolder: '请输入链接...',
      buttonOK: '确定',
    },
    // v1.1.4新增
    copyCode: {
      text: '复制代码',
      tips: '已复制',
    },
    mermaid: {
      // 流程图
      flow: '流程图',
      // 时序图
      sequence: '时序图',
      // 甘特图
      gantt: '甘特图',
      // 类图
      class: '类图',
      // 状态图
      state: '状态图',
      // 饼图
      pie: '饼图',
      // 关系图
      relationship: '关系图',
      // 旅程图
      journey: '旅程图',
    },
    // 1.9.0
    katex: {
      // 行内公式
      inline: '行内公式',
      // 块级公式
      block: '块级公式',
    },
  },
};

// 监听props值变化
onUpdated(() => {
  // 值不一样-通过赋值
  if (markdownStr.value === props.modelValue || editFlag.value) {
    return;
  }
  markdownStr.value = props.modelValue;
  // plaintext纯文本
  if (props.mode === 'plaintext') {
    modeFlag.value = true;
    // 纯文本模式
    nextTick(() => {
      if (!mdEditorRef.value) {
        return;
      }
      // 将html格式转换为纯文本
      plaintextStr.value = delHtmlTag(mdEditorRef.value.$el.outerHTML);
      // 隐藏md-editor-v3组件，将值渲染到div
      modeFlag.value = false;
    });
    return;
  }

});
</script>

<template>
  <div class="jm-markdown">
    <!--  plainText  -->
    <div
      v-if="props.mode === 'plaintext'"
      class="plain-text-mode"
    >
      {{ plaintextStr }}
    </div>
    <!--  readonly/editable  -->
    <Editor
      v-if="modeFlag"
      ref="mdEditorRef"
      v-model="markdownStr"
      :language-user-defined="languageUserDefined"
      :preview="props.mode !== 'editable'"
      :preview-only="modeStatus"
      :toolbars-exclude="['catalog', 'github']"
      language="zh-NB"
      @onChange="editFlag.value = true"
      @onSave="emit('save', content)"
    />
  </div>
</template>

<style lang="less">
.jm-markdown {
  .plain-text-mode {
    font-size: 26px;
    font-weight: 400;
    color: #474F68;
    line-height: 37px;
    background: #fff;
    padding: 60px 36px;
  }

  .default-theme {
    a {
      color: @themeColor;

      &:hover {
        opacity: 0.9;
      }
    }
  }
}

</style>