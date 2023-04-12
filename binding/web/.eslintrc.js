// Rules reference: http://eslint.org/docs/rules/
module.exports = {
  env: {
    browser: true,
    node: true,
    es6: true,
    mocha: true
  },

  parser: '@typescript-eslint/parser',
  parserOptions: {
    ecmaVersion: 2018
  },

  ignorePatterns: ['**/*.js', 'node_modules', 'dist'],
  overrides: [
    {
      files: ['src/**/*.ts'],
      extends: ['plugin:@typescript-eslint/recommended'],
      rules: {
        '@typescript-eslint/no-parameter-properties': 2,
        '@typescript-eslint/no-explicit-any': 0,
        '@typescript-eslint/no-var-requires': 2,
        '@typescript-eslint/no-non-null-assertion': 2,
        '@typescript-eslint/no-use-before-define': 2,
        '@typescript-eslint/camelcase': 0,
        '@typescript-eslint/no-empty-interface': 2,
        '@typescript-eslint/explicit-function-return-type': 2,
        '@typescript-eslint/ban-ts-comment': 0,
        '@typescript-eslint/no-empty-function': [2, { "allow": ["constructors"] }],
        '@typescript-eslint/no-inferrable-types': [
          2,
          {
            ignoreParameters: true,
            ignoreProperties: true
          }
        ],
        '@typescript-eslint/no-shadow': 2
      }
    },
    {
      files: ['test/**/*.ts', 'cypress/**/*.ts'],
      extends: ['plugin:cypress/recommended'],
      rules: {
        'no-unused-expressions': 0,
        'no-unused-vars': 0
      }
    }
  ],

  rules: {
    //=========================================================================
    //==================== Possible Errors ====================================
    //=========================================================================

    // disallow trailing commas in object literals
    'comma-dangle': [0, 'always-multiline'], //TODO: need discussion
    // disallow assignment in conditional expressions
    'no-cond-assign': [2, 'always'],
    // disallow use of console
    'no-console': 1,
    // disallow use of constant expressions in conditions
    'no-constant-condition': [2, { checkLoops: false }],
    // disallow control characters in regular expressions
    'no-control-regex': 2,
    // disallow use of debugger
    'no-debugger': 2,
    // disallow duplicate arguments in functions
    'no-dupe-args': 2,
    // disallow duplicate keys when creating object literals
    'no-dupe-keys': 2,
    // disallow a duplicate case label.
    'no-duplicate-case': 2,
    // disallow the use of empty character classes in regular expressions
    'no-empty-character-class': 2,
    // disallow empty statements
    'no-empty': 2,
    // disallow assigning to the exception in a catch block
    'no-ex-assign': 2,
    // disallow double-negation boolean casts in a boolean context
    'no-extra-boolean-cast': 2,
    // disallow unnecessary parentheses
    'no-extra-parens': [2, 'functions'],
    // disallow unnecessary semicolons
    'no-extra-semi': 2,
    // disallow overwriting functions written as function declarations
    'no-func-assign': 2,
    // disallow function or variable declarations in nested blocks
    'no-inner-declarations': 2,
    // disallow invalid regular expression strings in the RegExp constructor
    'no-invalid-regexp': 2,
    // disallow irregular whitespace outside of strings and comments
    'no-irregular-whitespace': 2,
    // disallow negation of the left operand of an in expression
    'no-negated-in-lhs': 2,
    // disallow the use of object properties of the global object (Math and JSON) as functions
    'no-obj-calls': 2,
    // disallow multiple spaces in a regular expression literal
    'no-regex-spaces': 2,
    // disallow sparse arrays
    'no-sparse-arrays': 2,
    // Avoid code that looks like two expressions but is actually one
    'no-unexpected-multiline': 2,
    // disallow unreachable statements after a return, throw, continue, or break statement
    'no-unreachable': 2,
    // disallow comparisons with the value NaN
    'use-isnan': 2,
    // ensure JSDoc comments are valid
    'valid-jsdoc': [
      0,
      {
        requireReturn: false,
        requireReturnDescription: false
      }
    ],
    // ensure that the results of typeof are compared against a valid string
    'valid-typeof': 2,

    //=========================================================================
    //==================== Best Practices =====================================
    //=========================================================================
    // Enforces getter/setter pairs in objects
    'accessor-pairs': 2,
    // treat var statements as if they were block scoped
    'block-scoped-var': 2,
    // specify the maximum cyclomatic complexity allowed in a program
    complexity: [0, 11],
    // require return statements to either always or never specify values
    'consistent-return': 2,
    // specify curly brace conventions for all control statements
    curly: [2, 'multi-line'],
    // require default case in switch statements
    'default-case': 2,
    // encourages use of dot notation whenever possible
    'dot-notation': [2, { allowKeywords: true }],
    // enforces consistent newlines before or after dots
    'dot-location': [2, 'property'],
    // require the use of === and !==
    eqeqeq: 2,
    // make sure for-in loops have an if statement
    'guard-for-in': 2,
    // disallow the use of alert, confirm, and prompt
    'no-alert': 2,
    // disallow use of arguments.caller or arguments.callee
    'no-caller': 2,
    // disallow lexical declarations in case clauses
    'no-case-declarations': 0,
    // disallow division operators explicitly at beginning of regular expression
    'no-div-regex': 2,
    // disallow else after a return in an if
    'no-else-return': 2,
    // disallow use of empty destructuring patterns
    'no-empty-pattern': 2,
    // disallow comparisons to null without a type-checking operator
    'no-eq-null': 2,
    // disallow use of eval()
    'no-eval': 2,
    // disallow adding to native types
    'no-extend-native': 2,
    // disallow unnecessary function binding
    'no-extra-bind': 2,
    // disallow fallthrough of case statements
    'no-fallthrough': 2,
    // disallow the use of leading or trailing decimal points in numeric literals
    'no-floating-decimal': 2,
    // disallow the type conversions with shorter notations
    'no-implicit-coercion': 2,
    // disallow use of eval()-like methods
    'no-implied-eval': 2,
    // disallow this keywords outside of classes or class-like objects
    'no-invalid-this': 0,
    // disallow usage of __iterator__ property
    'no-iterator': 2,
    // disallow use of labeled statements
    'no-labels': 2,
    // disallow unnecessary nested blocks
    'no-lone-blocks': 2,
    // disallow creation of functions within loops
    'no-loop-func': 2,
    // disallow the use of magic numbers
    'no-magic-numbers': 0, //TODO: need discussion
    // disallow use of multiple spaces
    'no-multi-spaces': 2,
    // disallow use of multiline strings
    'no-multi-str': 2,
    // disallow reassignments of native objects
    'no-native-reassign': 2,
    // disallow use of new operator for Function object
    'no-new-func': 2,
    // disallows creating new instances of String,Number, and Boolean
    'no-new-wrappers': 2,
    // disallow use of new operator when not part of the assignment or comparison
    'no-new': 2,
    // disallow use of octal escape sequences in string literals, such as
    // var foo = "Copyright \251";
    'no-octal-escape': 2,
    // disallow use of (old style) octal literals
    'no-octal': 2,
    // disallow reassignment of function parameters
    'no-param-reassign': 2,
    // disallow use of process.env
    'no-process-env': 2,
    // disallow usage of __proto__ property
    'no-proto': 2,
    // disallow declaring the same variable more then once
    'no-redeclare': 2,
    // disallow use of assignment in return statement
    'no-return-assign': 2,
    // disallow use of `javascript:` urls.
    'no-script-url': 2,
    // disallow comparisons where both sides are exactly the same
    'no-self-compare': 2,
    // disallow use of comma operator
    'no-sequences': 2,
    // restrict what can be thrown as an exception
    'no-throw-literal': 0,
    // disallow usage of expressions in statement position
    'no-unused-expressions': 2,
    // disallow unnecessary .call() and .apply()
    'no-useless-call': 2,
    // disallow unnecessary concatenation of literals or template literals
    'no-useless-concat': 2,
    // disallow use of void operator
    'no-void': 2,
    // disallow usage of configurable warning terms in comments: e.g. todo
    'no-warning-comments': [
      1,
      { terms: ['todo', 'fixme', 'xxx'], location: 'start' }
    ],
    // disallow use of the with statement
    'no-with': 2,
    // require use of the second argument for parseInt()
    radix: 2,
    // requires to declare all vars on top of their containing scope
    'vars-on-top': 0,
    // require immediate function invocation to be wrapped in parentheses
    'wrap-iife': [2, 'any'],
    // require or disallow Yoda conditions
    yoda: 2,

    // //=========================================================================
    // //==================== Strict Mode ========================================
    // //=========================================================================
    // require that all functions are run in strict mode
    // "strict": [2, "global"],
    //
    //=========================================================================
    //==================== Variables ==========================================
    //=========================================================================
    // enforce or disallow variable initializations at definition
    'init-declarations': 0,
    // disallow the catch clause parameter name being the same as a variable in the outer scope
    'no-catch-shadow': 0,
    // disallow deletion of variables
    'no-delete-var': 2,
    // disallow labels that share a name with a variable
    'no-label-var': 2,
    // disallow shadowing of names such as arguments
    'no-shadow-restricted-names': 2,
    // disallow declaration of variables already declared in the outer scope
    'no-shadow': 0,
    // disallow use of undefined when initializing variables
    'no-undef-init': 0,
    // disallow use of undeclared variables unless mentioned in a /*global */ block
    'no-undef': 2,
    // disallow use of undefined variable
    'no-undefined': 0,
    // disallow declaration of variables that are not used in the code
    'no-unused-vars': [1, { vars: 'local', args: 'after-used' }],
    // disallow use of variables before they are defined
    'no-use-before-define': 2,

    //=========================================================================
    //==================== Node.js ============================================
    //=========================================================================
    // enforce return after a callback
    'callback-return': 0,
    // disallow require() outside of the top-level module scope
    'global-require': 2,
    // enforces error handling in callbacks (node environment)
    'handle-callback-err': 2,
    // disallow mixing regular variable and require declarations
    'no-mixed-requires': 2,
    // disallow use of new operator with the require function
    'no-new-require': 2,
    // disallow string concatenation with __dirname and __filename
    'no-path-concat': 1,
    // disallow process.exit()
    'no-process-exit': 2,
    // restrict usage of specified node modules
    'no-restricted-modules': 0,
    // disallow use of synchronous methods (off by default)
    'no-sync': 0,

    //=========================================================================
    //==================== Stylistic Issues ===================================
    //=========================================================================
    // enforce spacing inside array brackets
    'array-bracket-spacing': 0,
    // disallow or enforce spaces inside of single line blocks
    'block-spacing': 1,
    // enforce one true brace style
    'brace-style': [1, '1tbs', { allowSingleLine: true }],
    // require camel case names
    camelcase: [1, { properties: 'always' }],
    // enforce spacing before and after comma
    'comma-spacing': [1, { before: false, after: true }],
    // enforce one true comma style
    'comma-style': [1, 'last'],
    // require or disallow padding inside computed properties
    'computed-property-spacing': 0,
    // enforces consistent naming when capturing the current execution context
    'consistent-this': 0,
    // enforce newline at the end of file, with no multiple empty lines
    'eol-last': 1,
    // require function expressions to have a name
    'func-names': 0,
    // enforces use of function declarations or expressions
    'func-style': 0,
    // this option enforces minimum and maximum identifier lengths (variable names, property names etc.)
    'id-length': 0,
    // require identifiers to match the provided regular expression
    'id-match': 0,
    // this option sets a specific tab width for your code
    indent: [1, 2, { SwitchCase: 1 }],
    // specify whether double or single quotes should be used in JSX attributes
    'jsx-quotes': [1, 'prefer-double'],
    // enforces spacing between keys and values in object literal properties
    'key-spacing': [1, { beforeColon: false, afterColon: true }],
    // disallow mixed "LF" and "CRLF" as linebreaks
    'linebreak-style': 0,
    // enforces empty lines around comments
    'lines-around-comment': 0,
    // specify the maximum depth that blocks can be nested
    'max-depth': [0, 4],
    // specify the maximum length of a line in your program
    'max-len': [0, 80, 4],
    // specify the maximum depth callbacks can be nested
    'max-nested-callbacks': 0,
    // limits the number of parameters that can be used in the function declaration.
    'max-params': [0, 3],
    // specify the maximum number of statement allowed in a function
    'max-statements': [0, 10],
    // require a capital letter for constructors
    'new-cap': [1, { newIsCap: true }],
    // disallow the omission of parentheses when invoking a constructor with no arguments
    'new-parens': 0,
    // allow/disallow an empty newline after var statement
    'newline-after-var': 0,
    // disallow use of the Array constructor
    'no-array-constructor': 0,
    // disallow use of bitwise operators
    'no-bitwise': 0,
    // disallow use of the continue statement
    'no-continue': 0,
    // disallow comments inline after code
    'no-inline-comments': 0,
    // disallow if as the only statement in an else block
    'no-lonely-if': 0,
    // disallow mixed spaces and tabs for indentation
    'no-mixed-spaces-and-tabs': 1,
    // disallow multiple empty lines
    'no-multiple-empty-lines': [1, { max: 2, maxEOF: 1 }],
    // disallow negated conditions
    'no-negated-condition': 0,
    // disallow nested ternary expressions
    'no-nested-ternary': 1,
    // disallow use of the Object constructor
    'no-new-object': 1,
    // disallow use of unary operators, ++ and --
    'no-plusplus': 0,
    // disallow use of certain syntax in code
    'no-restricted-syntax': 0,
    // disallow space between function identifier and application
    'no-spaced-func': 1,
    // disallow the use of ternary operators
    'no-ternary': 0,
    // disallow trailing whitespace at the end of lines
    'no-trailing-spaces': 1,
    // disallow dangling underscores in identifiers
    'no-underscore-dangle': 0,
    // disallow the use of Boolean literals in conditional expressions
    'no-unneeded-ternary': 0,
    // require or disallow padding inside curly braces
    'object-curly-spacing': ["error", "always"],
    // allow just one var statement per function
    'one-var': [1, 'never'],
    // require assignment operator shorthand where possible or prohibit it entirely
    'operator-assignment': 0,
    // enforce operators to be placed before or after line breaks
    'operator-linebreak': 0,
    // enforce padding within blocks
    'padded-blocks': [1, 'never'],
    // require quotes around object literal property names
    'quote-props': 0,
    // specify whether double or single quotes should be used
    quotes: 0,
    // Require JSDoc comment
    'require-jsdoc': 0,
    // enforce spacing before and after semicolons
    'semi-spacing': [1, { before: false, after: true }],
    // require or disallow use of semicolons instead of ASI
    semi: [1, 'always'],
    // sort variables within the same declaration block
    'sort-vars': 0,
    // require a space after certain keywords
    'keyword-spacing': 1,
    // require or disallow space before blocks
    'space-before-blocks': 1,
    // require or disallow space before function opening parenthesis
    'space-before-function-paren': [0, { anonymous: 'always', named: 'never' }],
    // require or disallow space before blocks
    'space-in-parens': 0,
    // require spaces around operators
    'space-infix-ops': 1,
    // Require or disallow spaces before/after unary operators
    'space-unary-ops': 0,
    // require or disallow a space immediately following the // or /* in a comment
    'spaced-comment': [
      1,
      'always',
      {
        exceptions: ['-', '+', '/', '='],
        markers: ['=', '!', '/'] // space here to support sprockets directives
      }
    ],
    // require regex literals to be wrapped in parentheses
    'wrap-regex': 0,

    //=========================================================================
    //==================== ES6 Rules ==========================================
    //=========================================================================
    'arrow-body-style': [1, 'as-needed'],
    // require parens in arrow function arguments
    'arrow-parens': [1, 'as-needed'],
    // require space before/after arrow function"s arrow
    'arrow-spacing': 1,
    // verify super() callings in constructors
    'constructor-super': 1,
    // enforce the spacing around the * in generator functions
    'generator-star-spacing': 1,
    // disallow arrow functions where a condition is expected
    'no-confusing-arrow': 1,
    // disallow modifying variables of class declarations
    'no-class-assign': 1,
    // disallow modifying variables that are declared using const
    'no-const-assign': 1,
    // disallow duplicate name in class members
    'no-dupe-class-members': 1,
    // disallow to use this/super before super() calling in constructors.
    'no-this-before-super': 1,
    // require let or const instead of var
    'no-var': 0, //TODO: enable on full migration to es6
    // require method and property shorthand syntax for object literals
    'object-shorthand': 0,
    // suggest using arrow functions as callbacks
    'prefer-arrow-callback': 0, //TODO: enable on full migration to es6
    // suggest using of const declaration for variables that are never modified after declared
    'prefer-const': 0, //TODO: enable on full migration to es6
    // suggest using Reflect methods where applicable
    'prefer-reflect': 0,
    // suggest using the spread operator instead of .apply()
    'prefer-spread': 0,
    // suggest using template literals instead of strings concatenation
    'prefer-template': 0, //TODO: enable on full migration to es6
    // disallow generator functions that do not have yield
    'require-yield': 0
  }
};
