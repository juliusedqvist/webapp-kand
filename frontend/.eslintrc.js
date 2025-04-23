module.exports = {
  root: true,
  env: {
    node: true,
  },
  extends: [
    'eslint:recommended',
    'plugin:vue/vue3-recommended'
  ],
  parserOptions: {
    ecmaVersion: 2020,
  },
  rules: {
    // customize rules here
    'vue/no-unused-vars': 'warn',
  },
};

