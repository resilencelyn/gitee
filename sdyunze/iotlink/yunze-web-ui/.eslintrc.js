module.exports = {
    root: true,
    env: {
        node: true
    },
    'extends': [
        'plugin:vue/essential',
        'eslint:recommended'
    ],
    rules: {
        'no-console': 'off',
        'no-debugger': process.env.NODE_ENV === 'production' ? 'error' : 'off',
        'no-undef': 'off',
        'vue/no-unused-vars': 'off',
        'vue/require-v-for-key': 'off',
        "vue/no-parsing-error":"off",
        "vue/no-reserved-keys":"off",
        "vue/valid-v-model":"off",
        "no-unused-vars":"off",
        "vue/no-unused-components":"off",
        " vue/valid-v-on":"off",
        "vue/no-dupe-keys":"off",
        "no-dupe-keys":"off",
        "no-empty":"off",
        "vue/valid-template-root":"off"

    },
    parserOptions: {
        parser: 'babel-eslint'
    }
};