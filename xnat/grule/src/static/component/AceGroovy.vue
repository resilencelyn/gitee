<template>
    <div ref="editor" style="width: 100%; height: 100%"></div>
</template>

<script>
import * as ace from "../js/lib/acejs/ace.min.js"
import("../js/lib/acejs/ext-language_tools.js")
export default {
    name: "AceGroovy",
    props: {modelValue: String, readonly: Boolean, placeholder: String},
    emits: ['update:modelValue'],
    created() {
        ace.config.set('basePath', "js/lib/acejs/")
    },
    beforeDestroy() {
        if (this.editor) {this.editor.destroy()}
    },
    mounted() {
        this.initEditor()
    },
    watch: {
      modelValue(v) {
          if (this.editor && v !== this.editor.session.getValue()) {
              this.editor.session.setValue(v);
          }
      },
      readonly() {
        this.editor.setReadOnly(this.readonly === true)
      }
    },
    methods: {
        initEditor() {
            this.editor = ace.edit(this.$refs.editor);
            if (this.modelValue) this.editor.session.setValue(this.modelValue);
            this.editor.setReadOnly(this.readonly === true);
            this.editor.setOptions({
                enableBasicAutocompletion: true,
                enableSnippets: true,
                enableLiveAutocompletion: true,
                placeholder: this.placeholder || '',
            });
            this.editor.on('change', (e) => {
                this.$emit('update:modelValue', this.editor.session.getValue())
            });
            this.editor.session.setMode('ace/mode/groovy');
            //this.editor.execCommand('find');
            this.editor.commands.addCommand({
                name: 'save',
                bindKey: {win: 'Ctrl-S', mac: 'Command-S'},
                exec: (editor) => {
                    // this.modelValue = this.editor.session.getValue();
                    this.$emit('update:modelValue', this.editor.session.getValue());
                    this.$emit('save', this.editor.session.getValue());
                },
                // readOnly: false // 如果不需要使用只读模式，这里设置false
            });
            // let languageTools = ace.require("ace/ext/language_tools");
            // console.log('languageTools', ace.require("ace/ext/language_tools"));
            // languageTools.addCompleter({
            //     getCompletions: (editor, session, pos, prefix, callback) => {
            //         callback(null, [
            //             {
            //                 name : "第一行", //名称
            //                 value : "身份证号码",//值，这就是匹配我们输入的内容，比如输入s或者select,这一行就会出现在提示框里，可根据自己需求修改，就是你想输入什么显示出北京呢，就改成什么
            //                 caption: "身",//字幕，下拉提示左侧内容,这也就是我们输入前缀匹配出来的内容，所以这里必须包含我们的前缀
            //                 meta: "字段-身份证号码", //类型，下拉提示右侧内容
            //                 type: "local",//可写为keyword
            //                 score : 1000 // 让它排在最上面，类似权值的概念
            //             },
            //             {
            //                 name : "年龄", //名称
            //                 value : "年龄",//值，这就是匹配我们输入的内容，比如输入s或者select,这一行就会出现在提示框里，可根据自己需求修改，就是你想输入什么显示出北京呢，就改成什么
            //                 caption: "年",//字幕，下拉提示左侧内容,这也就是我们输入前缀匹配出来的内容，所以这里必须包含我们的前缀
            //                 meta: "字段-年龄", //类型，下拉提示右侧内容
            //                 type: "local",//可写为keyword
            //                 score : 1000 // 让它排在最上面，类似权值的概念
            //             }
            //         ]);
            //     }
            // });
        },
    }
}
</script>

<style scoped>

</style>