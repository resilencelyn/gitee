<template>
    <div ref="editor" style="width: 100%; height: 100%"></div>
</template>

<script>
import * as ace from "../js/lib/acejs/ace.min.js"
import("../js/lib/acejs/ext-language_tools.js")
export default {
    name: "AceJson",
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
            this.editor.session.setMode('ace/mode/json');
            this.editor.commands.addCommand({
                name: 'save',
                bindKey: {win: 'Ctrl-S', mac: 'Command-S'},
                exec: (editor) => {
                    this.$emit('update:modelValue', this.editor.session.getValue());
                    this.$emit('save', this.editor.session.getValue());
                },
            });
        },
    }
}
</script>

<style scoped>

</style>