import { faker } from 'https://cdn.skypack.dev/pin/@faker-js/faker@v6.1.2-XXqPDwHPH8U4a4bdHNrb/mode=imports,min/optimized/@faker-js/faker.js';

function ownKeys(object, enumerableOnly) {
  var keys = Object.keys(object);

  if (Object.getOwnPropertySymbols) {
    var symbols = Object.getOwnPropertySymbols(object);
    enumerableOnly && (symbols = symbols.filter(function (sym) {
      return Object.getOwnPropertyDescriptor(object, sym).enumerable;
    })), keys.push.apply(keys, symbols);
  }

  return keys;
}

function _objectSpread2(target) {
  for (var i = 1; i < arguments.length; i++) {
    var source = null != arguments[i] ? arguments[i] : {};
    i % 2 ? ownKeys(Object(source), !0).forEach(function (key) {
      _defineProperty(target, key, source[key]);
    }) : Object.getOwnPropertyDescriptors ? Object.defineProperties(target, Object.getOwnPropertyDescriptors(source)) : ownKeys(Object(source)).forEach(function (key) {
      Object.defineProperty(target, key, Object.getOwnPropertyDescriptor(source, key));
    });
  }

  return target;
}

function _classCallCheck(instance, Constructor) {
  if (!(instance instanceof Constructor)) {
    throw new TypeError("Cannot call a class as a function");
  }
}

function _defineProperties(target, props) {
  for (var i = 0; i < props.length; i++) {
    var descriptor = props[i];
    descriptor.enumerable = descriptor.enumerable || false;
    descriptor.configurable = true;
    if ("value" in descriptor) descriptor.writable = true;
    Object.defineProperty(target, descriptor.key, descriptor);
  }
}

function _createClass(Constructor, protoProps, staticProps) {
  if (protoProps) _defineProperties(Constructor.prototype, protoProps);
  if (staticProps) _defineProperties(Constructor, staticProps);
  Object.defineProperty(Constructor, "prototype", {
    writable: false
  });
  return Constructor;
}

function _defineProperty(obj, key, value) {
  if (key in obj) {
    Object.defineProperty(obj, key, {
      value: value,
      enumerable: true,
      configurable: true,
      writable: true
    });
  } else {
    obj[key] = value;
  }

  return obj;
}

function _slicedToArray(arr, i) {
  return _arrayWithHoles(arr) || _iterableToArrayLimit(arr, i) || _unsupportedIterableToArray(arr, i) || _nonIterableRest();
}

function _toConsumableArray(arr) {
  return _arrayWithoutHoles(arr) || _iterableToArray(arr) || _unsupportedIterableToArray(arr) || _nonIterableSpread();
}

function _arrayWithoutHoles(arr) {
  if (Array.isArray(arr)) return _arrayLikeToArray(arr);
}

function _arrayWithHoles(arr) {
  if (Array.isArray(arr)) return arr;
}

function _iterableToArray(iter) {
  if (typeof Symbol !== "undefined" && iter[Symbol.iterator] != null || iter["@@iterator"] != null) return Array.from(iter);
}

function _iterableToArrayLimit(arr, i) {
  var _i = arr == null ? null : typeof Symbol !== "undefined" && arr[Symbol.iterator] || arr["@@iterator"];

  if (_i == null) return;
  var _arr = [];
  var _n = true;
  var _d = false;

  var _s, _e;

  try {
    for (_i = _i.call(arr); !(_n = (_s = _i.next()).done); _n = true) {
      _arr.push(_s.value);

      if (i && _arr.length === i) break;
    }
  } catch (err) {
    _d = true;
    _e = err;
  } finally {
    try {
      if (!_n && _i["return"] != null) _i["return"]();
    } finally {
      if (_d) throw _e;
    }
  }

  return _arr;
}

function _unsupportedIterableToArray(o, minLen) {
  if (!o) return;
  if (typeof o === "string") return _arrayLikeToArray(o, minLen);
  var n = Object.prototype.toString.call(o).slice(8, -1);
  if (n === "Object" && o.constructor) n = o.constructor.name;
  if (n === "Map" || n === "Set") return Array.from(o);
  if (n === "Arguments" || /^(?:Ui|I)nt(?:8|16|32)(?:Clamped)?Array$/.test(n)) return _arrayLikeToArray(o, minLen);
}

function _arrayLikeToArray(arr, len) {
  if (len == null || len > arr.length) len = arr.length;

  for (var i = 0, arr2 = new Array(len); i < len; i++) arr2[i] = arr[i];

  return arr2;
}

function _nonIterableSpread() {
  throw new TypeError("Invalid attempt to spread non-iterable instance.\nIn order to be iterable, non-array objects must have a [Symbol.iterator]() method.");
}

function _nonIterableRest() {
  throw new TypeError("Invalid attempt to destructure non-iterable instance.\nIn order to be iterable, non-array objects must have a [Symbol.iterator]() method.");
}

/*
    [正则查看工具](http://www.regexper.com/)

    #26 生成规则 支持 负数，例如 number|-100-100
*/

/** 常量集合 */
var Constant = {
  GUID: 1,

  /**
      RE_KEY
      'name|min-max': value
      'name|count': value
      'name|min-max.dmin-dmax': value
      'name|min-max.dcount': value
      'name|count.dmin-dmax': value
      'name|count.dcount': value
      'name|+step': value
       1 name, 2 step, 3 range [ min, max ], 4 drange [ dmin, dmax ]
    */
  RE_KEY: /(.+)\|(?:\+(\d+)|([\+\-]?\d+-?[\+\-]?\d*)?(?:\.(\d+-?\d*))?)/,
  RE_RANGE: /([\+\-]?\d+)-?([\+\-]?\d+)?/,

  /**
   * RE_PLACEHOLDER placeholder(*)
   * ! 这个被优化了
   */
  //  '@name(123232) @some @nnn(23232,3232) \\@mame'.replace(/(^@|(?<=[^\\])@)([^@#%&()\?\s]+)(?:\((.*?)\))?/g,(all,name,keys)=>{
  //     console.log(name,keys)
  //     return '结果替换'
  // })
  RE_PLACEHOLDER: /(^@|(?<=[^\\])@)([^@#%&()\?\s]+)(?:\((.*?)\))?/g
};

/** Detect free variable `global` from Node.js. */
var freeGlobal = typeof global == 'object' && global && global.Object === Object && global;

var freeGlobal$1 = freeGlobal;

/** Detect free variable `self`. */
var freeSelf = typeof self == 'object' && self && self.Object === Object && self;

/** Used as a reference to the global object. */
var root = freeGlobal$1 || freeSelf || Function('return this')();

var root$1 = root;

/** Built-in value references. */
var Symbol$1 = root$1.Symbol;

var Symbol$2 = Symbol$1;

/** Used for built-in method references. */
var objectProto$8 = Object.prototype;

/** Used to check objects for own properties. */
var hasOwnProperty$6 = objectProto$8.hasOwnProperty;

/**
 * Used to resolve the
 * [`toStringTag`](http://ecma-international.org/ecma-262/7.0/#sec-object.prototype.tostring)
 * of values.
 */
var nativeObjectToString$1 = objectProto$8.toString;

/** Built-in value references. */
var symToStringTag$1 = Symbol$2 ? Symbol$2.toStringTag : undefined;

/**
 * A specialized version of `baseGetTag` which ignores `Symbol.toStringTag` values.
 *
 * @private
 * @param {*} value The value to query.
 * @returns {string} Returns the raw `toStringTag`.
 */
function getRawTag(value) {
  var isOwn = hasOwnProperty$6.call(value, symToStringTag$1),
      tag = value[symToStringTag$1];

  try {
    value[symToStringTag$1] = undefined;
    var unmasked = true;
  } catch (e) {}

  var result = nativeObjectToString$1.call(value);
  if (unmasked) {
    if (isOwn) {
      value[symToStringTag$1] = tag;
    } else {
      delete value[symToStringTag$1];
    }
  }
  return result;
}

/** Used for built-in method references. */
var objectProto$7 = Object.prototype;

/**
 * Used to resolve the
 * [`toStringTag`](http://ecma-international.org/ecma-262/7.0/#sec-object.prototype.tostring)
 * of values.
 */
var nativeObjectToString = objectProto$7.toString;

/**
 * Converts `value` to a string using `Object.prototype.toString`.
 *
 * @private
 * @param {*} value The value to convert.
 * @returns {string} Returns the converted string.
 */
function objectToString(value) {
  return nativeObjectToString.call(value);
}

/** `Object#toString` result references. */
var nullTag = '[object Null]',
    undefinedTag = '[object Undefined]';

/** Built-in value references. */
var symToStringTag = Symbol$2 ? Symbol$2.toStringTag : undefined;

/**
 * The base implementation of `getTag` without fallbacks for buggy environments.
 *
 * @private
 * @param {*} value The value to query.
 * @returns {string} Returns the `toStringTag`.
 */
function baseGetTag(value) {
  if (value == null) {
    return value === undefined ? undefinedTag : nullTag;
  }
  return (symToStringTag && symToStringTag in Object(value))
    ? getRawTag(value)
    : objectToString(value);
}

/**
 * Checks if `value` is object-like. A value is object-like if it's not `null`
 * and has a `typeof` result of "object".
 *
 * @static
 * @memberOf _
 * @since 4.0.0
 * @category Lang
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is object-like, else `false`.
 * @example
 *
 * _.isObjectLike({});
 * // => true
 *
 * _.isObjectLike([1, 2, 3]);
 * // => true
 *
 * _.isObjectLike(_.noop);
 * // => false
 *
 * _.isObjectLike(null);
 * // => false
 */
function isObjectLike(value) {
  return value != null && typeof value == 'object';
}

/** `Object#toString` result references. */
var symbolTag = '[object Symbol]';

/**
 * Checks if `value` is classified as a `Symbol` primitive or object.
 *
 * @static
 * @memberOf _
 * @since 4.0.0
 * @category Lang
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is a symbol, else `false`.
 * @example
 *
 * _.isSymbol(Symbol.iterator);
 * // => true
 *
 * _.isSymbol('abc');
 * // => false
 */
function isSymbol(value) {
  return typeof value == 'symbol' ||
    (isObjectLike(value) && baseGetTag(value) == symbolTag);
}

/**
 * A specialized version of `_.map` for arrays without support for iteratee
 * shorthands.
 *
 * @private
 * @param {Array} [array] The array to iterate over.
 * @param {Function} iteratee The function invoked per iteration.
 * @returns {Array} Returns the new mapped array.
 */
function arrayMap(array, iteratee) {
  var index = -1,
      length = array == null ? 0 : array.length,
      result = Array(length);

  while (++index < length) {
    result[index] = iteratee(array[index], index, array);
  }
  return result;
}

/**
 * Checks if `value` is classified as an `Array` object.
 *
 * @static
 * @memberOf _
 * @since 0.1.0
 * @category Lang
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is an array, else `false`.
 * @example
 *
 * _.isArray([1, 2, 3]);
 * // => true
 *
 * _.isArray(document.body.children);
 * // => false
 *
 * _.isArray('abc');
 * // => false
 *
 * _.isArray(_.noop);
 * // => false
 */
var isArray = Array.isArray;

var isArray$1 = isArray;

/** Used as references for various `Number` constants. */
var INFINITY$2 = 1 / 0;

/** Used to convert symbols to primitives and strings. */
var symbolProto = Symbol$2 ? Symbol$2.prototype : undefined,
    symbolToString = symbolProto ? symbolProto.toString : undefined;

/**
 * The base implementation of `_.toString` which doesn't convert nullish
 * values to empty strings.
 *
 * @private
 * @param {*} value The value to process.
 * @returns {string} Returns the string.
 */
function baseToString(value) {
  // Exit early for strings to avoid a performance hit in some environments.
  if (typeof value == 'string') {
    return value;
  }
  if (isArray$1(value)) {
    // Recursively convert values (susceptible to call stack limits).
    return arrayMap(value, baseToString) + '';
  }
  if (isSymbol(value)) {
    return symbolToString ? symbolToString.call(value) : '';
  }
  var result = (value + '');
  return (result == '0' && (1 / value) == -INFINITY$2) ? '-0' : result;
}

/** Used to match a single whitespace character. */
var reWhitespace = /\s/;

/**
 * Used by `_.trim` and `_.trimEnd` to get the index of the last non-whitespace
 * character of `string`.
 *
 * @private
 * @param {string} string The string to inspect.
 * @returns {number} Returns the index of the last non-whitespace character.
 */
function trimmedEndIndex(string) {
  var index = string.length;

  while (index-- && reWhitespace.test(string.charAt(index))) {}
  return index;
}

/** Used to match leading whitespace. */
var reTrimStart = /^\s+/;

/**
 * The base implementation of `_.trim`.
 *
 * @private
 * @param {string} string The string to trim.
 * @returns {string} Returns the trimmed string.
 */
function baseTrim(string) {
  return string
    ? string.slice(0, trimmedEndIndex(string) + 1).replace(reTrimStart, '')
    : string;
}

/**
 * Checks if `value` is the
 * [language type](http://www.ecma-international.org/ecma-262/7.0/#sec-ecmascript-language-types)
 * of `Object`. (e.g. arrays, functions, objects, regexes, `new Number(0)`, and `new String('')`)
 *
 * @static
 * @memberOf _
 * @since 0.1.0
 * @category Lang
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is an object, else `false`.
 * @example
 *
 * _.isObject({});
 * // => true
 *
 * _.isObject([1, 2, 3]);
 * // => true
 *
 * _.isObject(_.noop);
 * // => true
 *
 * _.isObject(null);
 * // => false
 */
function isObject(value) {
  var type = typeof value;
  return value != null && (type == 'object' || type == 'function');
}

/** Used as references for various `Number` constants. */
var NAN = 0 / 0;

/** Used to detect bad signed hexadecimal string values. */
var reIsBadHex = /^[-+]0x[0-9a-f]+$/i;

/** Used to detect binary string values. */
var reIsBinary = /^0b[01]+$/i;

/** Used to detect octal string values. */
var reIsOctal = /^0o[0-7]+$/i;

/** Built-in method references without a dependency on `root`. */
var freeParseInt = parseInt;

/**
 * Converts `value` to a number.
 *
 * @static
 * @memberOf _
 * @since 4.0.0
 * @category Lang
 * @param {*} value The value to process.
 * @returns {number} Returns the number.
 * @example
 *
 * _.toNumber(3.2);
 * // => 3.2
 *
 * _.toNumber(Number.MIN_VALUE);
 * // => 5e-324
 *
 * _.toNumber(Infinity);
 * // => Infinity
 *
 * _.toNumber('3.2');
 * // => 3.2
 */
function toNumber(value) {
  if (typeof value == 'number') {
    return value;
  }
  if (isSymbol(value)) {
    return NAN;
  }
  if (isObject(value)) {
    var other = typeof value.valueOf == 'function' ? value.valueOf() : value;
    value = isObject(other) ? (other + '') : other;
  }
  if (typeof value != 'string') {
    return value === 0 ? value : +value;
  }
  value = baseTrim(value);
  var isBinary = reIsBinary.test(value);
  return (isBinary || reIsOctal.test(value))
    ? freeParseInt(value.slice(2), isBinary ? 2 : 8)
    : (reIsBadHex.test(value) ? NAN : +value);
}

/** Used as references for various `Number` constants. */
var INFINITY$1 = 1 / 0,
    MAX_INTEGER = 1.7976931348623157e+308;

/**
 * Converts `value` to a finite number.
 *
 * @static
 * @memberOf _
 * @since 4.12.0
 * @category Lang
 * @param {*} value The value to convert.
 * @returns {number} Returns the converted number.
 * @example
 *
 * _.toFinite(3.2);
 * // => 3.2
 *
 * _.toFinite(Number.MIN_VALUE);
 * // => 5e-324
 *
 * _.toFinite(Infinity);
 * // => 1.7976931348623157e+308
 *
 * _.toFinite('3.2');
 * // => 3.2
 */
function toFinite(value) {
  if (!value) {
    return value === 0 ? value : 0;
  }
  value = toNumber(value);
  if (value === INFINITY$1 || value === -INFINITY$1) {
    var sign = (value < 0 ? -1 : 1);
    return sign * MAX_INTEGER;
  }
  return value === value ? value : 0;
}

/**
 * Converts `value` to an integer.
 *
 * **Note:** This method is loosely based on
 * [`ToInteger`](http://www.ecma-international.org/ecma-262/7.0/#sec-tointeger).
 *
 * @static
 * @memberOf _
 * @since 4.0.0
 * @category Lang
 * @param {*} value The value to convert.
 * @returns {number} Returns the converted integer.
 * @example
 *
 * _.toInteger(3.2);
 * // => 3
 *
 * _.toInteger(Number.MIN_VALUE);
 * // => 0
 *
 * _.toInteger(Infinity);
 * // => 1.7976931348623157e+308
 *
 * _.toInteger('3.2');
 * // => 3
 */
function toInteger(value) {
  var result = toFinite(value),
      remainder = result % 1;

  return result === result ? (remainder ? result - remainder : result) : 0;
}

/**
 * This method returns the first argument it receives.
 *
 * @static
 * @since 0.1.0
 * @memberOf _
 * @category Util
 * @param {*} value Any value.
 * @returns {*} Returns `value`.
 * @example
 *
 * var object = { 'a': 1 };
 *
 * console.log(_.identity(object) === object);
 * // => true
 */
function identity(value) {
  return value;
}

/** `Object#toString` result references. */
var asyncTag = '[object AsyncFunction]',
    funcTag$1 = '[object Function]',
    genTag = '[object GeneratorFunction]',
    proxyTag = '[object Proxy]';

/**
 * Checks if `value` is classified as a `Function` object.
 *
 * @static
 * @memberOf _
 * @since 0.1.0
 * @category Lang
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is a function, else `false`.
 * @example
 *
 * _.isFunction(_);
 * // => true
 *
 * _.isFunction(/abc/);
 * // => false
 */
function isFunction(value) {
  if (!isObject(value)) {
    return false;
  }
  // The use of `Object#toString` avoids issues with the `typeof` operator
  // in Safari 9 which returns 'object' for typed arrays and other constructors.
  var tag = baseGetTag(value);
  return tag == funcTag$1 || tag == genTag || tag == asyncTag || tag == proxyTag;
}

/** Used to detect overreaching core-js shims. */
var coreJsData = root$1['__core-js_shared__'];

var coreJsData$1 = coreJsData;

/** Used to detect methods masquerading as native. */
var maskSrcKey = (function() {
  var uid = /[^.]+$/.exec(coreJsData$1 && coreJsData$1.keys && coreJsData$1.keys.IE_PROTO || '');
  return uid ? ('Symbol(src)_1.' + uid) : '';
}());

/**
 * Checks if `func` has its source masked.
 *
 * @private
 * @param {Function} func The function to check.
 * @returns {boolean} Returns `true` if `func` is masked, else `false`.
 */
function isMasked(func) {
  return !!maskSrcKey && (maskSrcKey in func);
}

/** Used for built-in method references. */
var funcProto$1 = Function.prototype;

/** Used to resolve the decompiled source of functions. */
var funcToString$1 = funcProto$1.toString;

/**
 * Converts `func` to its source code.
 *
 * @private
 * @param {Function} func The function to convert.
 * @returns {string} Returns the source code.
 */
function toSource(func) {
  if (func != null) {
    try {
      return funcToString$1.call(func);
    } catch (e) {}
    try {
      return (func + '');
    } catch (e) {}
  }
  return '';
}

/**
 * Used to match `RegExp`
 * [syntax characters](http://ecma-international.org/ecma-262/7.0/#sec-patterns).
 */
var reRegExpChar = /[\\^$.*+?()[\]{}|]/g;

/** Used to detect host constructors (Safari). */
var reIsHostCtor = /^\[object .+?Constructor\]$/;

/** Used for built-in method references. */
var funcProto = Function.prototype,
    objectProto$6 = Object.prototype;

/** Used to resolve the decompiled source of functions. */
var funcToString = funcProto.toString;

/** Used to check objects for own properties. */
var hasOwnProperty$5 = objectProto$6.hasOwnProperty;

/** Used to detect if a method is native. */
var reIsNative = RegExp('^' +
  funcToString.call(hasOwnProperty$5).replace(reRegExpChar, '\\$&')
  .replace(/hasOwnProperty|(function).*?(?=\\\()| for .+?(?=\\\])/g, '$1.*?') + '$'
);

/**
 * The base implementation of `_.isNative` without bad shim checks.
 *
 * @private
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is a native function,
 *  else `false`.
 */
function baseIsNative(value) {
  if (!isObject(value) || isMasked(value)) {
    return false;
  }
  var pattern = isFunction(value) ? reIsNative : reIsHostCtor;
  return pattern.test(toSource(value));
}

/**
 * Gets the value at `key` of `object`.
 *
 * @private
 * @param {Object} [object] The object to query.
 * @param {string} key The key of the property to get.
 * @returns {*} Returns the property value.
 */
function getValue(object, key) {
  return object == null ? undefined : object[key];
}

/**
 * Gets the native function at `key` of `object`.
 *
 * @private
 * @param {Object} object The object to query.
 * @param {string} key The key of the method to get.
 * @returns {*} Returns the function if it's native, else `undefined`.
 */
function getNative(object, key) {
  var value = getValue(object, key);
  return baseIsNative(value) ? value : undefined;
}

/**
 * Copies the values of `source` to `array`.
 *
 * @private
 * @param {Array} source The array to copy values from.
 * @param {Array} [array=[]] The array to copy values to.
 * @returns {Array} Returns `array`.
 */
function copyArray(source, array) {
  var index = -1,
      length = source.length;

  array || (array = Array(length));
  while (++index < length) {
    array[index] = source[index];
  }
  return array;
}

/** Used as references for various `Number` constants. */
var MAX_SAFE_INTEGER$2 = 9007199254740991;

/** Used to detect unsigned integer values. */
var reIsUint = /^(?:0|[1-9]\d*)$/;

/**
 * Checks if `value` is a valid array-like index.
 *
 * @private
 * @param {*} value The value to check.
 * @param {number} [length=MAX_SAFE_INTEGER] The upper bounds of a valid index.
 * @returns {boolean} Returns `true` if `value` is a valid index, else `false`.
 */
function isIndex(value, length) {
  var type = typeof value;
  length = length == null ? MAX_SAFE_INTEGER$2 : length;

  return !!length &&
    (type == 'number' ||
      (type != 'symbol' && reIsUint.test(value))) &&
        (value > -1 && value % 1 == 0 && value < length);
}

/**
 * Performs a
 * [`SameValueZero`](http://ecma-international.org/ecma-262/7.0/#sec-samevaluezero)
 * comparison between two values to determine if they are equivalent.
 *
 * @static
 * @memberOf _
 * @since 4.0.0
 * @category Lang
 * @param {*} value The value to compare.
 * @param {*} other The other value to compare.
 * @returns {boolean} Returns `true` if the values are equivalent, else `false`.
 * @example
 *
 * var object = { 'a': 1 };
 * var other = { 'a': 1 };
 *
 * _.eq(object, object);
 * // => true
 *
 * _.eq(object, other);
 * // => false
 *
 * _.eq('a', 'a');
 * // => true
 *
 * _.eq('a', Object('a'));
 * // => false
 *
 * _.eq(NaN, NaN);
 * // => true
 */
function eq(value, other) {
  return value === other || (value !== value && other !== other);
}

/** Used as references for various `Number` constants. */
var MAX_SAFE_INTEGER$1 = 9007199254740991;

/**
 * Checks if `value` is a valid array-like length.
 *
 * **Note:** This method is loosely based on
 * [`ToLength`](http://ecma-international.org/ecma-262/7.0/#sec-tolength).
 *
 * @static
 * @memberOf _
 * @since 4.0.0
 * @category Lang
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is a valid length, else `false`.
 * @example
 *
 * _.isLength(3);
 * // => true
 *
 * _.isLength(Number.MIN_VALUE);
 * // => false
 *
 * _.isLength(Infinity);
 * // => false
 *
 * _.isLength('3');
 * // => false
 */
function isLength(value) {
  return typeof value == 'number' &&
    value > -1 && value % 1 == 0 && value <= MAX_SAFE_INTEGER$1;
}

/**
 * Checks if `value` is array-like. A value is considered array-like if it's
 * not a function and has a `value.length` that's an integer greater than or
 * equal to `0` and less than or equal to `Number.MAX_SAFE_INTEGER`.
 *
 * @static
 * @memberOf _
 * @since 4.0.0
 * @category Lang
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is array-like, else `false`.
 * @example
 *
 * _.isArrayLike([1, 2, 3]);
 * // => true
 *
 * _.isArrayLike(document.body.children);
 * // => true
 *
 * _.isArrayLike('abc');
 * // => true
 *
 * _.isArrayLike(_.noop);
 * // => false
 */
function isArrayLike(value) {
  return value != null && isLength(value.length) && !isFunction(value);
}

/**
 * Checks if the given arguments are from an iteratee call.
 *
 * @private
 * @param {*} value The potential iteratee value argument.
 * @param {*} index The potential iteratee index or key argument.
 * @param {*} object The potential iteratee object argument.
 * @returns {boolean} Returns `true` if the arguments are from an iteratee call,
 *  else `false`.
 */
function isIterateeCall(value, index, object) {
  if (!isObject(object)) {
    return false;
  }
  var type = typeof index;
  if (type == 'number'
        ? (isArrayLike(object) && isIndex(index, object.length))
        : (type == 'string' && index in object)
      ) {
    return eq(object[index], value);
  }
  return false;
}

/** Used for built-in method references. */
var objectProto$5 = Object.prototype;

/**
 * Checks if `value` is likely a prototype object.
 *
 * @private
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is a prototype, else `false`.
 */
function isPrototype(value) {
  var Ctor = value && value.constructor,
      proto = (typeof Ctor == 'function' && Ctor.prototype) || objectProto$5;

  return value === proto;
}

/**
 * The base implementation of `_.times` without support for iteratee shorthands
 * or max array length checks.
 *
 * @private
 * @param {number} n The number of times to invoke `iteratee`.
 * @param {Function} iteratee The function invoked per iteration.
 * @returns {Array} Returns the array of results.
 */
function baseTimes(n, iteratee) {
  var index = -1,
      result = Array(n);

  while (++index < n) {
    result[index] = iteratee(index);
  }
  return result;
}

/** `Object#toString` result references. */
var argsTag$1 = '[object Arguments]';

/**
 * The base implementation of `_.isArguments`.
 *
 * @private
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is an `arguments` object,
 */
function baseIsArguments(value) {
  return isObjectLike(value) && baseGetTag(value) == argsTag$1;
}

/** Used for built-in method references. */
var objectProto$4 = Object.prototype;

/** Used to check objects for own properties. */
var hasOwnProperty$4 = objectProto$4.hasOwnProperty;

/** Built-in value references. */
var propertyIsEnumerable = objectProto$4.propertyIsEnumerable;

/**
 * Checks if `value` is likely an `arguments` object.
 *
 * @static
 * @memberOf _
 * @since 0.1.0
 * @category Lang
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is an `arguments` object,
 *  else `false`.
 * @example
 *
 * _.isArguments(function() { return arguments; }());
 * // => true
 *
 * _.isArguments([1, 2, 3]);
 * // => false
 */
var isArguments = baseIsArguments(function() { return arguments; }()) ? baseIsArguments : function(value) {
  return isObjectLike(value) && hasOwnProperty$4.call(value, 'callee') &&
    !propertyIsEnumerable.call(value, 'callee');
};

var isArguments$1 = isArguments;

/**
 * This method returns `false`.
 *
 * @static
 * @memberOf _
 * @since 4.13.0
 * @category Util
 * @returns {boolean} Returns `false`.
 * @example
 *
 * _.times(2, _.stubFalse);
 * // => [false, false]
 */
function stubFalse() {
  return false;
}

/** Detect free variable `exports`. */
var freeExports$1 = typeof exports == 'object' && exports && !exports.nodeType && exports;

/** Detect free variable `module`. */
var freeModule$1 = freeExports$1 && typeof module == 'object' && module && !module.nodeType && module;

/** Detect the popular CommonJS extension `module.exports`. */
var moduleExports$1 = freeModule$1 && freeModule$1.exports === freeExports$1;

/** Built-in value references. */
var Buffer = moduleExports$1 ? root$1.Buffer : undefined;

/* Built-in method references for those with the same name as other `lodash` methods. */
var nativeIsBuffer = Buffer ? Buffer.isBuffer : undefined;

/**
 * Checks if `value` is a buffer.
 *
 * @static
 * @memberOf _
 * @since 4.3.0
 * @category Lang
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is a buffer, else `false`.
 * @example
 *
 * _.isBuffer(new Buffer(2));
 * // => true
 *
 * _.isBuffer(new Uint8Array(2));
 * // => false
 */
var isBuffer = nativeIsBuffer || stubFalse;

var isBuffer$1 = isBuffer;

/** `Object#toString` result references. */
var argsTag = '[object Arguments]',
    arrayTag = '[object Array]',
    boolTag = '[object Boolean]',
    dateTag = '[object Date]',
    errorTag = '[object Error]',
    funcTag = '[object Function]',
    mapTag = '[object Map]',
    numberTag$1 = '[object Number]',
    objectTag = '[object Object]',
    regexpTag$1 = '[object RegExp]',
    setTag = '[object Set]',
    stringTag$1 = '[object String]',
    weakMapTag = '[object WeakMap]';

var arrayBufferTag = '[object ArrayBuffer]',
    dataViewTag = '[object DataView]',
    float32Tag = '[object Float32Array]',
    float64Tag = '[object Float64Array]',
    int8Tag = '[object Int8Array]',
    int16Tag = '[object Int16Array]',
    int32Tag = '[object Int32Array]',
    uint8Tag = '[object Uint8Array]',
    uint8ClampedTag = '[object Uint8ClampedArray]',
    uint16Tag = '[object Uint16Array]',
    uint32Tag = '[object Uint32Array]';

/** Used to identify `toStringTag` values of typed arrays. */
var typedArrayTags = {};
typedArrayTags[float32Tag] = typedArrayTags[float64Tag] =
typedArrayTags[int8Tag] = typedArrayTags[int16Tag] =
typedArrayTags[int32Tag] = typedArrayTags[uint8Tag] =
typedArrayTags[uint8ClampedTag] = typedArrayTags[uint16Tag] =
typedArrayTags[uint32Tag] = true;
typedArrayTags[argsTag] = typedArrayTags[arrayTag] =
typedArrayTags[arrayBufferTag] = typedArrayTags[boolTag] =
typedArrayTags[dataViewTag] = typedArrayTags[dateTag] =
typedArrayTags[errorTag] = typedArrayTags[funcTag] =
typedArrayTags[mapTag] = typedArrayTags[numberTag$1] =
typedArrayTags[objectTag] = typedArrayTags[regexpTag$1] =
typedArrayTags[setTag] = typedArrayTags[stringTag$1] =
typedArrayTags[weakMapTag] = false;

/**
 * The base implementation of `_.isTypedArray` without Node.js optimizations.
 *
 * @private
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is a typed array, else `false`.
 */
function baseIsTypedArray(value) {
  return isObjectLike(value) &&
    isLength(value.length) && !!typedArrayTags[baseGetTag(value)];
}

/**
 * The base implementation of `_.unary` without support for storing metadata.
 *
 * @private
 * @param {Function} func The function to cap arguments for.
 * @returns {Function} Returns the new capped function.
 */
function baseUnary(func) {
  return function(value) {
    return func(value);
  };
}

/** Detect free variable `exports`. */
var freeExports = typeof exports == 'object' && exports && !exports.nodeType && exports;

/** Detect free variable `module`. */
var freeModule = freeExports && typeof module == 'object' && module && !module.nodeType && module;

/** Detect the popular CommonJS extension `module.exports`. */
var moduleExports = freeModule && freeModule.exports === freeExports;

/** Detect free variable `process` from Node.js. */
var freeProcess = moduleExports && freeGlobal$1.process;

/** Used to access faster Node.js helpers. */
var nodeUtil = (function() {
  try {
    // Use `util.types` for Node.js 10+.
    var types = freeModule && freeModule.require && freeModule.require('util').types;

    if (types) {
      return types;
    }

    // Legacy `process.binding('util')` for Node.js < 10.
    return freeProcess && freeProcess.binding && freeProcess.binding('util');
  } catch (e) {}
}());

var nodeUtil$1 = nodeUtil;

/* Node.js helper references. */
var nodeIsTypedArray = nodeUtil$1 && nodeUtil$1.isTypedArray;

/**
 * Checks if `value` is classified as a typed array.
 *
 * @static
 * @memberOf _
 * @since 3.0.0
 * @category Lang
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is a typed array, else `false`.
 * @example
 *
 * _.isTypedArray(new Uint8Array);
 * // => true
 *
 * _.isTypedArray([]);
 * // => false
 */
var isTypedArray = nodeIsTypedArray ? baseUnary(nodeIsTypedArray) : baseIsTypedArray;

var isTypedArray$1 = isTypedArray;

/** Used for built-in method references. */
var objectProto$3 = Object.prototype;

/** Used to check objects for own properties. */
var hasOwnProperty$3 = objectProto$3.hasOwnProperty;

/**
 * Creates an array of the enumerable property names of the array-like `value`.
 *
 * @private
 * @param {*} value The value to query.
 * @param {boolean} inherited Specify returning inherited property names.
 * @returns {Array} Returns the array of property names.
 */
function arrayLikeKeys(value, inherited) {
  var isArr = isArray$1(value),
      isArg = !isArr && isArguments$1(value),
      isBuff = !isArr && !isArg && isBuffer$1(value),
      isType = !isArr && !isArg && !isBuff && isTypedArray$1(value),
      skipIndexes = isArr || isArg || isBuff || isType,
      result = skipIndexes ? baseTimes(value.length, String) : [],
      length = result.length;

  for (var key in value) {
    if ((inherited || hasOwnProperty$3.call(value, key)) &&
        !(skipIndexes && (
           // Safari 9 has enumerable `arguments.length` in strict mode.
           key == 'length' ||
           // Node.js 0.10 has enumerable non-index properties on buffers.
           (isBuff && (key == 'offset' || key == 'parent')) ||
           // PhantomJS 2 has enumerable non-index properties on typed arrays.
           (isType && (key == 'buffer' || key == 'byteLength' || key == 'byteOffset')) ||
           // Skip index properties.
           isIndex(key, length)
        ))) {
      result.push(key);
    }
  }
  return result;
}

/**
 * Creates a unary function that invokes `func` with its argument transformed.
 *
 * @private
 * @param {Function} func The function to wrap.
 * @param {Function} transform The argument transform.
 * @returns {Function} Returns the new function.
 */
function overArg(func, transform) {
  return function(arg) {
    return func(transform(arg));
  };
}

/* Built-in method references for those with the same name as other `lodash` methods. */
var nativeKeys = overArg(Object.keys, Object);

var nativeKeys$1 = nativeKeys;

/** Used for built-in method references. */
var objectProto$2 = Object.prototype;

/** Used to check objects for own properties. */
var hasOwnProperty$2 = objectProto$2.hasOwnProperty;

/**
 * The base implementation of `_.keys` which doesn't treat sparse arrays as dense.
 *
 * @private
 * @param {Object} object The object to query.
 * @returns {Array} Returns the array of property names.
 */
function baseKeys(object) {
  if (!isPrototype(object)) {
    return nativeKeys$1(object);
  }
  var result = [];
  for (var key in Object(object)) {
    if (hasOwnProperty$2.call(object, key) && key != 'constructor') {
      result.push(key);
    }
  }
  return result;
}

/**
 * Creates an array of the own enumerable property names of `object`.
 *
 * **Note:** Non-object values are coerced to objects. See the
 * [ES spec](http://ecma-international.org/ecma-262/7.0/#sec-object.keys)
 * for more details.
 *
 * @static
 * @since 0.1.0
 * @memberOf _
 * @category Object
 * @param {Object} object The object to query.
 * @returns {Array} Returns the array of property names.
 * @example
 *
 * function Foo() {
 *   this.a = 1;
 *   this.b = 2;
 * }
 *
 * Foo.prototype.c = 3;
 *
 * _.keys(new Foo);
 * // => ['a', 'b'] (iteration order is not guaranteed)
 *
 * _.keys('hi');
 * // => ['0', '1']
 */
function keys$1(object) {
  return isArrayLike(object) ? arrayLikeKeys(object) : baseKeys(object);
}

/** Used to match property names within property paths. */
var reIsDeepProp = /\.|\[(?:[^[\]]*|(["'])(?:(?!\1)[^\\]|\\.)*?\1)\]/,
    reIsPlainProp = /^\w*$/;

/**
 * Checks if `value` is a property name and not a property path.
 *
 * @private
 * @param {*} value The value to check.
 * @param {Object} [object] The object to query keys on.
 * @returns {boolean} Returns `true` if `value` is a property name, else `false`.
 */
function isKey(value, object) {
  if (isArray$1(value)) {
    return false;
  }
  var type = typeof value;
  if (type == 'number' || type == 'symbol' || type == 'boolean' ||
      value == null || isSymbol(value)) {
    return true;
  }
  return reIsPlainProp.test(value) || !reIsDeepProp.test(value) ||
    (object != null && value in Object(object));
}

/* Built-in method references that are verified to be native. */
var nativeCreate = getNative(Object, 'create');

var nativeCreate$1 = nativeCreate;

/**
 * Removes all key-value entries from the hash.
 *
 * @private
 * @name clear
 * @memberOf Hash
 */
function hashClear() {
  this.__data__ = nativeCreate$1 ? nativeCreate$1(null) : {};
  this.size = 0;
}

/**
 * Removes `key` and its value from the hash.
 *
 * @private
 * @name delete
 * @memberOf Hash
 * @param {Object} hash The hash to modify.
 * @param {string} key The key of the value to remove.
 * @returns {boolean} Returns `true` if the entry was removed, else `false`.
 */
function hashDelete(key) {
  var result = this.has(key) && delete this.__data__[key];
  this.size -= result ? 1 : 0;
  return result;
}

/** Used to stand-in for `undefined` hash values. */
var HASH_UNDEFINED$1 = '__lodash_hash_undefined__';

/** Used for built-in method references. */
var objectProto$1 = Object.prototype;

/** Used to check objects for own properties. */
var hasOwnProperty$1 = objectProto$1.hasOwnProperty;

/**
 * Gets the hash value for `key`.
 *
 * @private
 * @name get
 * @memberOf Hash
 * @param {string} key The key of the value to get.
 * @returns {*} Returns the entry value.
 */
function hashGet(key) {
  var data = this.__data__;
  if (nativeCreate$1) {
    var result = data[key];
    return result === HASH_UNDEFINED$1 ? undefined : result;
  }
  return hasOwnProperty$1.call(data, key) ? data[key] : undefined;
}

/** Used for built-in method references. */
var objectProto = Object.prototype;

/** Used to check objects for own properties. */
var hasOwnProperty = objectProto.hasOwnProperty;

/**
 * Checks if a hash value for `key` exists.
 *
 * @private
 * @name has
 * @memberOf Hash
 * @param {string} key The key of the entry to check.
 * @returns {boolean} Returns `true` if an entry for `key` exists, else `false`.
 */
function hashHas(key) {
  var data = this.__data__;
  return nativeCreate$1 ? (data[key] !== undefined) : hasOwnProperty.call(data, key);
}

/** Used to stand-in for `undefined` hash values. */
var HASH_UNDEFINED = '__lodash_hash_undefined__';

/**
 * Sets the hash `key` to `value`.
 *
 * @private
 * @name set
 * @memberOf Hash
 * @param {string} key The key of the value to set.
 * @param {*} value The value to set.
 * @returns {Object} Returns the hash instance.
 */
function hashSet(key, value) {
  var data = this.__data__;
  this.size += this.has(key) ? 0 : 1;
  data[key] = (nativeCreate$1 && value === undefined) ? HASH_UNDEFINED : value;
  return this;
}

/**
 * Creates a hash object.
 *
 * @private
 * @constructor
 * @param {Array} [entries] The key-value pairs to cache.
 */
function Hash(entries) {
  var index = -1,
      length = entries == null ? 0 : entries.length;

  this.clear();
  while (++index < length) {
    var entry = entries[index];
    this.set(entry[0], entry[1]);
  }
}

// Add methods to `Hash`.
Hash.prototype.clear = hashClear;
Hash.prototype['delete'] = hashDelete;
Hash.prototype.get = hashGet;
Hash.prototype.has = hashHas;
Hash.prototype.set = hashSet;

/**
 * Removes all key-value entries from the list cache.
 *
 * @private
 * @name clear
 * @memberOf ListCache
 */
function listCacheClear() {
  this.__data__ = [];
  this.size = 0;
}

/**
 * Gets the index at which the `key` is found in `array` of key-value pairs.
 *
 * @private
 * @param {Array} array The array to inspect.
 * @param {*} key The key to search for.
 * @returns {number} Returns the index of the matched value, else `-1`.
 */
function assocIndexOf(array, key) {
  var length = array.length;
  while (length--) {
    if (eq(array[length][0], key)) {
      return length;
    }
  }
  return -1;
}

/** Used for built-in method references. */
var arrayProto = Array.prototype;

/** Built-in value references. */
var splice = arrayProto.splice;

/**
 * Removes `key` and its value from the list cache.
 *
 * @private
 * @name delete
 * @memberOf ListCache
 * @param {string} key The key of the value to remove.
 * @returns {boolean} Returns `true` if the entry was removed, else `false`.
 */
function listCacheDelete(key) {
  var data = this.__data__,
      index = assocIndexOf(data, key);

  if (index < 0) {
    return false;
  }
  var lastIndex = data.length - 1;
  if (index == lastIndex) {
    data.pop();
  } else {
    splice.call(data, index, 1);
  }
  --this.size;
  return true;
}

/**
 * Gets the list cache value for `key`.
 *
 * @private
 * @name get
 * @memberOf ListCache
 * @param {string} key The key of the value to get.
 * @returns {*} Returns the entry value.
 */
function listCacheGet(key) {
  var data = this.__data__,
      index = assocIndexOf(data, key);

  return index < 0 ? undefined : data[index][1];
}

/**
 * Checks if a list cache value for `key` exists.
 *
 * @private
 * @name has
 * @memberOf ListCache
 * @param {string} key The key of the entry to check.
 * @returns {boolean} Returns `true` if an entry for `key` exists, else `false`.
 */
function listCacheHas(key) {
  return assocIndexOf(this.__data__, key) > -1;
}

/**
 * Sets the list cache `key` to `value`.
 *
 * @private
 * @name set
 * @memberOf ListCache
 * @param {string} key The key of the value to set.
 * @param {*} value The value to set.
 * @returns {Object} Returns the list cache instance.
 */
function listCacheSet(key, value) {
  var data = this.__data__,
      index = assocIndexOf(data, key);

  if (index < 0) {
    ++this.size;
    data.push([key, value]);
  } else {
    data[index][1] = value;
  }
  return this;
}

/**
 * Creates an list cache object.
 *
 * @private
 * @constructor
 * @param {Array} [entries] The key-value pairs to cache.
 */
function ListCache(entries) {
  var index = -1,
      length = entries == null ? 0 : entries.length;

  this.clear();
  while (++index < length) {
    var entry = entries[index];
    this.set(entry[0], entry[1]);
  }
}

// Add methods to `ListCache`.
ListCache.prototype.clear = listCacheClear;
ListCache.prototype['delete'] = listCacheDelete;
ListCache.prototype.get = listCacheGet;
ListCache.prototype.has = listCacheHas;
ListCache.prototype.set = listCacheSet;

/* Built-in method references that are verified to be native. */
var Map$1 = getNative(root$1, 'Map');

var Map$2 = Map$1;

/**
 * Removes all key-value entries from the map.
 *
 * @private
 * @name clear
 * @memberOf MapCache
 */
function mapCacheClear() {
  this.size = 0;
  this.__data__ = {
    'hash': new Hash,
    'map': new (Map$2 || ListCache),
    'string': new Hash
  };
}

/**
 * Checks if `value` is suitable for use as unique object key.
 *
 * @private
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is suitable, else `false`.
 */
function isKeyable(value) {
  var type = typeof value;
  return (type == 'string' || type == 'number' || type == 'symbol' || type == 'boolean')
    ? (value !== '__proto__')
    : (value === null);
}

/**
 * Gets the data for `map`.
 *
 * @private
 * @param {Object} map The map to query.
 * @param {string} key The reference key.
 * @returns {*} Returns the map data.
 */
function getMapData(map, key) {
  var data = map.__data__;
  return isKeyable(key)
    ? data[typeof key == 'string' ? 'string' : 'hash']
    : data.map;
}

/**
 * Removes `key` and its value from the map.
 *
 * @private
 * @name delete
 * @memberOf MapCache
 * @param {string} key The key of the value to remove.
 * @returns {boolean} Returns `true` if the entry was removed, else `false`.
 */
function mapCacheDelete(key) {
  var result = getMapData(this, key)['delete'](key);
  this.size -= result ? 1 : 0;
  return result;
}

/**
 * Gets the map value for `key`.
 *
 * @private
 * @name get
 * @memberOf MapCache
 * @param {string} key The key of the value to get.
 * @returns {*} Returns the entry value.
 */
function mapCacheGet(key) {
  return getMapData(this, key).get(key);
}

/**
 * Checks if a map value for `key` exists.
 *
 * @private
 * @name has
 * @memberOf MapCache
 * @param {string} key The key of the entry to check.
 * @returns {boolean} Returns `true` if an entry for `key` exists, else `false`.
 */
function mapCacheHas(key) {
  return getMapData(this, key).has(key);
}

/**
 * Sets the map `key` to `value`.
 *
 * @private
 * @name set
 * @memberOf MapCache
 * @param {string} key The key of the value to set.
 * @param {*} value The value to set.
 * @returns {Object} Returns the map cache instance.
 */
function mapCacheSet(key, value) {
  var data = getMapData(this, key),
      size = data.size;

  data.set(key, value);
  this.size += data.size == size ? 0 : 1;
  return this;
}

/**
 * Creates a map cache object to store key-value pairs.
 *
 * @private
 * @constructor
 * @param {Array} [entries] The key-value pairs to cache.
 */
function MapCache(entries) {
  var index = -1,
      length = entries == null ? 0 : entries.length;

  this.clear();
  while (++index < length) {
    var entry = entries[index];
    this.set(entry[0], entry[1]);
  }
}

// Add methods to `MapCache`.
MapCache.prototype.clear = mapCacheClear;
MapCache.prototype['delete'] = mapCacheDelete;
MapCache.prototype.get = mapCacheGet;
MapCache.prototype.has = mapCacheHas;
MapCache.prototype.set = mapCacheSet;

/** Error message constants. */
var FUNC_ERROR_TEXT = 'Expected a function';

/**
 * Creates a function that memoizes the result of `func`. If `resolver` is
 * provided, it determines the cache key for storing the result based on the
 * arguments provided to the memoized function. By default, the first argument
 * provided to the memoized function is used as the map cache key. The `func`
 * is invoked with the `this` binding of the memoized function.
 *
 * **Note:** The cache is exposed as the `cache` property on the memoized
 * function. Its creation may be customized by replacing the `_.memoize.Cache`
 * constructor with one whose instances implement the
 * [`Map`](http://ecma-international.org/ecma-262/7.0/#sec-properties-of-the-map-prototype-object)
 * method interface of `clear`, `delete`, `get`, `has`, and `set`.
 *
 * @static
 * @memberOf _
 * @since 0.1.0
 * @category Function
 * @param {Function} func The function to have its output memoized.
 * @param {Function} [resolver] The function to resolve the cache key.
 * @returns {Function} Returns the new memoized function.
 * @example
 *
 * var object = { 'a': 1, 'b': 2 };
 * var other = { 'c': 3, 'd': 4 };
 *
 * var values = _.memoize(_.values);
 * values(object);
 * // => [1, 2]
 *
 * values(other);
 * // => [3, 4]
 *
 * object.a = 2;
 * values(object);
 * // => [1, 2]
 *
 * // Modify the result cache.
 * values.cache.set(object, ['a', 'b']);
 * values(object);
 * // => ['a', 'b']
 *
 * // Replace `_.memoize.Cache`.
 * _.memoize.Cache = WeakMap;
 */
function memoize(func, resolver) {
  if (typeof func != 'function' || (resolver != null && typeof resolver != 'function')) {
    throw new TypeError(FUNC_ERROR_TEXT);
  }
  var memoized = function() {
    var args = arguments,
        key = resolver ? resolver.apply(this, args) : args[0],
        cache = memoized.cache;

    if (cache.has(key)) {
      return cache.get(key);
    }
    var result = func.apply(this, args);
    memoized.cache = cache.set(key, result) || cache;
    return result;
  };
  memoized.cache = new (memoize.Cache || MapCache);
  return memoized;
}

// Expose `MapCache`.
memoize.Cache = MapCache;

/** Used as the maximum memoize cache size. */
var MAX_MEMOIZE_SIZE = 500;

/**
 * A specialized version of `_.memoize` which clears the memoized function's
 * cache when it exceeds `MAX_MEMOIZE_SIZE`.
 *
 * @private
 * @param {Function} func The function to have its output memoized.
 * @returns {Function} Returns the new memoized function.
 */
function memoizeCapped(func) {
  var result = memoize(func, function(key) {
    if (cache.size === MAX_MEMOIZE_SIZE) {
      cache.clear();
    }
    return key;
  });

  var cache = result.cache;
  return result;
}

/** Used to match property names within property paths. */
var rePropName = /[^.[\]]+|\[(?:(-?\d+(?:\.\d+)?)|(["'])((?:(?!\2)[^\\]|\\.)*?)\2)\]|(?=(?:\.|\[\])(?:\.|\[\]|$))/g;

/** Used to match backslashes in property paths. */
var reEscapeChar = /\\(\\)?/g;

/**
 * Converts `string` to a property path array.
 *
 * @private
 * @param {string} string The string to convert.
 * @returns {Array} Returns the property path array.
 */
var stringToPath = memoizeCapped(function(string) {
  var result = [];
  if (string.charCodeAt(0) === 46 /* . */) {
    result.push('');
  }
  string.replace(rePropName, function(match, number, quote, subString) {
    result.push(quote ? subString.replace(reEscapeChar, '$1') : (number || match));
  });
  return result;
});

var stringToPath$1 = stringToPath;

/**
 * Converts `value` to a string. An empty string is returned for `null`
 * and `undefined` values. The sign of `-0` is preserved.
 *
 * @static
 * @memberOf _
 * @since 4.0.0
 * @category Lang
 * @param {*} value The value to convert.
 * @returns {string} Returns the converted string.
 * @example
 *
 * _.toString(null);
 * // => ''
 *
 * _.toString(-0);
 * // => '-0'
 *
 * _.toString([1, 2, 3]);
 * // => '1,2,3'
 */
function toString(value) {
  return value == null ? '' : baseToString(value);
}

/**
 * Casts `value` to a path array if it's not one.
 *
 * @private
 * @param {*} value The value to inspect.
 * @param {Object} [object] The object to query keys on.
 * @returns {Array} Returns the cast property path array.
 */
function castPath(value, object) {
  if (isArray$1(value)) {
    return value;
  }
  return isKey(value, object) ? [value] : stringToPath$1(toString(value));
}

/** Used as references for various `Number` constants. */
var INFINITY = 1 / 0;

/**
 * Converts `value` to a string key if it's not a string or symbol.
 *
 * @private
 * @param {*} value The value to inspect.
 * @returns {string|symbol} Returns the key.
 */
function toKey(value) {
  if (typeof value == 'string' || isSymbol(value)) {
    return value;
  }
  var result = (value + '');
  return (result == '0' && (1 / value) == -INFINITY) ? '-0' : result;
}

/**
 * The base implementation of `_.get` without support for default values.
 *
 * @private
 * @param {Object} object The object to query.
 * @param {Array|string} path The path of the property to get.
 * @returns {*} Returns the resolved value.
 */
function baseGet(object, path) {
  path = castPath(path, object);

  var index = 0,
      length = path.length;

  while (object != null && index < length) {
    object = object[toKey(path[index++])];
  }
  return (index && index == length) ? object : undefined;
}

/**
 * Gets the value at `path` of `object`. If the resolved value is
 * `undefined`, the `defaultValue` is returned in its place.
 *
 * @static
 * @memberOf _
 * @since 3.7.0
 * @category Object
 * @param {Object} object The object to query.
 * @param {Array|string} path The path of the property to get.
 * @param {*} [defaultValue] The value returned for `undefined` resolved values.
 * @returns {*} Returns the resolved value.
 * @example
 *
 * var object = { 'a': [{ 'b': { 'c': 3 } }] };
 *
 * _.get(object, 'a[0].b.c');
 * // => 3
 *
 * _.get(object, ['a', '0', 'b', 'c']);
 * // => 3
 *
 * _.get(object, 'a.b.c', 'default');
 * // => 'default'
 */
function get(object, path, defaultValue) {
  var result = object == null ? undefined : baseGet(object, path);
  return result === undefined ? defaultValue : result;
}

/**
 * The base implementation of `_.slice` without an iteratee call guard.
 *
 * @private
 * @param {Array} array The array to slice.
 * @param {number} [start=0] The start position.
 * @param {number} [end=array.length] The end position.
 * @returns {Array} Returns the slice of `array`.
 */
function baseSlice(array, start, end) {
  var index = -1,
      length = array.length;

  if (start < 0) {
    start = -start > length ? 0 : (length + start);
  }
  end = end > length ? length : end;
  if (end < 0) {
    end += length;
  }
  length = start > end ? 0 : ((end - start) >>> 0);
  start >>>= 0;

  var result = Array(length);
  while (++index < length) {
    result[index] = array[index + start];
  }
  return result;
}

/**
 * Casts `array` to a slice if it's needed.
 *
 * @private
 * @param {Array} array The array to inspect.
 * @param {number} start The start position.
 * @param {number} [end=array.length] The end position.
 * @returns {Array} Returns the cast slice.
 */
function castSlice(array, start, end) {
  var length = array.length;
  end = end === undefined ? length : end;
  return (!start && end >= length) ? array : baseSlice(array, start, end);
}

/** Used to compose unicode character classes. */
var rsAstralRange$1 = '\\ud800-\\udfff',
    rsComboMarksRange$1 = '\\u0300-\\u036f',
    reComboHalfMarksRange$1 = '\\ufe20-\\ufe2f',
    rsComboSymbolsRange$1 = '\\u20d0-\\u20ff',
    rsComboRange$1 = rsComboMarksRange$1 + reComboHalfMarksRange$1 + rsComboSymbolsRange$1,
    rsVarRange$1 = '\\ufe0e\\ufe0f';

/** Used to compose unicode capture groups. */
var rsZWJ$1 = '\\u200d';

/** Used to detect strings with [zero-width joiners or code points from the astral planes](http://eev.ee/blog/2015/09/12/dark-corners-of-unicode/). */
var reHasUnicode = RegExp('[' + rsZWJ$1 + rsAstralRange$1  + rsComboRange$1 + rsVarRange$1 + ']');

/**
 * Checks if `string` contains Unicode symbols.
 *
 * @private
 * @param {string} string The string to inspect.
 * @returns {boolean} Returns `true` if a symbol is found, else `false`.
 */
function hasUnicode(string) {
  return reHasUnicode.test(string);
}

/**
 * Converts an ASCII `string` to an array.
 *
 * @private
 * @param {string} string The string to convert.
 * @returns {Array} Returns the converted array.
 */
function asciiToArray(string) {
  return string.split('');
}

/** Used to compose unicode character classes. */
var rsAstralRange = '\\ud800-\\udfff',
    rsComboMarksRange = '\\u0300-\\u036f',
    reComboHalfMarksRange = '\\ufe20-\\ufe2f',
    rsComboSymbolsRange = '\\u20d0-\\u20ff',
    rsComboRange = rsComboMarksRange + reComboHalfMarksRange + rsComboSymbolsRange,
    rsVarRange = '\\ufe0e\\ufe0f';

/** Used to compose unicode capture groups. */
var rsAstral = '[' + rsAstralRange + ']',
    rsCombo = '[' + rsComboRange + ']',
    rsFitz = '\\ud83c[\\udffb-\\udfff]',
    rsModifier = '(?:' + rsCombo + '|' + rsFitz + ')',
    rsNonAstral = '[^' + rsAstralRange + ']',
    rsRegional = '(?:\\ud83c[\\udde6-\\uddff]){2}',
    rsSurrPair = '[\\ud800-\\udbff][\\udc00-\\udfff]',
    rsZWJ = '\\u200d';

/** Used to compose unicode regexes. */
var reOptMod = rsModifier + '?',
    rsOptVar = '[' + rsVarRange + ']?',
    rsOptJoin = '(?:' + rsZWJ + '(?:' + [rsNonAstral, rsRegional, rsSurrPair].join('|') + ')' + rsOptVar + reOptMod + ')*',
    rsSeq = rsOptVar + reOptMod + rsOptJoin,
    rsSymbol = '(?:' + [rsNonAstral + rsCombo + '?', rsCombo, rsRegional, rsSurrPair, rsAstral].join('|') + ')';

/** Used to match [string symbols](https://mathiasbynens.be/notes/javascript-unicode). */
var reUnicode = RegExp(rsFitz + '(?=' + rsFitz + ')|' + rsSymbol + rsSeq, 'g');

/**
 * Converts a Unicode `string` to an array.
 *
 * @private
 * @param {string} string The string to convert.
 * @returns {Array} Returns the converted array.
 */
function unicodeToArray(string) {
  return string.match(reUnicode) || [];
}

/**
 * Converts `string` to an array.
 *
 * @private
 * @param {string} string The string to convert.
 * @returns {Array} Returns the converted array.
 */
function stringToArray(string) {
  return hasUnicode(string)
    ? unicodeToArray(string)
    : asciiToArray(string);
}

/**
 * Creates a function like `_.lowerFirst`.
 *
 * @private
 * @param {string} methodName The name of the `String` case method to use.
 * @returns {Function} Returns the new case function.
 */
function createCaseFirst(methodName) {
  return function(string) {
    string = toString(string);

    var strSymbols = hasUnicode(string)
      ? stringToArray(string)
      : undefined;

    var chr = strSymbols
      ? strSymbols[0]
      : string.charAt(0);

    var trailing = strSymbols
      ? castSlice(strSymbols, 1).join('')
      : string.slice(1);

    return chr[methodName]() + trailing;
  };
}

/**
 * Converts the first character of `string` to upper case.
 *
 * @static
 * @memberOf _
 * @since 4.0.0
 * @category String
 * @param {string} [string=''] The string to convert.
 * @returns {string} Returns the converted string.
 * @example
 *
 * _.upperFirst('fred');
 * // => 'Fred'
 *
 * _.upperFirst('FRED');
 * // => 'FRED'
 */
var upperFirst = createCaseFirst('toUpperCase');

var upperFirst$1 = upperFirst;

/**
 * Converts the first character of `string` to upper case and the remaining
 * to lower case.
 *
 * @static
 * @memberOf _
 * @since 3.0.0
 * @category String
 * @param {string} [string=''] The string to capitalize.
 * @returns {string} Returns the capitalized string.
 * @example
 *
 * _.capitalize('FRED');
 * // => 'Fred'
 */
function capitalize(string) {
  return upperFirst$1(toString(string).toLowerCase());
}

/**
 * The base implementation of `_.clamp` which doesn't coerce arguments.
 *
 * @private
 * @param {number} number The number to clamp.
 * @param {number} [lower] The lower bound.
 * @param {number} upper The upper bound.
 * @returns {number} Returns the clamped number.
 */
function baseClamp(number, lower, upper) {
  if (number === number) {
    if (upper !== undefined) {
      number = number <= upper ? number : upper;
    }
    if (lower !== undefined) {
      number = number >= lower ? number : lower;
    }
  }
  return number;
}

/**
 * This method is like `_.isArrayLike` except that it also checks if `value`
 * is an object.
 *
 * @static
 * @memberOf _
 * @since 4.0.0
 * @category Lang
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is an array-like object,
 *  else `false`.
 * @example
 *
 * _.isArrayLikeObject([1, 2, 3]);
 * // => true
 *
 * _.isArrayLikeObject(document.body.children);
 * // => true
 *
 * _.isArrayLikeObject('abc');
 * // => false
 *
 * _.isArrayLikeObject(_.noop);
 * // => false
 */
function isArrayLikeObject(value) {
  return isObjectLike(value) && isArrayLike(value);
}

/**
 * Casts `value` to `identity` if it's not a function.
 *
 * @private
 * @param {*} value The value to inspect.
 * @returns {Function} Returns cast function.
 */
function castFunction(value) {
  return typeof value == 'function' ? value : identity;
}

/** `Object#toString` result references. */
var stringTag = '[object String]';

/**
 * Checks if `value` is classified as a `String` primitive or object.
 *
 * @static
 * @since 0.1.0
 * @memberOf _
 * @category Lang
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is a string, else `false`.
 * @example
 *
 * _.isString('abc');
 * // => true
 *
 * _.isString(1);
 * // => false
 */
function isString(value) {
  return typeof value == 'string' ||
    (!isArray$1(value) && isObjectLike(value) && baseGetTag(value) == stringTag);
}

/**
 * The base implementation of `_.values` and `_.valuesIn` which creates an
 * array of `object` property values corresponding to the property names
 * of `props`.
 *
 * @private
 * @param {Object} object The object to query.
 * @param {Array} props The property names to get values for.
 * @returns {Object} Returns the array of property values.
 */
function baseValues(object, props) {
  return arrayMap(props, function(key) {
    return object[key];
  });
}

/**
 * Creates an array of the own enumerable string keyed property values of `object`.
 *
 * **Note:** Non-object values are coerced to objects.
 *
 * @static
 * @since 0.1.0
 * @memberOf _
 * @category Object
 * @param {Object} object The object to query.
 * @returns {Array} Returns the array of property values.
 * @example
 *
 * function Foo() {
 *   this.a = 1;
 *   this.b = 2;
 * }
 *
 * Foo.prototype.c = 3;
 *
 * _.values(new Foo);
 * // => [1, 2] (iteration order is not guaranteed)
 *
 * _.values('hi');
 * // => ['h', 'i']
 */
function values$1(object) {
  return object == null ? [] : baseValues(object, keys$1(object));
}

/** `Object#toString` result references. */
var numberTag = '[object Number]';

/**
 * Checks if `value` is classified as a `Number` primitive or object.
 *
 * **Note:** To exclude `Infinity`, `-Infinity`, and `NaN`, which are
 * classified as numbers, use the `_.isFinite` method.
 *
 * @static
 * @memberOf _
 * @since 0.1.0
 * @category Lang
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is a number, else `false`.
 * @example
 *
 * _.isNumber(3);
 * // => true
 *
 * _.isNumber(Number.MIN_VALUE);
 * // => true
 *
 * _.isNumber(Infinity);
 * // => true
 *
 * _.isNumber('3');
 * // => false
 */
function isNumber(value) {
  return typeof value == 'number' ||
    (isObjectLike(value) && baseGetTag(value) == numberTag);
}

/** `Object#toString` result references. */
var regexpTag = '[object RegExp]';

/**
 * The base implementation of `_.isRegExp` without Node.js optimizations.
 *
 * @private
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is a regexp, else `false`.
 */
function baseIsRegExp(value) {
  return isObjectLike(value) && baseGetTag(value) == regexpTag;
}

/* Node.js helper references. */
var nodeIsRegExp = nodeUtil$1 && nodeUtil$1.isRegExp;

/**
 * Checks if `value` is classified as a `RegExp` object.
 *
 * @static
 * @memberOf _
 * @since 0.1.0
 * @category Lang
 * @param {*} value The value to check.
 * @returns {boolean} Returns `true` if `value` is a regexp, else `false`.
 * @example
 *
 * _.isRegExp(/abc/);
 * // => true
 *
 * _.isRegExp('/abc/');
 * // => false
 */
var isRegExp = nodeIsRegExp ? baseUnary(nodeIsRegExp) : baseIsRegExp;

var isRegExp$1 = isRegExp;

/* Built-in method references for those with the same name as other `lodash` methods. */
var nativeFloor = Math.floor,
    nativeRandom$1 = Math.random;

/**
 * The base implementation of `_.random` without support for returning
 * floating-point numbers.
 *
 * @private
 * @param {number} lower The lower bound.
 * @param {number} upper The upper bound.
 * @returns {number} Returns the random number.
 */
function baseRandom(lower, upper) {
  return lower + nativeFloor(nativeRandom$1() * (upper - lower + 1));
}

/** Built-in method references without a dependency on `root`. */
var freeParseFloat = parseFloat;

/* Built-in method references for those with the same name as other `lodash` methods. */
var nativeMin$1 = Math.min,
    nativeRandom = Math.random;

/**
 * Produces a random number between the inclusive `lower` and `upper` bounds.
 * If only one argument is provided a number between `0` and the given number
 * is returned. If `floating` is `true`, or either `lower` or `upper` are
 * floats, a floating-point number is returned instead of an integer.
 *
 * **Note:** JavaScript follows the IEEE-754 standard for resolving
 * floating-point values which can produce unexpected results.
 *
 * @static
 * @memberOf _
 * @since 0.7.0
 * @category Number
 * @param {number} [lower=0] The lower bound.
 * @param {number} [upper=1] The upper bound.
 * @param {boolean} [floating] Specify returning a floating-point number.
 * @returns {number} Returns the random number.
 * @example
 *
 * _.random(0, 5);
 * // => an integer between 0 and 5
 *
 * _.random(5);
 * // => also an integer between 0 and 5
 *
 * _.random(5, true);
 * // => a floating-point number between 0 and 5
 *
 * _.random(1.2, 5.2);
 * // => a floating-point number between 1.2 and 5.2
 */
function random$1(lower, upper, floating) {
  if (floating && typeof floating != 'boolean' && isIterateeCall(lower, upper, floating)) {
    upper = floating = undefined;
  }
  if (floating === undefined) {
    if (typeof upper == 'boolean') {
      floating = upper;
      upper = undefined;
    }
    else if (typeof lower == 'boolean') {
      floating = lower;
      lower = undefined;
    }
  }
  if (lower === undefined && upper === undefined) {
    lower = 0;
    upper = 1;
  }
  else {
    lower = toFinite(lower);
    if (upper === undefined) {
      upper = lower;
      lower = 0;
    } else {
      upper = toFinite(upper);
    }
  }
  if (lower > upper) {
    var temp = lower;
    lower = upper;
    upper = temp;
  }
  if (floating || lower % 1 || upper % 1) {
    var rand = nativeRandom();
    return nativeMin$1(lower + (rand * (upper - lower + freeParseFloat('1e-' + ((rand + '').length - 1)))), upper);
  }
  return baseRandom(lower, upper);
}

/* Built-in method references for those with the same name as other `lodash` methods. */
var nativeCeil = Math.ceil,
    nativeMax = Math.max;

/**
 * The base implementation of `_.range` and `_.rangeRight` which doesn't
 * coerce arguments.
 *
 * @private
 * @param {number} start The start of the range.
 * @param {number} end The end of the range.
 * @param {number} step The value to increment or decrement by.
 * @param {boolean} [fromRight] Specify iterating from right to left.
 * @returns {Array} Returns the range of numbers.
 */
function baseRange(start, end, step, fromRight) {
  var index = -1,
      length = nativeMax(nativeCeil((end - start) / (step || 1)), 0),
      result = Array(length);

  while (length--) {
    result[fromRight ? length : ++index] = start;
    start += step;
  }
  return result;
}

/**
 * Creates a `_.range` or `_.rangeRight` function.
 *
 * @private
 * @param {boolean} [fromRight] Specify iterating from right to left.
 * @returns {Function} Returns the new range function.
 */
function createRange(fromRight) {
  return function(start, end, step) {
    if (step && typeof step != 'number' && isIterateeCall(start, end, step)) {
      end = step = undefined;
    }
    // Ensure the sign of `-0` is preserved.
    start = toFinite(start);
    if (end === undefined) {
      end = start;
      start = 0;
    } else {
      end = toFinite(end);
    }
    step = step === undefined ? (start < end ? 1 : -1) : toFinite(step);
    return baseRange(start, end, step, fromRight);
  };
}

/**
 * Creates an array of numbers (positive and/or negative) progressing from
 * `start` up to, but not including, `end`. A step of `-1` is used if a negative
 * `start` is specified without an `end` or `step`. If `end` is not specified,
 * it's set to `start` with `start` then set to `0`.
 *
 * **Note:** JavaScript follows the IEEE-754 standard for resolving
 * floating-point values which can produce unexpected results.
 *
 * @static
 * @since 0.1.0
 * @memberOf _
 * @category Util
 * @param {number} [start=0] The start of the range.
 * @param {number} end The end of the range.
 * @param {number} [step=1] The value to increment or decrement by.
 * @returns {Array} Returns the range of numbers.
 * @see _.inRange, _.rangeRight
 * @example
 *
 * _.range(4);
 * // => [0, 1, 2, 3]
 *
 * _.range(-4);
 * // => [0, -1, -2, -3]
 *
 * _.range(1, 5);
 * // => [1, 2, 3, 4]
 *
 * _.range(0, 20, 5);
 * // => [0, 5, 10, 15]
 *
 * _.range(0, -4, -1);
 * // => [0, -1, -2, -3]
 *
 * _.range(1, 4, 0);
 * // => [1, 1, 1]
 *
 * _.range(0);
 * // => []
 */
var range$1 = createRange();

var _range = range$1;

/**
 * A specialized version of `_.shuffle` which mutates and sets the size of `array`.
 *
 * @private
 * @param {Array} array The array to shuffle.
 * @param {number} [size=array.length] The size of `array`.
 * @returns {Array} Returns `array`.
 */
function shuffleSelf(array, size) {
  var index = -1,
      length = array.length,
      lastIndex = length - 1;

  size = size === undefined ? length : size;
  while (++index < size) {
    var rand = baseRandom(index, lastIndex),
        value = array[rand];

    array[rand] = array[index];
    array[index] = value;
  }
  array.length = size;
  return array;
}

/**
 * A specialized version of `_.sampleSize` for arrays.
 *
 * @private
 * @param {Array} array The array to sample.
 * @param {number} n The number of elements to sample.
 * @returns {Array} Returns the random elements.
 */
function arraySampleSize(array, n) {
  return shuffleSelf(copyArray(array), baseClamp(n, 0, array.length));
}

/**
 * The base implementation of `_.sampleSize` without param guards.
 *
 * @private
 * @param {Array|Object} collection The collection to sample.
 * @param {number} n The number of elements to sample.
 * @returns {Array} Returns the random elements.
 */
function baseSampleSize(collection, n) {
  var array = values$1(collection);
  return shuffleSelf(array, baseClamp(n, 0, array.length));
}

/**
 * Gets `n` random elements at unique keys from `collection` up to the
 * size of `collection`.
 *
 * @static
 * @memberOf _
 * @since 4.0.0
 * @category Collection
 * @param {Array|Object} collection The collection to sample.
 * @param {number} [n=1] The number of elements to sample.
 * @param- {Object} [guard] Enables use as an iteratee for methods like `_.map`.
 * @returns {Array} Returns the random elements.
 * @example
 *
 * _.sampleSize([1, 2, 3], 2);
 * // => [3, 1]
 *
 * _.sampleSize([1, 2, 3], 4);
 * // => [2, 3, 1]
 */
function sampleSize$1(collection, n, guard) {
  if ((guard ? isIterateeCall(collection, n, guard) : n === undefined)) {
    n = 1;
  } else {
    n = toInteger(n);
  }
  var func = isArray$1(collection) ? arraySampleSize : baseSampleSize;
  return func(collection, n);
}

/**
 * A specialized version of `_.shuffle` for arrays.
 *
 * @private
 * @param {Array} array The array to shuffle.
 * @returns {Array} Returns the new shuffled array.
 */
function arrayShuffle(array) {
  return shuffleSelf(copyArray(array));
}

/**
 * The base implementation of `_.shuffle`.
 *
 * @private
 * @param {Array|Object} collection The collection to shuffle.
 * @returns {Array} Returns the new shuffled array.
 */
function baseShuffle(collection) {
  return shuffleSelf(values$1(collection));
}

/**
 * Creates an array of shuffled values, using a version of the
 * [Fisher-Yates shuffle](https://en.wikipedia.org/wiki/Fisher-Yates_shuffle).
 *
 * @static
 * @memberOf _
 * @since 0.1.0
 * @category Collection
 * @param {Array|Object} collection The collection to shuffle.
 * @returns {Array} Returns the new shuffled array.
 * @example
 *
 * _.shuffle([1, 2, 3, 4]);
 * // => [4, 1, 3, 2]
 */
function shuffle$1(collection) {
  var func = isArray$1(collection) ? arrayShuffle : baseShuffle;
  return func(collection);
}

/** Used as references for various `Number` constants. */
var MAX_SAFE_INTEGER = 9007199254740991;

/** Used as references for the maximum length and index of an array. */
var MAX_ARRAY_LENGTH = 4294967295;

/* Built-in method references for those with the same name as other `lodash` methods. */
var nativeMin = Math.min;

/**
 * Invokes the iteratee `n` times, returning an array of the results of
 * each invocation. The iteratee is invoked with one argument; (index).
 *
 * @static
 * @since 0.1.0
 * @memberOf _
 * @category Util
 * @param {number} n The number of times to invoke `iteratee`.
 * @param {Function} [iteratee=_.identity] The function invoked per iteration.
 * @returns {Array} Returns the array of results.
 * @example
 *
 * _.times(3, String);
 * // => ['0', '1', '2']
 *
 *  _.times(4, _.constant(0));
 * // => [0, 0, 0, 0]
 */
function times(n, iteratee) {
  n = toInteger(n);
  if (n < 1 || n > MAX_SAFE_INTEGER) {
    return [];
  }
  var index = MAX_ARRAY_LENGTH,
      length = nativeMin(n, MAX_ARRAY_LENGTH);

  iteratee = castFunction(iteratee);
  n -= MAX_ARRAY_LENGTH;

  var result = baseTimes(length, iteratee);
  while (++index < n) {
    iteratee(index);
  }
  return result;
}

// 使用 lodash 提供的方式
/** 获取类型的字符串描述，为小写 */

function getType(obj) {
  return obj === null || obj === undefined ? String(obj) : Object.prototype.toString.call(obj).match(/\[object (\w+)\]/)[1].toLowerCase();
}
function isObjectOrArray(value) {
  return isObject(value) || isArray$1(value);
}
function isNumeric(value) {
  return !isNaN(parseFloat(value)) && isFinite(value);
}

/** 执行多次该函数 */

function each(obj, iterator, thisArg) {
  var i;
  var key;

  if (isNumber(obj)) {
    /** 重复使用函数 */
    _toConsumableArray(Array(obj).keys()).forEach(function (index) {
      return iterator.call(thisArg, index, index);
    });
  } else if (isArrayLikeObject(obj)) {
    /** 遍历类数组对象 */
    for (i = 0; i < obj.length; i++) {
      var result = iterator.call(thisArg, obj[i], i, obj);
      if (result === false) break;
    }
  } else {
    /** 遍历对象 */
    // 注意这里是被打断，而非跳过
    for (key in obj) {
      var _result = iterator.call(thisArg, obj[key], key, obj);

      if (_result === false) break;
    }
  }
}

/*
    ### Mock.heredoc(fn)

    * Mock.heredoc(fn)

    以直观、安全的方式书写（多行）HTML 模板。

    **使用示例**如下所示：

        var tpl = Mock.heredoc(function() {
            /*!
        {{email}}{{age}}
        <!-- Mock {
            email: '@EMAIL',
            age: '@INT(1,100)'
        } -->
            *\/
        })

    **相关阅读**
    * [Creating multiline strings in JavaScript](http://stackoverflow.com/questions/805107/creating-multiline-strings-in-javascript)、
*/
function heredoc(fn) {
  // 1. 移除起始的 function(){ /*!
  // 2. 移除末尾的 */ }
  // 3. 移除起始和末尾的空格
  return fn.toString().replace(/^[^\/]+\/\*!?/, '').replace(/\*\/[^\/]+$/, '').replace(/^[\s\xA0]+/, '').replace(/[\s\xA0]+$/, ''); // .trim()
}

/*
    ## Utilities
*/
var keys = Object.keys;
var values = Object.values;
function noop() {}

var Util = /*#__PURE__*/Object.freeze({
  __proto__: null,
  keys: keys,
  values: values,
  noop: noop,
  getType: getType,
  isObjectOrArray: isObjectOrArray,
  isNumeric: isNumeric,
  isString: isString,
  isObject: isObject,
  isArray: isArray$1,
  isRegExp: isRegExp$1,
  isFunction: isFunction,
  each: each,
  heredoc: heredoc
});

var commonjsGlobal$1 = typeof globalThis !== 'undefined' ? globalThis : typeof window !== 'undefined' ? window : typeof global !== 'undefined' ? global : typeof self !== 'undefined' ? self : {};

var alea$1 = {exports: {}};

(function (module) {
// A port of an algorithm by Johannes Baagøe <baagoe@baagoe.com>, 2010
// http://baagoe.com/en/RandomMusings/javascript/
// https://github.com/nquinlan/better-random-numbers-for-javascript-mirror
// Original work is under MIT license -

// Copyright (C) 2010 by Johannes Baagøe <baagoe@baagoe.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.



(function(global, module, define) {

function Alea(seed) {
  var me = this, mash = Mash();

  me.next = function() {
    var t = 2091639 * me.s0 + me.c * 2.3283064365386963e-10; // 2^-32
    me.s0 = me.s1;
    me.s1 = me.s2;
    return me.s2 = t - (me.c = t | 0);
  };

  // Apply the seeding algorithm from Baagoe.
  me.c = 1;
  me.s0 = mash(' ');
  me.s1 = mash(' ');
  me.s2 = mash(' ');
  me.s0 -= mash(seed);
  if (me.s0 < 0) { me.s0 += 1; }
  me.s1 -= mash(seed);
  if (me.s1 < 0) { me.s1 += 1; }
  me.s2 -= mash(seed);
  if (me.s2 < 0) { me.s2 += 1; }
  mash = null;
}

function copy(f, t) {
  t.c = f.c;
  t.s0 = f.s0;
  t.s1 = f.s1;
  t.s2 = f.s2;
  return t;
}

function impl(seed, opts) {
  var xg = new Alea(seed),
      state = opts && opts.state,
      prng = xg.next;
  prng.int32 = function() { return (xg.next() * 0x100000000) | 0; };
  prng.double = function() {
    return prng() + (prng() * 0x200000 | 0) * 1.1102230246251565e-16; // 2^-53
  };
  prng.quick = prng;
  if (state) {
    if (typeof(state) == 'object') copy(state, xg);
    prng.state = function() { return copy(xg, {}); };
  }
  return prng;
}

function Mash() {
  var n = 0xefc8249d;

  var mash = function(data) {
    data = String(data);
    for (var i = 0; i < data.length; i++) {
      n += data.charCodeAt(i);
      var h = 0.02519603282416938 * n;
      n = h >>> 0;
      h -= n;
      h *= n;
      n = h >>> 0;
      h -= n;
      n += h * 0x100000000; // 2^32
    }
    return (n >>> 0) * 2.3283064365386963e-10; // 2^-32
  };

  return mash;
}


if (module && module.exports) {
  module.exports = impl;
} else if (define && define.amd) {
  define(function() { return impl; });
} else {
  this.alea = impl;
}

})(
  commonjsGlobal$1,
  module,    // present in node.js
  (typeof undefined) == 'function'    // present with an AMD loader
);
}(alea$1));

var xor128$1 = {exports: {}};

(function (module) {
// A Javascript implementaion of the "xor128" prng algorithm by
// George Marsaglia.  See http://www.jstatsoft.org/v08/i14/paper

(function(global, module, define) {

function XorGen(seed) {
  var me = this, strseed = '';

  me.x = 0;
  me.y = 0;
  me.z = 0;
  me.w = 0;

  // Set up generator function.
  me.next = function() {
    var t = me.x ^ (me.x << 11);
    me.x = me.y;
    me.y = me.z;
    me.z = me.w;
    return me.w ^= (me.w >>> 19) ^ t ^ (t >>> 8);
  };

  if (seed === (seed | 0)) {
    // Integer seed.
    me.x = seed;
  } else {
    // String seed.
    strseed += seed;
  }

  // Mix in string seed, then discard an initial batch of 64 values.
  for (var k = 0; k < strseed.length + 64; k++) {
    me.x ^= strseed.charCodeAt(k) | 0;
    me.next();
  }
}

function copy(f, t) {
  t.x = f.x;
  t.y = f.y;
  t.z = f.z;
  t.w = f.w;
  return t;
}

function impl(seed, opts) {
  var xg = new XorGen(seed),
      state = opts && opts.state,
      prng = function() { return (xg.next() >>> 0) / 0x100000000; };
  prng.double = function() {
    do {
      var top = xg.next() >>> 11,
          bot = (xg.next() >>> 0) / 0x100000000,
          result = (top + bot) / (1 << 21);
    } while (result === 0);
    return result;
  };
  prng.int32 = xg.next;
  prng.quick = prng;
  if (state) {
    if (typeof(state) == 'object') copy(state, xg);
    prng.state = function() { return copy(xg, {}); };
  }
  return prng;
}

if (module && module.exports) {
  module.exports = impl;
} else if (define && define.amd) {
  define(function() { return impl; });
} else {
  this.xor128 = impl;
}

})(
  commonjsGlobal$1,
  module,    // present in node.js
  (typeof undefined) == 'function'    // present with an AMD loader
);
}(xor128$1));

var xorwow$1 = {exports: {}};

(function (module) {
// A Javascript implementaion of the "xorwow" prng algorithm by
// George Marsaglia.  See http://www.jstatsoft.org/v08/i14/paper

(function(global, module, define) {

function XorGen(seed) {
  var me = this, strseed = '';

  // Set up generator function.
  me.next = function() {
    var t = (me.x ^ (me.x >>> 2));
    me.x = me.y; me.y = me.z; me.z = me.w; me.w = me.v;
    return (me.d = (me.d + 362437 | 0)) +
       (me.v = (me.v ^ (me.v << 4)) ^ (t ^ (t << 1))) | 0;
  };

  me.x = 0;
  me.y = 0;
  me.z = 0;
  me.w = 0;
  me.v = 0;

  if (seed === (seed | 0)) {
    // Integer seed.
    me.x = seed;
  } else {
    // String seed.
    strseed += seed;
  }

  // Mix in string seed, then discard an initial batch of 64 values.
  for (var k = 0; k < strseed.length + 64; k++) {
    me.x ^= strseed.charCodeAt(k) | 0;
    if (k == strseed.length) {
      me.d = me.x << 10 ^ me.x >>> 4;
    }
    me.next();
  }
}

function copy(f, t) {
  t.x = f.x;
  t.y = f.y;
  t.z = f.z;
  t.w = f.w;
  t.v = f.v;
  t.d = f.d;
  return t;
}

function impl(seed, opts) {
  var xg = new XorGen(seed),
      state = opts && opts.state,
      prng = function() { return (xg.next() >>> 0) / 0x100000000; };
  prng.double = function() {
    do {
      var top = xg.next() >>> 11,
          bot = (xg.next() >>> 0) / 0x100000000,
          result = (top + bot) / (1 << 21);
    } while (result === 0);
    return result;
  };
  prng.int32 = xg.next;
  prng.quick = prng;
  if (state) {
    if (typeof(state) == 'object') copy(state, xg);
    prng.state = function() { return copy(xg, {}); };
  }
  return prng;
}

if (module && module.exports) {
  module.exports = impl;
} else if (define && define.amd) {
  define(function() { return impl; });
} else {
  this.xorwow = impl;
}

})(
  commonjsGlobal$1,
  module,    // present in node.js
  (typeof undefined) == 'function'    // present with an AMD loader
);
}(xorwow$1));

var xorshift7$1 = {exports: {}};

(function (module) {
// A Javascript implementaion of the "xorshift7" algorithm by
// François Panneton and Pierre L'ecuyer:
// "On the Xorgshift Random Number Generators"
// http://saluc.engr.uconn.edu/refs/crypto/rng/panneton05onthexorshift.pdf

(function(global, module, define) {

function XorGen(seed) {
  var me = this;

  // Set up generator function.
  me.next = function() {
    // Update xor generator.
    var X = me.x, i = me.i, t, v;
    t = X[i]; t ^= (t >>> 7); v = t ^ (t << 24);
    t = X[(i + 1) & 7]; v ^= t ^ (t >>> 10);
    t = X[(i + 3) & 7]; v ^= t ^ (t >>> 3);
    t = X[(i + 4) & 7]; v ^= t ^ (t << 7);
    t = X[(i + 7) & 7]; t = t ^ (t << 13); v ^= t ^ (t << 9);
    X[i] = v;
    me.i = (i + 1) & 7;
    return v;
  };

  function init(me, seed) {
    var j, X = [];

    if (seed === (seed | 0)) {
      // Seed state array using a 32-bit integer.
      X[0] = seed;
    } else {
      // Seed state using a string.
      seed = '' + seed;
      for (j = 0; j < seed.length; ++j) {
        X[j & 7] = (X[j & 7] << 15) ^
            (seed.charCodeAt(j) + X[(j + 1) & 7] << 13);
      }
    }
    // Enforce an array length of 8, not all zeroes.
    while (X.length < 8) X.push(0);
    for (j = 0; j < 8 && X[j] === 0; ++j);
    if (j == 8) X[7] = -1;

    me.x = X;
    me.i = 0;

    // Discard an initial 256 values.
    for (j = 256; j > 0; --j) {
      me.next();
    }
  }

  init(me, seed);
}

function copy(f, t) {
  t.x = f.x.slice();
  t.i = f.i;
  return t;
}

function impl(seed, opts) {
  if (seed == null) seed = +(new Date);
  var xg = new XorGen(seed),
      state = opts && opts.state,
      prng = function() { return (xg.next() >>> 0) / 0x100000000; };
  prng.double = function() {
    do {
      var top = xg.next() >>> 11,
          bot = (xg.next() >>> 0) / 0x100000000,
          result = (top + bot) / (1 << 21);
    } while (result === 0);
    return result;
  };
  prng.int32 = xg.next;
  prng.quick = prng;
  if (state) {
    if (state.x) copy(state, xg);
    prng.state = function() { return copy(xg, {}); };
  }
  return prng;
}

if (module && module.exports) {
  module.exports = impl;
} else if (define && define.amd) {
  define(function() { return impl; });
} else {
  this.xorshift7 = impl;
}

})(
  commonjsGlobal$1,
  module,    // present in node.js
  (typeof undefined) == 'function'    // present with an AMD loader
);
}(xorshift7$1));

var xor4096$1 = {exports: {}};

(function (module) {
// A Javascript implementaion of Richard Brent's Xorgens xor4096 algorithm.
//
// This fast non-cryptographic random number generator is designed for
// use in Monte-Carlo algorithms. It combines a long-period xorshift
// generator with a Weyl generator, and it passes all common batteries
// of stasticial tests for randomness while consuming only a few nanoseconds
// for each prng generated.  For background on the generator, see Brent's
// paper: "Some long-period random number generators using shifts and xors."
// http://arxiv.org/pdf/1004.3115v1.pdf
//
// Usage:
//
// var xor4096 = require('xor4096');
// random = xor4096(1);                        // Seed with int32 or string.
// assert.equal(random(), 0.1520436450538547); // (0, 1) range, 53 bits.
// assert.equal(random.int32(), 1806534897);   // signed int32, 32 bits.
//
// For nonzero numeric keys, this impelementation provides a sequence
// identical to that by Brent's xorgens 3 implementaion in C.  This
// implementation also provides for initalizing the generator with
// string seeds, or for saving and restoring the state of the generator.
//
// On Chrome, this prng benchmarks about 2.1 times slower than
// Javascript's built-in Math.random().

(function(global, module, define) {

function XorGen(seed) {
  var me = this;

  // Set up generator function.
  me.next = function() {
    var w = me.w,
        X = me.X, i = me.i, t, v;
    // Update Weyl generator.
    me.w = w = (w + 0x61c88647) | 0;
    // Update xor generator.
    v = X[(i + 34) & 127];
    t = X[i = ((i + 1) & 127)];
    v ^= v << 13;
    t ^= t << 17;
    v ^= v >>> 15;
    t ^= t >>> 12;
    // Update Xor generator array state.
    v = X[i] = v ^ t;
    me.i = i;
    // Result is the combination.
    return (v + (w ^ (w >>> 16))) | 0;
  };

  function init(me, seed) {
    var t, v, i, j, w, X = [], limit = 128;
    if (seed === (seed | 0)) {
      // Numeric seeds initialize v, which is used to generates X.
      v = seed;
      seed = null;
    } else {
      // String seeds are mixed into v and X one character at a time.
      seed = seed + '\0';
      v = 0;
      limit = Math.max(limit, seed.length);
    }
    // Initialize circular array and weyl value.
    for (i = 0, j = -32; j < limit; ++j) {
      // Put the unicode characters into the array, and shuffle them.
      if (seed) v ^= seed.charCodeAt((j + 32) % seed.length);
      // After 32 shuffles, take v as the starting w value.
      if (j === 0) w = v;
      v ^= v << 10;
      v ^= v >>> 15;
      v ^= v << 4;
      v ^= v >>> 13;
      if (j >= 0) {
        w = (w + 0x61c88647) | 0;     // Weyl.
        t = (X[j & 127] ^= (v + w));  // Combine xor and weyl to init array.
        i = (0 == t) ? i + 1 : 0;     // Count zeroes.
      }
    }
    // We have detected all zeroes; make the key nonzero.
    if (i >= 128) {
      X[(seed && seed.length || 0) & 127] = -1;
    }
    // Run the generator 512 times to further mix the state before using it.
    // Factoring this as a function slows the main generator, so it is just
    // unrolled here.  The weyl generator is not advanced while warming up.
    i = 127;
    for (j = 4 * 128; j > 0; --j) {
      v = X[(i + 34) & 127];
      t = X[i = ((i + 1) & 127)];
      v ^= v << 13;
      t ^= t << 17;
      v ^= v >>> 15;
      t ^= t >>> 12;
      X[i] = v ^ t;
    }
    // Storing state as object members is faster than using closure variables.
    me.w = w;
    me.X = X;
    me.i = i;
  }

  init(me, seed);
}

function copy(f, t) {
  t.i = f.i;
  t.w = f.w;
  t.X = f.X.slice();
  return t;
}
function impl(seed, opts) {
  if (seed == null) seed = +(new Date);
  var xg = new XorGen(seed),
      state = opts && opts.state,
      prng = function() { return (xg.next() >>> 0) / 0x100000000; };
  prng.double = function() {
    do {
      var top = xg.next() >>> 11,
          bot = (xg.next() >>> 0) / 0x100000000,
          result = (top + bot) / (1 << 21);
    } while (result === 0);
    return result;
  };
  prng.int32 = xg.next;
  prng.quick = prng;
  if (state) {
    if (state.X) copy(state, xg);
    prng.state = function() { return copy(xg, {}); };
  }
  return prng;
}

if (module && module.exports) {
  module.exports = impl;
} else if (define && define.amd) {
  define(function() { return impl; });
} else {
  this.xor4096 = impl;
}

})(
  commonjsGlobal$1,                                     // window object or global
  module,    // present in node.js
  (typeof undefined) == 'function'    // present with an AMD loader
);
}(xor4096$1));

var tychei$1 = {exports: {}};

(function (module) {
// A Javascript implementaion of the "Tyche-i" prng algorithm by
// Samuel Neves and Filipe Araujo.
// See https://eden.dei.uc.pt/~sneves/pubs/2011-snfa2.pdf

(function(global, module, define) {

function XorGen(seed) {
  var me = this, strseed = '';

  // Set up generator function.
  me.next = function() {
    var b = me.b, c = me.c, d = me.d, a = me.a;
    b = (b << 25) ^ (b >>> 7) ^ c;
    c = (c - d) | 0;
    d = (d << 24) ^ (d >>> 8) ^ a;
    a = (a - b) | 0;
    me.b = b = (b << 20) ^ (b >>> 12) ^ c;
    me.c = c = (c - d) | 0;
    me.d = (d << 16) ^ (c >>> 16) ^ a;
    return me.a = (a - b) | 0;
  };

  /* The following is non-inverted tyche, which has better internal
   * bit diffusion, but which is about 25% slower than tyche-i in JS.
  me.next = function() {
    var a = me.a, b = me.b, c = me.c, d = me.d;
    a = (me.a + me.b | 0) >>> 0;
    d = me.d ^ a; d = d << 16 ^ d >>> 16;
    c = me.c + d | 0;
    b = me.b ^ c; b = b << 12 ^ d >>> 20;
    me.a = a = a + b | 0;
    d = d ^ a; me.d = d = d << 8 ^ d >>> 24;
    me.c = c = c + d | 0;
    b = b ^ c;
    return me.b = (b << 7 ^ b >>> 25);
  }
  */

  me.a = 0;
  me.b = 0;
  me.c = 2654435769 | 0;
  me.d = 1367130551;

  if (seed === Math.floor(seed)) {
    // Integer seed.
    me.a = (seed / 0x100000000) | 0;
    me.b = seed | 0;
  } else {
    // String seed.
    strseed += seed;
  }

  // Mix in string seed, then discard an initial batch of 64 values.
  for (var k = 0; k < strseed.length + 20; k++) {
    me.b ^= strseed.charCodeAt(k) | 0;
    me.next();
  }
}

function copy(f, t) {
  t.a = f.a;
  t.b = f.b;
  t.c = f.c;
  t.d = f.d;
  return t;
}
function impl(seed, opts) {
  var xg = new XorGen(seed),
      state = opts && opts.state,
      prng = function() { return (xg.next() >>> 0) / 0x100000000; };
  prng.double = function() {
    do {
      var top = xg.next() >>> 11,
          bot = (xg.next() >>> 0) / 0x100000000,
          result = (top + bot) / (1 << 21);
    } while (result === 0);
    return result;
  };
  prng.int32 = xg.next;
  prng.quick = prng;
  if (state) {
    if (typeof(state) == 'object') copy(state, xg);
    prng.state = function() { return copy(xg, {}); };
  }
  return prng;
}

if (module && module.exports) {
  module.exports = impl;
} else if (define && define.amd) {
  define(function() { return impl; });
} else {
  this.tychei = impl;
}

})(
  commonjsGlobal$1,
  module,    // present in node.js
  (typeof undefined) == 'function'    // present with an AMD loader
);
}(tychei$1));

var seedrandom$1 = {exports: {}};

/*
Copyright 2019 David Bau.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

(function (module) {
(function (global, pool, math) {
//
// The following constants are related to IEEE 754 limits.
//

var width = 256,        // each RC4 output is 0 <= x < 256
    chunks = 6,         // at least six RC4 outputs for each double
    digits = 52,        // there are 52 significant digits in a double
    rngname = 'random', // rngname: name for Math.random and Math.seedrandom
    startdenom = math.pow(width, chunks),
    significance = math.pow(2, digits),
    overflow = significance * 2,
    mask = width - 1,
    nodecrypto;         // node.js crypto module, initialized at the bottom.

//
// seedrandom()
// This is the seedrandom function described above.
//
function seedrandom(seed, options, callback) {
  var key = [];
  options = (options == true) ? { entropy: true } : (options || {});

  // Flatten the seed string or build one from local entropy if needed.
  var shortseed = mixkey(flatten(
    options.entropy ? [seed, tostring(pool)] :
    (seed == null) ? autoseed() : seed, 3), key);

  // Use the seed to initialize an ARC4 generator.
  var arc4 = new ARC4(key);

  // This function returns a random double in [0, 1) that contains
  // randomness in every bit of the mantissa of the IEEE 754 value.
  var prng = function() {
    var n = arc4.g(chunks),             // Start with a numerator n < 2 ^ 48
        d = startdenom,                 //   and denominator d = 2 ^ 48.
        x = 0;                          //   and no 'extra last byte'.
    while (n < significance) {          // Fill up all significant digits by
      n = (n + x) * width;              //   shifting numerator and
      d *= width;                       //   denominator and generating a
      x = arc4.g(1);                    //   new least-significant-byte.
    }
    while (n >= overflow) {             // To avoid rounding up, before adding
      n /= 2;                           //   last byte, shift everything
      d /= 2;                           //   right using integer math until
      x >>>= 1;                         //   we have exactly the desired bits.
    }
    return (n + x) / d;                 // Form the number within [0, 1).
  };

  prng.int32 = function() { return arc4.g(4) | 0; };
  prng.quick = function() { return arc4.g(4) / 0x100000000; };
  prng.double = prng;

  // Mix the randomness into accumulated entropy.
  mixkey(tostring(arc4.S), pool);

  // Calling convention: what to return as a function of prng, seed, is_math.
  return (options.pass || callback ||
      function(prng, seed, is_math_call, state) {
        if (state) {
          // Load the arc4 state from the given state if it has an S array.
          if (state.S) { copy(state, arc4); }
          // Only provide the .state method if requested via options.state.
          prng.state = function() { return copy(arc4, {}); };
        }

        // If called as a method of Math (Math.seedrandom()), mutate
        // Math.random because that is how seedrandom.js has worked since v1.0.
        if (is_math_call) { math[rngname] = prng; return seed; }

        // Otherwise, it is a newer calling convention, so return the
        // prng directly.
        else return prng;
      })(
  prng,
  shortseed,
  'global' in options ? options.global : (this == math),
  options.state);
}

//
// ARC4
//
// An ARC4 implementation.  The constructor takes a key in the form of
// an array of at most (width) integers that should be 0 <= x < (width).
//
// The g(count) method returns a pseudorandom integer that concatenates
// the next (count) outputs from ARC4.  Its return value is a number x
// that is in the range 0 <= x < (width ^ count).
//
function ARC4(key) {
  var t, keylen = key.length,
      me = this, i = 0, j = me.i = me.j = 0, s = me.S = [];

  // The empty key [] is treated as [0].
  if (!keylen) { key = [keylen++]; }

  // Set up S using the standard key scheduling algorithm.
  while (i < width) {
    s[i] = i++;
  }
  for (i = 0; i < width; i++) {
    s[i] = s[j = mask & (j + key[i % keylen] + (t = s[i]))];
    s[j] = t;
  }

  // The "g" method returns the next (count) outputs as one number.
  (me.g = function(count) {
    // Using instance members instead of closure state nearly doubles speed.
    var t, r = 0,
        i = me.i, j = me.j, s = me.S;
    while (count--) {
      t = s[i = mask & (i + 1)];
      r = r * width + s[mask & ((s[i] = s[j = mask & (j + t)]) + (s[j] = t))];
    }
    me.i = i; me.j = j;
    return r;
    // For robust unpredictability, the function call below automatically
    // discards an initial batch of values.  This is called RC4-drop[256].
    // See http://google.com/search?q=rsa+fluhrer+response&btnI
  })(width);
}

//
// copy()
// Copies internal state of ARC4 to or from a plain object.
//
function copy(f, t) {
  t.i = f.i;
  t.j = f.j;
  t.S = f.S.slice();
  return t;
}
//
// flatten()
// Converts an object tree to nested arrays of strings.
//
function flatten(obj, depth) {
  var result = [], typ = (typeof obj), prop;
  if (depth && typ == 'object') {
    for (prop in obj) {
      try { result.push(flatten(obj[prop], depth - 1)); } catch (e) {}
    }
  }
  return (result.length ? result : typ == 'string' ? obj : obj + '\0');
}

//
// mixkey()
// Mixes a string seed into a key that is an array of integers, and
// returns a shortened string seed that is equivalent to the result key.
//
function mixkey(seed, key) {
  var stringseed = seed + '', smear, j = 0;
  while (j < stringseed.length) {
    key[mask & j] =
      mask & ((smear ^= key[mask & j] * 19) + stringseed.charCodeAt(j++));
  }
  return tostring(key);
}

//
// autoseed()
// Returns an object for autoseeding, using window.crypto and Node crypto
// module if available.
//
function autoseed() {
  try {
    var out;
    if (nodecrypto && (out = nodecrypto.randomBytes)) {
      // The use of 'out' to remember randomBytes makes tight minified code.
      out = out(width);
    } else {
      out = new Uint8Array(width);
      (global.crypto || global.msCrypto).getRandomValues(out);
    }
    return tostring(out);
  } catch (e) {
    var browser = global.navigator,
        plugins = browser && browser.plugins;
    return [+new Date, global, plugins, global.screen, tostring(pool)];
  }
}

//
// tostring()
// Converts an array of charcodes to a string
//
function tostring(a) {
  return String.fromCharCode.apply(0, a);
}

//
// When seedrandom.js is loaded, we immediately mix a few bits
// from the built-in RNG into the entropy pool.  Because we do
// not want to interfere with deterministic PRNG state later,
// seedrandom will not call math.random on its own again after
// initialization.
//
mixkey(math.random(), pool);

//
// Nodejs and AMD support: export the implementation as a module using
// either convention.
//
if (module.exports) {
  module.exports = seedrandom;
  // When in node.js, try using crypto package for autoseeding.
  try {
    nodecrypto = require('crypto');
  } catch (ex) {}
} else {
  // When included as a plain script, set up Math.seedrandom global.
  math['seed' + rngname] = seedrandom;
}


// End anonymous scope, and pass initial values.
})(
  // global: `self` in browsers (including strict mode and web workers),
  // otherwise `this` in Node and other environments
  (typeof self !== 'undefined') ? self : commonjsGlobal$1,
  [],     // pool: entropy pool starts empty
  Math    // math: package containing random, pow, and seedrandom
);
}(seedrandom$1));

// A library of seedable RNGs implemented in Javascript.
//
// Usage:
//
// var seedrandom = require('seedrandom');
// var random = seedrandom(1); // or any seed.
// var x = random();       // 0 <= x < 1.  Every bit is random.
// var x = random.quick(); // 0 <= x < 1.  32 bits of randomness.

// alea, a 53-bit multiply-with-carry generator by Johannes Baagøe.
// Period: ~2^116
// Reported to pass all BigCrush tests.
var alea = alea$1.exports;

// xor128, a pure xor-shift generator by George Marsaglia.
// Period: 2^128-1.
// Reported to fail: MatrixRank and LinearComp.
var xor128 = xor128$1.exports;

// xorwow, George Marsaglia's 160-bit xor-shift combined plus weyl.
// Period: 2^192-2^32
// Reported to fail: CollisionOver, SimpPoker, and LinearComp.
var xorwow = xorwow$1.exports;

// xorshift7, by François Panneton and Pierre L'ecuyer, takes
// a different approach: it adds robustness by allowing more shifts
// than Marsaglia's original three.  It is a 7-shift generator
// with 256 bits, that passes BigCrush with no systmatic failures.
// Period 2^256-1.
// No systematic BigCrush failures reported.
var xorshift7 = xorshift7$1.exports;

// xor4096, by Richard Brent, is a 4096-bit xor-shift with a
// very long period that also adds a Weyl generator. It also passes
// BigCrush with no systematic failures.  Its long period may
// be useful if you have many generators and need to avoid
// collisions.
// Period: 2^4128-2^32.
// No systematic BigCrush failures reported.
var xor4096 = xor4096$1.exports;

// Tyche-i, by Samuel Neves and Filipe Araujo, is a bit-shifting random
// number generator derived from ChaCha, a modern stream cipher.
// https://eden.dei.uc.pt/~sneves/pubs/2011-snfa2.pdf
// Period: ~2^127
// No systematic BigCrush failures reported.
var tychei = tychei$1.exports;

// The original ARC4-based prng included in this library.
// Period: ~2^1600
var sr = seedrandom$1.exports;

sr.alea = alea;
sr.xor128 = xor128;
sr.xorwow = xorwow;
sr.xorshift7 = xorshift7;
sr.xor4096 = xor4096;
sr.tychei = tychei;

var seedrandom = sr;

function _random(lower, upper, floating, seed) {
  const step = upper - lower;
  const result = lower + step * (seed !== undefined ? seedrandom.alea(seed).quick() : Math.random());
  return floating ? result : Math.floor(result);
}

function random(...args) {
  args = args.filter(i => i !== undefined);
  const [a, b, c, d] = args;

  switch (args.length) {
    case 0:
      return _random(0, 1, true);

    case 1:
      switch (typeof a) {
        case "string":
          return _random(0, 1, false, a);

        case "boolean":
          return _random(0, 1, a);

        case "number":
          return _random(0, a, false);
      }

    case 2:
      switch (typeof b) {
        case "string":
          return _random(0, a, false, b);

        case "boolean":
          return _random(0, 1, b);

        case "number":
          return _random(a, b, false);
      }

    case 3:
      switch (typeof c) {
        case "string":
          return _random(a, b, false, c);

        case "boolean":
          return _random(a, b, c);
      }

    default:
      return _random(a, b, c, d);
  }
}

function sample(arr, seed) {
  return arr[random(0, arr.length, false, seed)];
}

function _sampleSize(collection, n, seed) {
  const copy = [...collection];
  if (n >= collection.length) return copy;
  if (n < 0) return [];
  const temp = [];

  while (temp.length < n) {
    const result = random(0, copy.length, seed);
    temp.push(copy.splice(result, 1)[0]);
  }

  return temp;
}

function sampleSize(collection, ...args) {
  args = args.filter(i => i !== undefined);
  const [a, b] = args;

  switch (args.length) {
    case 0:
      return _sampleSize(collection, 1);

    case 1:
      switch (typeof a) {
        case "string":
          return _sampleSize(collection, 1, a);

        case "number":
          return _sampleSize(collection, a);
      }

    default:
      return _sampleSize(collection, a, b);
  }
}

function parseRange(_range) {
  // 解析 min，max 值
  var _ref = _range && _range.match(Constant.RE_RANGE) || [],
      _ref2 = _slicedToArray(_ref, 3),
      _min = _ref2[1],
      _max = _ref2[2];

  var min = _min && parseInt(_min, 10) || 1;
  var max = _max && parseInt(_max, 10) || undefined;
  var count; // 求得两者之间的一个整数

  if (isNumber(min)) {
    count = isNumber(max) ? random(min, max, false) : min;
    if (max === undefined) max = count;
  }

  return {
    min: min,
    max: max,
    count: count
  };
}
/** 解析数据模板 */


function parser() {
  var paramString = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : '';
  // 解析整个指令字符串
  var parameters = paramString.match(Constant.RE_KEY) || [];

  var _parameters = _slicedToArray(parameters, 5);
      _parameters[1];
      var _step = _parameters[2],
      _range = _parameters[3],
      _drange = _parameters[4];

  var _parseRange = parseRange(_range),
      min = _parseRange.min,
      max = _parseRange.max,
      count = _parseRange.count;

  var _parseRange2 = parseRange(_drange),
      dmin = _parseRange2.min,
      dmax = _parseRange2.max,
      dcount = _parseRange2.count;

  var result = {
    /** 没有匹配到则为空数组 */
    parameters: parameters,
    range: [min, max],
    min: min,
    step: _step === undefined ? undefined : parseInt(_step, 10),
    max: max,
    count: count,
    decimal: typeof dcount === 'number',
    dmin: dmin,
    dmax: dmax,
    dcount: dcount
  };
  return result;
}

var store$1 = new Map();
/** 数组类型的构建器 */

var array = function array(_ref) {
  var rule = _ref.rule,
      template = _ref.template,
      context = _ref.context;
  var min = rule.min,
      max = rule.max,
      parameters = rule.parameters,
      count = rule.count,
      step = rule.step;
  var result = []; // 'name|1': []
  // 'name|count': []
  // 'name|min-max': []

  if (template.length === 0) return []; // 'arr': [{ 'email': '@EMAIL' }, { 'email': '@EMAIL' }]

  if (parameters.length === 0) {
    return template.map(function (item, index) {
      return gen(item, index.toString(), _objectSpread2(_objectSpread2({}, context), {}, {
        path: context.path,
        templatePath: context.templatePath,
        currentContext: result,
        templateCurrentContext: template
      }));
    });
  } else {
    // 'data|+1': [{}, {}]
    if (isNumber(step)) {
      //! array 依照上级的路径进行判断
      var path = context.path; // 从存储中获取一个元素

      var storeKey = path.join();
      var totalIndex = (store$1.get(storeKey) || 1) + step;
      store$1.set(storeKey, totalIndex);
      var index = totalIndex % template.length;
      return gen(template[index], index.toString(), _objectSpread2(_objectSpread2({}, context), {}, {
        path: context.path,
        templatePath: context.templatePath,
        currentContext: [],
        templateCurrentContext: template
      }));
    } else if (min === max && max === count) {
      // ! array|1 是抽取 ，array|2 是重复
      // 'method|1': ['GET', 'POST', 'HEAD', 'DELETE']
      if (count === 1) {
        var value = sample(template);
        return gen(value, template.indexOf(value).toString(), _objectSpread2(_objectSpread2({}, context), {}, {
          path: context.path,
          templatePath: context.templatePath,
          currentContext: [],
          templateCurrentContext: template
        }));
      } else {
        var data = times(count, function () {
          return template.map(function (i) {
            var key = template.indexOf(i).toString();
            return gen(i, key, _objectSpread2(_objectSpread2({}, context), {}, {
              path: context.path,
              templatePath: context.templatePath,
              currentContext: [],
              templateCurrentContext: template
            }));
          });
        });
        return data;
      }
    } else if (count) {
      // 'data|1-10': [{}]
      return times(count, function () {
        return template.map(function (item, index) {
          var result = gen(item, index.toString(), _objectSpread2(_objectSpread2({}, context), {}, {
            path: context.path,
            templatePath: context.templatePath,
            currentContext: [],
            templateCurrentContext: template
          }));
          return result;
        });
      });
    }
  }
};

/**
 * @description  文本类型不需要进行更改
 */
var decodes = {
  "boolean": function boolean(input) {
    return JSON.parse(input);
  },
  number: function number(input) {
    var result = parseInt(input);
    return isNaN(result) ? input : result;
  },
  // json 可以同时转化多类型的数据，适合于重载的场景
  json: function json(input) {
    try {
      return JSON.parse(input);
    } catch (e) {
      return input;
    }
  }
};
/** 包裹函数即可实现类型转化 */

var decode = function decode(func, decodeKey) {
  return function () {
    for (var _len = arguments.length, args = new Array(_len), _key = 0; _key < _len; _key++) {
      args[_key] = arguments[_key];
    }

    return func.apply(null, args.map(function (item, index) {
      if (item === 'undefined') return;
      var key = decodeKey[index];

      if (key && key in decodes) {
        return decodes[key].call(null, item);
      }

      return item;
    }));
  };
};

/** 布尔随机，
 * @param cur 反转结果
 */

var _boolean$1 = decode(function (midPoint, max) {
  var cur = arguments.length > 2 && arguments[2] !== undefined ? arguments[2] : true;
  // ! cur:boolean 是划分概率的一个启动符号
  var result = random(0, max, true) < midPoint;
  return cur ? result : !result;
}, ['number', 'number', 'boolean']);

function _boolean(_ref) {
  var rule = _ref.rule,
      template = _ref.template;
      _ref.context;
  var max = rule.max,
      min = rule.min,
      count = rule.count;

  if (count === max && count === min) {
    // 'prop|trueFalsePoint-maxEdge': false, 选定正反分界线和最大值
    return _boolean$1(min, max, template);
  } else {
    return _boolean$1(1, count, template); // 'prop|multiple': false, 概率为 1/multiple
  }
}

var integer = decode(function () {
  var min = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : -2 ^ 10;
  var max = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 2 ^ 10;
  return random$1(min, max, false);
}, ['number', 'number']);
var natural = decode(function () {
  var min = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : 0;
  var max = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 2 ^ 10;
  return random$1(min, max, false);
}, ['number', 'number']);

var _float = decode(function () {
  var min = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : -2 ^ 10;
  var max = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 2 ^ 10;
  var dmin = arguments.length > 2 && arguments[2] !== undefined ? arguments[2] : 1;
  var dmax = arguments.length > 3 && arguments[3] !== undefined ? arguments[3] : 17;
  if (dmin < 1) dmin = 1;
  if (dmax > 17) dmax = 17; // 获取小数位数

  var dcount = random$1(dmin, dmax, false);
  var floatPoint = min === max ? random$1(0, 1, true) + min : random$1(min, max, true);
  return parseFloat(floatPoint.toFixed(dcount));
}, ['number', 'number', 'number', 'number']);

var store = new Map();
var number$1 = function number(_ref) {
  var template = _ref.template,
      _ref$rule = _ref.rule,
      decimal = _ref$rule.decimal,
      dmin = _ref$rule.dmin,
      dmax = _ref$rule.dmax,
      _ref$rule$range = _slicedToArray(_ref$rule.range, 2),
      min = _ref$rule$range[0],
      max = _ref$rule$range[1],
      count = _ref$rule.count,
      step = _ref$rule.step,
      context = _ref.context;

  var _template$toString$sp = template.toString().split('.'),
      _template$toString$sp2 = _slicedToArray(_template$toString$sp, 2),
      dec = _template$toString$sp2[1];

  if (decimal) {
    // 'float1|.1-10': 10,
    // 'float2|1-100.1-10': 1,
    // 'float3|999.1-10': 1,
    // 'float4|.3-10': 123.123,
    var result = _float(min, max, dmin, dmax); // 内置字符串转数字


    if (dec) {
      // 这里是原来有小数部分时，进行小数部分覆盖
      var floatString = result.toString().replace(new RegExp("\\.\\d{0,".concat(dec.length, "}|(?<=[^\\.]+)$"), 'm'), '.' + dec);
      return parseFloat(floatString);
    } else {
      return result;
    }
  } else {
    // integer
    // 'grade1|1-100': 1,
    if (step) {
      var key = context.templatePath.join();
      var val = store.get(key) || template;
      store.set(key, val + 1);
      return val + 1;
    } else {
      return typeof count === 'number' ? count : 1;
    }
  }
};

var VirtualValue = /*#__PURE__*/function () {
  function VirtualValue(value, context) {
    _classCallCheck(this, VirtualValue);

    _defineProperty(this, "key", '');

    this.value = value;
    this.context = context;
  }

  _createClass(VirtualValue, [{
    key: "replaceToReal",
    value: function replaceToReal() {
      this.parent[this.key] = this.value;
      this.parent = null;
    }
  }]);

  return VirtualValue;
}();

var setValueToObject = function setValueToObject(obj, key, value) {
  if (value instanceof VirtualValue) {
    value.parent = obj;
    value.key = key;
  }

  obj[key] = value;
};

var object = function object(_ref) {
  var template = _ref.template,
      _ref$rule = _ref.rule,
      min = _ref$rule.min,
      max = _ref$rule.max,
      count = _ref$rule.count,
      parameters = _ref$rule.parameters,
      context = _ref.context;
  var _ref2 = context,
      path = _ref2.path,
      templatePath = _ref2.templatePath;
  var result = {}; // 'obj|min-max': {}

  if (parameters.length) {
    console.log(count, min, max);
    sampleSize(Object.keys(template), count).forEach(function (key) {
      var name = key.replace(Constant.RE_KEY, '$1');
      setValueToObject(result, name, gen(template[key], key, _objectSpread2(_objectSpread2({}, context), {}, {
        path: [].concat(_toConsumableArray(path), [name]),
        templatePath: [].concat(_toConsumableArray(templatePath), [key]),
        currentContext: result,
        templateCurrentContext: template
      })));
    });
  } else {
    // 'obj': {}
    Object.entries(template).forEach(function (_ref3) {
      var _ref4 = _slicedToArray(_ref3, 2),
          key = _ref4[0],
          value = _ref4[1];

      var name = key.replace(Constant.RE_KEY, '$1');

      if (typeof value === 'function') {
        //! 非函数类型的键值对优先于函数类型，保证依赖加载
        // 函数会进行延迟执行
        var vir = new VirtualValue(value, context);
        context.delay["function"].push(vir);
        setValueToObject(result, name, vir);
      } else {
        setValueToObject(result, name, gen(value, key, _objectSpread2(_objectSpread2({}, context), {}, {
          path: path.concat(key),
          templatePath: templatePath.concat(key),
          currentContext: result,
          templateCurrentContext: template
        })));
      }
    });
  }

  return result;
};

var commonjsGlobal = typeof globalThis !== 'undefined' ? globalThis : typeof window !== 'undefined' ? window : typeof global !== 'undefined' ? global : typeof self !== 'undefined' ? self : {};

var lib$1 = {exports: {}};

var util$1 = {};

var types$4 = {
  ROOT       : 0,
  GROUP      : 1,
  POSITION   : 2,
  SET        : 3,
  RANGE      : 4,
  REPETITION : 5,
  REFERENCE  : 6,
  CHAR       : 7,
};

var sets$1 = {};

const types$3 = types$4;

const INTS = () => [{ type: types$3.RANGE , from: 48, to: 57 }];

const WORDS = () => {
  return [
    { type: types$3.CHAR, value: 95 },
    { type: types$3.RANGE, from: 97, to: 122 },
    { type: types$3.RANGE, from: 65, to: 90 }
  ].concat(INTS());
};

const WHITESPACE = () => {
  return [
    { type: types$3.CHAR, value: 9 },
    { type: types$3.CHAR, value: 10 },
    { type: types$3.CHAR, value: 11 },
    { type: types$3.CHAR, value: 12 },
    { type: types$3.CHAR, value: 13 },
    { type: types$3.CHAR, value: 32 },
    { type: types$3.CHAR, value: 160 },
    { type: types$3.CHAR, value: 5760 },
    { type: types$3.RANGE, from: 8192, to: 8202 },
    { type: types$3.CHAR, value: 8232 },
    { type: types$3.CHAR, value: 8233 },
    { type: types$3.CHAR, value: 8239 },
    { type: types$3.CHAR, value: 8287 },
    { type: types$3.CHAR, value: 12288 },
    { type: types$3.CHAR, value: 65279 }
  ];
};

const NOTANYCHAR = () => {
  return [
    { type: types$3.CHAR, value: 10 },
    { type: types$3.CHAR, value: 13 },
    { type: types$3.CHAR, value: 8232 },
    { type: types$3.CHAR, value: 8233 },
  ];
};

// Predefined class objects.
sets$1.words = () => ({ type: types$3.SET, set: WORDS(), not: false });
sets$1.notWords = () => ({ type: types$3.SET, set: WORDS(), not: true });
sets$1.ints = () => ({ type: types$3.SET, set: INTS(), not: false });
sets$1.notInts = () => ({ type: types$3.SET, set: INTS(), not: true });
sets$1.whitespace = () => ({ type: types$3.SET, set: WHITESPACE(), not: false });
sets$1.notWhitespace = () => ({ type: types$3.SET, set: WHITESPACE(), not: true });
sets$1.anyChar = () => ({ type: types$3.SET, set: NOTANYCHAR(), not: true });

(function (exports) {
const types = types$4;
const sets  = sets$1;


const CTRL = '@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^ ?';
const SLSH = { '0': 0, 't': 9, 'n': 10, 'v': 11, 'f': 12, 'r': 13 };

/**
 * Finds character representations in str and convert all to
 * their respective characters
 *
 * @param {String} str
 * @return {String}
 */
exports.strToChars = function(str) {
  /* jshint maxlen: false */
  var chars_regex = /(\[\\b\])|(\\)?\\(?:u([A-F0-9]{4})|x([A-F0-9]{2})|(0?[0-7]{2})|c([@A-Z[\\\]^?])|([0tnvfr]))/g;
  str = str.replace(chars_regex, function(s, b, lbs, a16, b16, c8, dctrl, eslsh) {
    if (lbs) {
      return s;
    }

    var code = b ? 8 :
      a16   ? parseInt(a16, 16) :
      b16   ? parseInt(b16, 16) :
      c8    ? parseInt(c8,   8) :
      dctrl ? CTRL.indexOf(dctrl) :
      SLSH[eslsh];

    var c = String.fromCharCode(code);

    // Escape special regex characters.
    if (/[[\]{}^$.|?*+()]/.test(c)) {
      c = '\\' + c;
    }

    return c;
  });

  return str;
};


/**
 * turns class into tokens
 * reads str until it encounters a ] not preceeded by a \
 *
 * @param {String} str
 * @param {String} regexpStr
 * @return {Array.<Array.<Object>, Number>}
 */
exports.tokenizeClass = (str, regexpStr) => {
  /* jshint maxlen: false */
  var tokens = [];
  var regexp = /\\(?:(w)|(d)|(s)|(W)|(D)|(S))|((?:(?:\\)(.)|([^\]\\]))-(?:\\)?([^\]]))|(\])|(?:\\)?([^])/g;
  var rs, c;


  while ((rs = regexp.exec(str)) != null) {
    if (rs[1]) {
      tokens.push(sets.words());

    } else if (rs[2]) {
      tokens.push(sets.ints());

    } else if (rs[3]) {
      tokens.push(sets.whitespace());

    } else if (rs[4]) {
      tokens.push(sets.notWords());

    } else if (rs[5]) {
      tokens.push(sets.notInts());

    } else if (rs[6]) {
      tokens.push(sets.notWhitespace());

    } else if (rs[7]) {
      tokens.push({
        type: types.RANGE,
        from: (rs[8] || rs[9]).charCodeAt(0),
        to: rs[10].charCodeAt(0),
      });

    } else if ((c = rs[12])) {
      tokens.push({
        type: types.CHAR,
        value: c.charCodeAt(0),
      });

    } else {
      return [tokens, regexp.lastIndex];
    }
  }

  exports.error(regexpStr, 'Unterminated character class');
};


/**
 * Shortcut to throw errors.
 *
 * @param {String} regexp
 * @param {String} msg
 */
exports.error = (regexp, msg) => {
  throw new SyntaxError('Invalid regular expression: /' + regexp + '/: ' + msg);
};
}(util$1));

var positions$1 = {};

const types$2 = types$4;
positions$1.wordBoundary = () => ({ type: types$2.POSITION, value: 'b' });
positions$1.nonWordBoundary = () => ({ type: types$2.POSITION, value: 'B' });
positions$1.begin = () => ({ type: types$2.POSITION, value: '^' });
positions$1.end = () => ({ type: types$2.POSITION, value: '$' });

const util      = util$1;
const types$1     = types$4;
const sets      = sets$1;
const positions = positions$1;


lib$1.exports = (regexpStr) => {
  var i = 0, l, c,
    start = { type: types$1.ROOT, stack: []},

    // Keep track of last clause/group and stack.
    lastGroup = start,
    last = start.stack,
    groupStack = [];


  var repeatErr = (i) => {
    util.error(regexpStr, `Nothing to repeat at column ${i - 1}`);
  };

  // Decode a few escaped characters.
  var str = util.strToChars(regexpStr);
  l = str.length;

  // Iterate through each character in string.
  while (i < l) {
    c = str[i++];

    switch (c) {
      // Handle escaped characters, inclues a few sets.
      case '\\':
        c = str[i++];

        switch (c) {
          case 'b':
            last.push(positions.wordBoundary());
            break;

          case 'B':
            last.push(positions.nonWordBoundary());
            break;

          case 'w':
            last.push(sets.words());
            break;

          case 'W':
            last.push(sets.notWords());
            break;

          case 'd':
            last.push(sets.ints());
            break;

          case 'D':
            last.push(sets.notInts());
            break;

          case 's':
            last.push(sets.whitespace());
            break;

          case 'S':
            last.push(sets.notWhitespace());
            break;

          default:
            // Check if c is integer.
            // In which case it's a reference.
            if (/\d/.test(c)) {
              last.push({ type: types$1.REFERENCE, value: parseInt(c, 10) });

            // Escaped character.
            } else {
              last.push({ type: types$1.CHAR, value: c.charCodeAt(0) });
            }
        }

        break;


      // Positionals.
      case '^':
        last.push(positions.begin());
        break;

      case '$':
        last.push(positions.end());
        break;


      // Handle custom sets.
      case '[':
        // Check if this class is 'anti' i.e. [^abc].
        var not;
        if (str[i] === '^') {
          not = true;
          i++;
        } else {
          not = false;
        }

        // Get all the characters in class.
        var classTokens = util.tokenizeClass(str.slice(i), regexpStr);

        // Increase index by length of class.
        i += classTokens[1];
        last.push({
          type: types$1.SET,
          set: classTokens[0],
          not,
        });

        break;


      // Class of any character except \n.
      case '.':
        last.push(sets.anyChar());
        break;


      // Push group onto stack.
      case '(':
        // Create group.
        var group = {
          type: types$1.GROUP,
          stack: [],
          remember: true,
        };

        c = str[i];

        // If if this is a special kind of group.
        if (c === '?') {
          c = str[i + 1];
          i += 2;

          // Match if followed by.
          if (c === '=') {
            group.followedBy = true;

          // Match if not followed by.
          } else if (c === '!') {
            group.notFollowedBy = true;

          } else if (c !== ':') {
            util.error(regexpStr,
              `Invalid group, character '${c}'` +
              ` after '?' at column ${i - 1}`);
          }

          group.remember = false;
        }

        // Insert subgroup into current group stack.
        last.push(group);

        // Remember the current group for when the group closes.
        groupStack.push(lastGroup);

        // Make this new group the current group.
        lastGroup = group;
        last = group.stack;
        break;


      // Pop group out of stack.
      case ')':
        if (groupStack.length === 0) {
          util.error(regexpStr, `Unmatched ) at column ${i - 1}`);
        }
        lastGroup = groupStack.pop();

        // Check if this group has a PIPE.
        // To get back the correct last stack.
        last = lastGroup.options ?
          lastGroup.options[lastGroup.options.length - 1] : lastGroup.stack;
        break;


      // Use pipe character to give more choices.
      case '|':
        // Create array where options are if this is the first PIPE
        // in this clause.
        if (!lastGroup.options) {
          lastGroup.options = [lastGroup.stack];
          delete lastGroup.stack;
        }

        // Create a new stack and add to options for rest of clause.
        var stack = [];
        lastGroup.options.push(stack);
        last = stack;
        break;


      // Repetition.
      // For every repetition, remove last element from last stack
      // then insert back a RANGE object.
      // This design is chosen because there could be more than
      // one repetition symbols in a regex i.e. `a?+{2,3}`.
      case '{':
        var rs = /^(\d+)(,(\d+)?)?\}/.exec(str.slice(i)), min, max;
        if (rs !== null) {
          if (last.length === 0) {
            repeatErr(i);
          }
          min = parseInt(rs[1], 10);
          max = rs[2] ? rs[3] ? parseInt(rs[3], 10) : Infinity : min;
          i += rs[0].length;

          last.push({
            type: types$1.REPETITION,
            min,
            max,
            value: last.pop(),
          });
        } else {
          last.push({
            type: types$1.CHAR,
            value: 123,
          });
        }
        break;

      case '?':
        if (last.length === 0) {
          repeatErr(i);
        }
        last.push({
          type: types$1.REPETITION,
          min: 0,
          max: 1,
          value: last.pop(),
        });
        break;

      case '+':
        if (last.length === 0) {
          repeatErr(i);
        }
        last.push({
          type: types$1.REPETITION,
          min: 1,
          max: Infinity,
          value: last.pop(),
        });
        break;

      case '*':
        if (last.length === 0) {
          repeatErr(i);
        }
        last.push({
          type: types$1.REPETITION,
          min: 0,
          max: Infinity,
          value: last.pop(),
        });
        break;


      // Default is a character that is not `\[](){}?+*^$`.
      default:
        last.push({
          type: types$1.CHAR,
          value: c.charCodeAt(0),
        });
    }

  }

  // Check if any groups have not been closed.
  if (groupStack.length !== 0) {
    util.error(regexpStr, 'Unterminated group');
  }

  return start;
};

lib$1.exports.types = types$1;

/* eslint indent: 4 */


// Private helper class
class SubRange {
    constructor(low, high) {
        this.low = low;
        this.high = high;
        this.length = 1 + high - low;
    }

    overlaps(range) {
        return !(this.high < range.low || this.low > range.high);
    }

    touches(range) {
        return !(this.high + 1 < range.low || this.low - 1 > range.high);
    }

    // Returns inclusive combination of SubRanges as a SubRange.
    add(range) {
        return new SubRange(
            Math.min(this.low, range.low),
            Math.max(this.high, range.high)
        );
    }

    // Returns subtraction of SubRanges as an array of SubRanges.
    // (There's a case where subtraction divides it in 2)
    subtract(range) {
        if (range.low <= this.low && range.high >= this.high) {
            return [];
        } else if (range.low > this.low && range.high < this.high) {
            return [
                new SubRange(this.low, range.low - 1),
                new SubRange(range.high + 1, this.high)
            ];
        } else if (range.low <= this.low) {
            return [new SubRange(range.high + 1, this.high)];
        } else {
            return [new SubRange(this.low, range.low - 1)];
        }
    }

    toString() {
        return this.low == this.high ?
            this.low.toString() : this.low + '-' + this.high;
    }
}


class DRange$1 {
    constructor(a, b) {
        this.ranges = [];
        this.length = 0;
        if (a != null) this.add(a, b);
    }

    _update_length() {
        this.length = this.ranges.reduce((previous, range) => {
            return previous + range.length;
        }, 0);
    }

    add(a, b) {
        var _add = (subrange) => {
            var i = 0;
            while (i < this.ranges.length && !subrange.touches(this.ranges[i])) {
                i++;
            }
            var newRanges = this.ranges.slice(0, i);
            while (i < this.ranges.length && subrange.touches(this.ranges[i])) {
                subrange = subrange.add(this.ranges[i]);
                i++;
            }
            newRanges.push(subrange);
            this.ranges = newRanges.concat(this.ranges.slice(i));
            this._update_length();
        };

        if (a instanceof DRange$1) {
            a.ranges.forEach(_add);
        } else {
            if (b == null) b = a;
            _add(new SubRange(a, b));
        }
        return this;
    }

    subtract(a, b) {
        var _subtract = (subrange) => {
            var i = 0;
            while (i < this.ranges.length && !subrange.overlaps(this.ranges[i])) {
                i++;
            }
            var newRanges = this.ranges.slice(0, i);
            while (i < this.ranges.length && subrange.overlaps(this.ranges[i])) {
                newRanges = newRanges.concat(this.ranges[i].subtract(subrange));
                i++;
            }
            this.ranges = newRanges.concat(this.ranges.slice(i));
            this._update_length();
        };

        if (a instanceof DRange$1) {
            a.ranges.forEach(_subtract);
        } else {
            if (b == null) b = a;
            _subtract(new SubRange(a, b));
        }
        return this;
    }

    intersect(a, b) {
        var newRanges = [];
        var _intersect = (subrange) => {
            var i = 0;
            while (i < this.ranges.length && !subrange.overlaps(this.ranges[i])) {
                i++;
            }
            while (i < this.ranges.length && subrange.overlaps(this.ranges[i])) {
                var low = Math.max(this.ranges[i].low, subrange.low);
                var high = Math.min(this.ranges[i].high, subrange.high);
                newRanges.push(new SubRange(low, high));
                i++;
            }
        };

        if (a instanceof DRange$1) {
            a.ranges.forEach(_intersect);
        } else {
            if (b == null) b = a;
            _intersect(new SubRange(a, b));
        }
        this.ranges = newRanges;
        this._update_length();
        return this;
    }

    index(index) {
        var i = 0;
        while (i < this.ranges.length && this.ranges[i].length <= index) {
            index -= this.ranges[i].length;
            i++;
        }
        return this.ranges[i].low + index;
    }

    toString() {
        return '[ ' + this.ranges.join(', ') + ' ]';
    }

    clone() {
        return new DRange$1(this);
    }

    numbers() {
        return this.ranges.reduce((result, subrange) => {
            var i = subrange.low;
            while (i <= subrange.high) {
                result.push(i);
                i++;
            }
            return result;
        }, []);
    }

    subranges() {
        return this.ranges.map((subrange) => ({
            low: subrange.low,
            high: subrange.high,
            length: 1 + subrange.high - subrange.low
        }));
    }
}

var lib = DRange$1;

const ret    = lib$1.exports;
const DRange = lib;
const types  = ret.types;


var randexp = class RandExp {
  /**
   * @constructor
   * @param {RegExp|String} regexp
   * @param {String} m
   */
  constructor(regexp, m) {
    this._setDefaults(regexp);
    if (regexp instanceof RegExp) {
      this.ignoreCase = regexp.ignoreCase;
      this.multiline = regexp.multiline;
      regexp = regexp.source;

    } else if (typeof regexp === 'string') {
      this.ignoreCase = m && m.indexOf('i') !== -1;
      this.multiline = m && m.indexOf('m') !== -1;
    } else {
      throw new Error('Expected a regexp or string');
    }

    this.tokens = ret(regexp);
  }


  /**
   * Checks if some custom properties have been set for this regexp.
   *
   * @param {RandExp} randexp
   * @param {RegExp} regexp
   */
  _setDefaults(regexp) {
    // When a repetitional token has its max set to Infinite,
    // randexp won't actually generate a random amount between min and Infinite
    // instead it will see Infinite as min + 100.
    this.max = regexp.max != null ? regexp.max :
      RandExp.prototype.max != null ? RandExp.prototype.max : 100;

    // This allows expanding to include additional characters
    // for instance: RandExp.defaultRange.add(0, 65535);
    this.defaultRange = regexp.defaultRange ?
      regexp.defaultRange : this.defaultRange.clone();

    if (regexp.randInt) {
      this.randInt = regexp.randInt;
    }
  }


  /**
   * Generates the random string.
   *
   * @return {String}
   */
  gen() {
    return this._gen(this.tokens, []);
  }


  /**
   * Generate random string modeled after given tokens.
   *
   * @param {Object} token
   * @param {Array.<String>} groups
   * @return {String}
   */
  _gen(token, groups) {
    var stack, str, n, i, l;

    switch (token.type) {
      case types.ROOT:
      case types.GROUP:
        // Ignore lookaheads for now.
        if (token.followedBy || token.notFollowedBy) { return ''; }

        // Insert placeholder until group string is generated.
        if (token.remember && token.groupNumber === undefined) {
          token.groupNumber = groups.push(null) - 1;
        }

        stack = token.options ?
          this._randSelect(token.options) : token.stack;

        str = '';
        for (i = 0, l = stack.length; i < l; i++) {
          str += this._gen(stack[i], groups);
        }

        if (token.remember) {
          groups[token.groupNumber] = str;
        }
        return str;

      case types.POSITION:
        // Do nothing for now.
        return '';

      case types.SET:
        var expandedSet = this._expand(token);
        if (!expandedSet.length) { return ''; }
        return String.fromCharCode(this._randSelect(expandedSet));

      case types.REPETITION:
        // Randomly generate number between min and max.
        n = this.randInt(token.min,
          token.max === Infinity ? token.min + this.max : token.max);

        str = '';
        for (i = 0; i < n; i++) {
          str += this._gen(token.value, groups);
        }

        return str;

      case types.REFERENCE:
        return groups[token.value - 1] || '';

      case types.CHAR:
        var code = this.ignoreCase && this._randBool() ?
          this._toOtherCase(token.value) : token.value;
        return String.fromCharCode(code);
    }
  }


  /**
   * If code is alphabetic, converts to other case.
   * If not alphabetic, returns back code.
   *
   * @param {Number} code
   * @return {Number}
   */
  _toOtherCase(code) {
    return code + (97 <= code && code <= 122 ? -32 :
      65 <= code && code <= 90  ?  32 : 0);
  }


  /**
   * Randomly returns a true or false value.
   *
   * @return {Boolean}
   */
  _randBool() {
    return !this.randInt(0, 1);
  }


  /**
   * Randomly selects and returns a value from the array.
   *
   * @param {Array.<Object>} arr
   * @return {Object}
   */
  _randSelect(arr) {
    if (arr instanceof DRange) {
      return arr.index(this.randInt(0, arr.length - 1));
    }
    return arr[this.randInt(0, arr.length - 1)];
  }


  /**
   * expands a token to a DiscontinuousRange of characters which has a
   * length and an index function (for random selecting)
   *
   * @param {Object} token
   * @return {DiscontinuousRange}
   */
  _expand(token) {
    if (token.type === ret.types.CHAR) {
      return new DRange(token.value);
    } else if (token.type === ret.types.RANGE) {
      return new DRange(token.from, token.to);
    } else {
      let drange = new DRange();
      for (let i = 0; i < token.set.length; i++) {
        let subrange = this._expand(token.set[i]);
        drange.add(subrange);
        if (this.ignoreCase) {
          for (let j = 0; j < subrange.length; j++) {
            let code = subrange.index(j);
            let otherCaseCode = this._toOtherCase(code);
            if (code !== otherCaseCode) {
              drange.add(otherCaseCode);
            }
          }
        }
      }
      if (token.not) {
        return this.defaultRange.clone().subtract(drange);
      } else {
        return this.defaultRange.clone().intersect(drange);
      }
    }
  }


  /**
   * Randomly generates and returns a number between a and b (inclusive).
   *
   * @param {Number} a
   * @param {Number} b
   * @return {Number}
   */
  randInt(a, b) {
    return a + Math.floor(Math.random() * (1 + b - a));
  }


  /**
   * Default range of characters to generate from.
   */
  get defaultRange() {
    return this._range = this._range || new DRange(32, 126);
  }

  set defaultRange(range) {
    this._range = range;
  }


  /**
   *
   * Enables use of randexp with a shorter call.
   *
   * @param {RegExp|String| regexp}
   * @param {String} m
   * @return {String}
   */
  static randexp(regexp, m) {
    var randexp;
    if(typeof regexp === 'string') {
      regexp = new RegExp(regexp, m);
    }

    if (regexp._randexp === undefined) {
      randexp = new RandExp(regexp, m);
      regexp._randexp = randexp;
    } else {
      randexp = regexp._randexp;
      randexp._setDefaults(regexp);
    }
    return randexp.gen();
  }


  /**
   * Enables sugary /regexp/.gen syntax.
   */
  static sugar() {
    /* eshint freeze:false */
    RegExp.prototype.gen = function() {
      return RandExp.randexp(this);
    };
  }
};

var regexp = function regexp(_ref) {
  var _ref$rule$count = _ref.rule.count,
      count = _ref$rule$count === void 0 ? 1 : _ref$rule$count,
      template = _ref.template;
  // 'name': /regexp/,
  // 'name|1-5': /regexp/
  var gen = new randexp(template);
  return count === 1 ? gen.gen() : times(count, function () {
    return gen.gen();
  });
};

var lower$1 = 'abcdefghijklmnopqrstuvwxyz'.split('');
var upper$1 = lower$1.map(function (i) {
  return i.toUpperCase();
});
var number = '0123456789'.split('');
var symbol = '!@#$%^&*()[]'.split('');
var characterPools = {
  lower: lower$1,
  upper: upper$1,
  number: number,
  symbol: symbol,
  alpha: lower$1.concat(upper$1),
  "default": [].concat(_toConsumableArray(lower$1), _toConsumableArray(upper$1), _toConsumableArray(number), _toConsumableArray(symbol))
};

var character = decode(function () {
  var pool = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : 'default';
  pool = pool.toLowerCase();

  if (pool in characterPools) {
    return sample(characterPools[pool]);
  } else {
    return sample(pool.split(''));
  }
}, ['json']);

function _string() {
  var len = 0;
  var pool;
  var min;
  var max;

  for (var _len = arguments.length, args = new Array(_len), _key = 0; _key < _len; _key++) {
    args[_key] = arguments[_key];
  }

  switch (args.length) {
    case 0:
      // ()
      len = random(3, 7, false);
      break;

    case 1:
      // ( length )
      len = args[0];
      break;

    case 2:
      // ( pool, length )
      if (typeof args[0] === 'string') {
        pool = args[0];
        len = args[1];
      } else {
        // ( min, max )
        min = args[0];
        max = args[1];
        len = random(min, max, false);
      }

      break;

    case 3:
      // ( pool, min, max )
      pool = args[0];
      min = args[1];
      max = args[2];
      len = random(min, max, false);
      break;
  }

  return times(len, function () {
    return character(pool);
  }).join('');
}
var upper = function upper(str) {
  return str.toUpperCase();
};
var lower = function lower(str) {
  return str.toLowerCase();
};
var string$1 = decode(_string, ['number', 'number', 'number']);

var range = decode(_range, ['number', 'number', 'number']);

var chroma$1 = {exports: {}};

/**
 * chroma.js - JavaScript library for color conversions
 *
 * Copyright (c) 2011-2019, Gregor Aisch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name Gregor Aisch may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GREGOR AISCH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * -------------------------------------------------------
 *
 * chroma.js includes colors from colorbrewer2.org, which are released under
 * the following license:
 *
 * Copyright (c) 2002 Cynthia Brewer, Mark Harrower,
 * and The Pennsylvania State University.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 * ------------------------------------------------------
 *
 * Named colors are taken from X11 Color Names.
 * http://www.w3.org/TR/css3-color/#svg-color
 *
 * @preserve
 */

(function (module, exports) {
(function (global, factory) {
    module.exports = factory() ;
})(commonjsGlobal, (function () {
    var limit$2 = function (x, min, max) {
        if ( min === void 0 ) min=0;
        if ( max === void 0 ) max=1;

        return x < min ? min : x > max ? max : x;
    };

    var limit$1 = limit$2;

    var clip_rgb$3 = function (rgb) {
        rgb._clipped = false;
        rgb._unclipped = rgb.slice(0);
        for (var i=0; i<=3; i++) {
            if (i < 3) {
                if (rgb[i] < 0 || rgb[i] > 255) { rgb._clipped = true; }
                rgb[i] = limit$1(rgb[i], 0, 255);
            } else if (i === 3) {
                rgb[i] = limit$1(rgb[i], 0, 1);
            }
        }
        return rgb;
    };

    // ported from jQuery's $.type
    var classToType = {};
    for (var i$1 = 0, list$1 = ['Boolean', 'Number', 'String', 'Function', 'Array', 'Date', 'RegExp', 'Undefined', 'Null']; i$1 < list$1.length; i$1 += 1) {
        var name = list$1[i$1];

        classToType[("[object " + name + "]")] = name.toLowerCase();
    }
    var type$p = function(obj) {
        return classToType[Object.prototype.toString.call(obj)] || "object";
    };

    var type$o = type$p;

    var unpack$B = function (args, keyOrder) {
        if ( keyOrder === void 0 ) keyOrder=null;

    	// if called with more than 3 arguments, we return the arguments
        if (args.length >= 3) { return Array.prototype.slice.call(args); }
        // with less than 3 args we check if first arg is object
        // and use the keyOrder string to extract and sort properties
    	if (type$o(args[0]) == 'object' && keyOrder) {
    		return keyOrder.split('')
    			.filter(function (k) { return args[0][k] !== undefined; })
    			.map(function (k) { return args[0][k]; });
    	}
    	// otherwise we just return the first argument
    	// (which we suppose is an array of args)
        return args[0];
    };

    var type$n = type$p;

    var last$4 = function (args) {
        if (args.length < 2) { return null; }
        var l = args.length-1;
        if (type$n(args[l]) == 'string') { return args[l].toLowerCase(); }
        return null;
    };

    var PI$2 = Math.PI;

    var utils = {
    	clip_rgb: clip_rgb$3,
    	limit: limit$2,
    	type: type$p,
    	unpack: unpack$B,
    	last: last$4,
    	PI: PI$2,
    	TWOPI: PI$2*2,
    	PITHIRD: PI$2/3,
    	DEG2RAD: PI$2 / 180,
    	RAD2DEG: 180 / PI$2
    };

    var input$h = {
    	format: {},
    	autodetect: []
    };

    var last$3 = utils.last;
    var clip_rgb$2 = utils.clip_rgb;
    var type$m = utils.type;
    var _input = input$h;

    var Color$D = function Color() {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        var me = this;
        if (type$m(args[0]) === 'object' &&
            args[0].constructor &&
            args[0].constructor === this.constructor) {
            // the argument is already a Color instance
            return args[0];
        }

        // last argument could be the mode
        var mode = last$3(args);
        var autodetect = false;

        if (!mode) {
            autodetect = true;
            if (!_input.sorted) {
                _input.autodetect = _input.autodetect.sort(function (a,b) { return b.p - a.p; });
                _input.sorted = true;
            }
            // auto-detect format
            for (var i = 0, list = _input.autodetect; i < list.length; i += 1) {
                var chk = list[i];

                mode = chk.test.apply(chk, args);
                if (mode) { break; }
            }
        }

        if (_input.format[mode]) {
            var rgb = _input.format[mode].apply(null, autodetect ? args : args.slice(0,-1));
            me._rgb = clip_rgb$2(rgb);
        } else {
            throw new Error('unknown format: '+args);
        }

        // add alpha channel
        if (me._rgb.length === 3) { me._rgb.push(1); }
    };

    Color$D.prototype.toString = function toString () {
        if (type$m(this.hex) == 'function') { return this.hex(); }
        return ("[" + (this._rgb.join(',')) + "]");
    };

    var Color_1 = Color$D;

    var chroma$k = function () {
    	var args = [], len = arguments.length;
    	while ( len-- ) args[ len ] = arguments[ len ];

    	return new (Function.prototype.bind.apply( chroma$k.Color, [ null ].concat( args) ));
    };

    chroma$k.Color = Color_1;
    chroma$k.version = '2.4.2';

    var chroma_1 = chroma$k;

    var unpack$A = utils.unpack;
    var max$2 = Math.max;

    var rgb2cmyk$1 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        var ref = unpack$A(args, 'rgb');
        var r = ref[0];
        var g = ref[1];
        var b = ref[2];
        r = r / 255;
        g = g / 255;
        b = b / 255;
        var k = 1 - max$2(r,max$2(g,b));
        var f = k < 1 ? 1 / (1-k) : 0;
        var c = (1-r-k) * f;
        var m = (1-g-k) * f;
        var y = (1-b-k) * f;
        return [c,m,y,k];
    };

    var rgb2cmyk_1 = rgb2cmyk$1;

    var unpack$z = utils.unpack;

    var cmyk2rgb = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        args = unpack$z(args, 'cmyk');
        var c = args[0];
        var m = args[1];
        var y = args[2];
        var k = args[3];
        var alpha = args.length > 4 ? args[4] : 1;
        if (k === 1) { return [0,0,0,alpha]; }
        return [
            c >= 1 ? 0 : 255 * (1-c) * (1-k), // r
            m >= 1 ? 0 : 255 * (1-m) * (1-k), // g
            y >= 1 ? 0 : 255 * (1-y) * (1-k), // b
            alpha
        ];
    };

    var cmyk2rgb_1 = cmyk2rgb;

    var chroma$j = chroma_1;
    var Color$C = Color_1;
    var input$g = input$h;
    var unpack$y = utils.unpack;
    var type$l = utils.type;

    var rgb2cmyk = rgb2cmyk_1;

    Color$C.prototype.cmyk = function() {
        return rgb2cmyk(this._rgb);
    };

    chroma$j.cmyk = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        return new (Function.prototype.bind.apply( Color$C, [ null ].concat( args, ['cmyk']) ));
    };

    input$g.format.cmyk = cmyk2rgb_1;

    input$g.autodetect.push({
        p: 2,
        test: function () {
            var args = [], len = arguments.length;
            while ( len-- ) args[ len ] = arguments[ len ];

            args = unpack$y(args, 'cmyk');
            if (type$l(args) === 'array' && args.length === 4) {
                return 'cmyk';
            }
        }
    });

    var unpack$x = utils.unpack;
    var last$2 = utils.last;
    var rnd = function (a) { return Math.round(a*100)/100; };

    /*
     * supported arguments:
     * - hsl2css(h,s,l)
     * - hsl2css(h,s,l,a)
     * - hsl2css([h,s,l], mode)
     * - hsl2css([h,s,l,a], mode)
     * - hsl2css({h,s,l,a}, mode)
     */
    var hsl2css$1 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        var hsla = unpack$x(args, 'hsla');
        var mode = last$2(args) || 'lsa';
        hsla[0] = rnd(hsla[0] || 0);
        hsla[1] = rnd(hsla[1]*100) + '%';
        hsla[2] = rnd(hsla[2]*100) + '%';
        if (mode === 'hsla' || (hsla.length > 3 && hsla[3]<1)) {
            hsla[3] = hsla.length > 3 ? hsla[3] : 1;
            mode = 'hsla';
        } else {
            hsla.length = 3;
        }
        return (mode + "(" + (hsla.join(',')) + ")");
    };

    var hsl2css_1 = hsl2css$1;

    var unpack$w = utils.unpack;

    /*
     * supported arguments:
     * - rgb2hsl(r,g,b)
     * - rgb2hsl(r,g,b,a)
     * - rgb2hsl([r,g,b])
     * - rgb2hsl([r,g,b,a])
     * - rgb2hsl({r,g,b,a})
     */
    var rgb2hsl$3 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        args = unpack$w(args, 'rgba');
        var r = args[0];
        var g = args[1];
        var b = args[2];

        r /= 255;
        g /= 255;
        b /= 255;

        var min = Math.min(r, g, b);
        var max = Math.max(r, g, b);

        var l = (max + min) / 2;
        var s, h;

        if (max === min){
            s = 0;
            h = Number.NaN;
        } else {
            s = l < 0.5 ? (max - min) / (max + min) : (max - min) / (2 - max - min);
        }

        if (r == max) { h = (g - b) / (max - min); }
        else if (g == max) { h = 2 + (b - r) / (max - min); }
        else if (b == max) { h = 4 + (r - g) / (max - min); }

        h *= 60;
        if (h < 0) { h += 360; }
        if (args.length>3 && args[3]!==undefined) { return [h,s,l,args[3]]; }
        return [h,s,l];
    };

    var rgb2hsl_1 = rgb2hsl$3;

    var unpack$v = utils.unpack;
    var last$1 = utils.last;
    var hsl2css = hsl2css_1;
    var rgb2hsl$2 = rgb2hsl_1;
    var round$6 = Math.round;

    /*
     * supported arguments:
     * - rgb2css(r,g,b)
     * - rgb2css(r,g,b,a)
     * - rgb2css([r,g,b], mode)
     * - rgb2css([r,g,b,a], mode)
     * - rgb2css({r,g,b,a}, mode)
     */
    var rgb2css$1 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        var rgba = unpack$v(args, 'rgba');
        var mode = last$1(args) || 'rgb';
        if (mode.substr(0,3) == 'hsl') {
            return hsl2css(rgb2hsl$2(rgba), mode);
        }
        rgba[0] = round$6(rgba[0]);
        rgba[1] = round$6(rgba[1]);
        rgba[2] = round$6(rgba[2]);
        if (mode === 'rgba' || (rgba.length > 3 && rgba[3]<1)) {
            rgba[3] = rgba.length > 3 ? rgba[3] : 1;
            mode = 'rgba';
        }
        return (mode + "(" + (rgba.slice(0,mode==='rgb'?3:4).join(',')) + ")");
    };

    var rgb2css_1 = rgb2css$1;

    var unpack$u = utils.unpack;
    var round$5 = Math.round;

    var hsl2rgb$1 = function () {
        var assign;

        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];
        args = unpack$u(args, 'hsl');
        var h = args[0];
        var s = args[1];
        var l = args[2];
        var r,g,b;
        if (s === 0) {
            r = g = b = l*255;
        } else {
            var t3 = [0,0,0];
            var c = [0,0,0];
            var t2 = l < 0.5 ? l * (1+s) : l+s-l*s;
            var t1 = 2 * l - t2;
            var h_ = h / 360;
            t3[0] = h_ + 1/3;
            t3[1] = h_;
            t3[2] = h_ - 1/3;
            for (var i=0; i<3; i++) {
                if (t3[i] < 0) { t3[i] += 1; }
                if (t3[i] > 1) { t3[i] -= 1; }
                if (6 * t3[i] < 1)
                    { c[i] = t1 + (t2 - t1) * 6 * t3[i]; }
                else if (2 * t3[i] < 1)
                    { c[i] = t2; }
                else if (3 * t3[i] < 2)
                    { c[i] = t1 + (t2 - t1) * ((2 / 3) - t3[i]) * 6; }
                else
                    { c[i] = t1; }
            }
            (assign = [round$5(c[0]*255),round$5(c[1]*255),round$5(c[2]*255)], r = assign[0], g = assign[1], b = assign[2]);
        }
        if (args.length > 3) {
            // keep alpha channel
            return [r,g,b,args[3]];
        }
        return [r,g,b,1];
    };

    var hsl2rgb_1 = hsl2rgb$1;

    var hsl2rgb = hsl2rgb_1;
    var input$f = input$h;

    var RE_RGB = /^rgb\(\s*(-?\d+),\s*(-?\d+)\s*,\s*(-?\d+)\s*\)$/;
    var RE_RGBA = /^rgba\(\s*(-?\d+),\s*(-?\d+)\s*,\s*(-?\d+)\s*,\s*([01]|[01]?\.\d+)\)$/;
    var RE_RGB_PCT = /^rgb\(\s*(-?\d+(?:\.\d+)?)%,\s*(-?\d+(?:\.\d+)?)%\s*,\s*(-?\d+(?:\.\d+)?)%\s*\)$/;
    var RE_RGBA_PCT = /^rgba\(\s*(-?\d+(?:\.\d+)?)%,\s*(-?\d+(?:\.\d+)?)%\s*,\s*(-?\d+(?:\.\d+)?)%\s*,\s*([01]|[01]?\.\d+)\)$/;
    var RE_HSL = /^hsl\(\s*(-?\d+(?:\.\d+)?),\s*(-?\d+(?:\.\d+)?)%\s*,\s*(-?\d+(?:\.\d+)?)%\s*\)$/;
    var RE_HSLA = /^hsla\(\s*(-?\d+(?:\.\d+)?),\s*(-?\d+(?:\.\d+)?)%\s*,\s*(-?\d+(?:\.\d+)?)%\s*,\s*([01]|[01]?\.\d+)\)$/;

    var round$4 = Math.round;

    var css2rgb$1 = function (css) {
        css = css.toLowerCase().trim();
        var m;

        if (input$f.format.named) {
            try {
                return input$f.format.named(css);
            } catch (e) {
                // eslint-disable-next-line
            }
        }

        // rgb(250,20,0)
        if ((m = css.match(RE_RGB))) {
            var rgb = m.slice(1,4);
            for (var i=0; i<3; i++) {
                rgb[i] = +rgb[i];
            }
            rgb[3] = 1;  // default alpha
            return rgb;
        }

        // rgba(250,20,0,0.4)
        if ((m = css.match(RE_RGBA))) {
            var rgb$1 = m.slice(1,5);
            for (var i$1=0; i$1<4; i$1++) {
                rgb$1[i$1] = +rgb$1[i$1];
            }
            return rgb$1;
        }

        // rgb(100%,0%,0%)
        if ((m = css.match(RE_RGB_PCT))) {
            var rgb$2 = m.slice(1,4);
            for (var i$2=0; i$2<3; i$2++) {
                rgb$2[i$2] = round$4(rgb$2[i$2] * 2.55);
            }
            rgb$2[3] = 1;  // default alpha
            return rgb$2;
        }

        // rgba(100%,0%,0%,0.4)
        if ((m = css.match(RE_RGBA_PCT))) {
            var rgb$3 = m.slice(1,5);
            for (var i$3=0; i$3<3; i$3++) {
                rgb$3[i$3] = round$4(rgb$3[i$3] * 2.55);
            }
            rgb$3[3] = +rgb$3[3];
            return rgb$3;
        }

        // hsl(0,100%,50%)
        if ((m = css.match(RE_HSL))) {
            var hsl = m.slice(1,4);
            hsl[1] *= 0.01;
            hsl[2] *= 0.01;
            var rgb$4 = hsl2rgb(hsl);
            rgb$4[3] = 1;
            return rgb$4;
        }

        // hsla(0,100%,50%,0.5)
        if ((m = css.match(RE_HSLA))) {
            var hsl$1 = m.slice(1,4);
            hsl$1[1] *= 0.01;
            hsl$1[2] *= 0.01;
            var rgb$5 = hsl2rgb(hsl$1);
            rgb$5[3] = +m[4];  // default alpha = 1
            return rgb$5;
        }
    };

    css2rgb$1.test = function (s) {
        return RE_RGB.test(s) ||
            RE_RGBA.test(s) ||
            RE_RGB_PCT.test(s) ||
            RE_RGBA_PCT.test(s) ||
            RE_HSL.test(s) ||
            RE_HSLA.test(s);
    };

    var css2rgb_1 = css2rgb$1;

    var chroma$i = chroma_1;
    var Color$B = Color_1;
    var input$e = input$h;
    var type$k = utils.type;

    var rgb2css = rgb2css_1;
    var css2rgb = css2rgb_1;

    Color$B.prototype.css = function(mode) {
        return rgb2css(this._rgb, mode);
    };

    chroma$i.css = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        return new (Function.prototype.bind.apply( Color$B, [ null ].concat( args, ['css']) ));
    };

    input$e.format.css = css2rgb;

    input$e.autodetect.push({
        p: 5,
        test: function (h) {
            var rest = [], len = arguments.length - 1;
            while ( len-- > 0 ) rest[ len ] = arguments[ len + 1 ];

            if (!rest.length && type$k(h) === 'string' && css2rgb.test(h)) {
                return 'css';
            }
        }
    });

    var Color$A = Color_1;
    var chroma$h = chroma_1;
    var input$d = input$h;
    var unpack$t = utils.unpack;

    input$d.format.gl = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        var rgb = unpack$t(args, 'rgba');
        rgb[0] *= 255;
        rgb[1] *= 255;
        rgb[2] *= 255;
        return rgb;
    };

    chroma$h.gl = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        return new (Function.prototype.bind.apply( Color$A, [ null ].concat( args, ['gl']) ));
    };

    Color$A.prototype.gl = function() {
        var rgb = this._rgb;
        return [rgb[0]/255, rgb[1]/255, rgb[2]/255, rgb[3]];
    };

    var unpack$s = utils.unpack;

    var rgb2hcg$1 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        var ref = unpack$s(args, 'rgb');
        var r = ref[0];
        var g = ref[1];
        var b = ref[2];
        var min = Math.min(r, g, b);
        var max = Math.max(r, g, b);
        var delta = max - min;
        var c = delta * 100 / 255;
        var _g = min / (255 - delta) * 100;
        var h;
        if (delta === 0) {
            h = Number.NaN;
        } else {
            if (r === max) { h = (g - b) / delta; }
            if (g === max) { h = 2+(b - r) / delta; }
            if (b === max) { h = 4+(r - g) / delta; }
            h *= 60;
            if (h < 0) { h += 360; }
        }
        return [h, c, _g];
    };

    var rgb2hcg_1 = rgb2hcg$1;

    var unpack$r = utils.unpack;
    var floor$3 = Math.floor;

    /*
     * this is basically just HSV with some minor tweaks
     *
     * hue.. [0..360]
     * chroma .. [0..1]
     * grayness .. [0..1]
     */

    var hcg2rgb = function () {
        var assign, assign$1, assign$2, assign$3, assign$4, assign$5;

        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];
        args = unpack$r(args, 'hcg');
        var h = args[0];
        var c = args[1];
        var _g = args[2];
        var r,g,b;
        _g = _g * 255;
        var _c = c * 255;
        if (c === 0) {
            r = g = b = _g;
        } else {
            if (h === 360) { h = 0; }
            if (h > 360) { h -= 360; }
            if (h < 0) { h += 360; }
            h /= 60;
            var i = floor$3(h);
            var f = h - i;
            var p = _g * (1 - c);
            var q = p + _c * (1 - f);
            var t = p + _c * f;
            var v = p + _c;
            switch (i) {
                case 0: (assign = [v, t, p], r = assign[0], g = assign[1], b = assign[2]); break
                case 1: (assign$1 = [q, v, p], r = assign$1[0], g = assign$1[1], b = assign$1[2]); break
                case 2: (assign$2 = [p, v, t], r = assign$2[0], g = assign$2[1], b = assign$2[2]); break
                case 3: (assign$3 = [p, q, v], r = assign$3[0], g = assign$3[1], b = assign$3[2]); break
                case 4: (assign$4 = [t, p, v], r = assign$4[0], g = assign$4[1], b = assign$4[2]); break
                case 5: (assign$5 = [v, p, q], r = assign$5[0], g = assign$5[1], b = assign$5[2]); break
            }
        }
        return [r, g, b, args.length > 3 ? args[3] : 1];
    };

    var hcg2rgb_1 = hcg2rgb;

    var unpack$q = utils.unpack;
    var type$j = utils.type;
    var chroma$g = chroma_1;
    var Color$z = Color_1;
    var input$c = input$h;

    var rgb2hcg = rgb2hcg_1;

    Color$z.prototype.hcg = function() {
        return rgb2hcg(this._rgb);
    };

    chroma$g.hcg = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        return new (Function.prototype.bind.apply( Color$z, [ null ].concat( args, ['hcg']) ));
    };

    input$c.format.hcg = hcg2rgb_1;

    input$c.autodetect.push({
        p: 1,
        test: function () {
            var args = [], len = arguments.length;
            while ( len-- ) args[ len ] = arguments[ len ];

            args = unpack$q(args, 'hcg');
            if (type$j(args) === 'array' && args.length === 3) {
                return 'hcg';
            }
        }
    });

    var unpack$p = utils.unpack;
    var last = utils.last;
    var round$3 = Math.round;

    var rgb2hex$2 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        var ref = unpack$p(args, 'rgba');
        var r = ref[0];
        var g = ref[1];
        var b = ref[2];
        var a = ref[3];
        var mode = last(args) || 'auto';
        if (a === undefined) { a = 1; }
        if (mode === 'auto') {
            mode = a < 1 ? 'rgba' : 'rgb';
        }
        r = round$3(r);
        g = round$3(g);
        b = round$3(b);
        var u = r << 16 | g << 8 | b;
        var str = "000000" + u.toString(16); //#.toUpperCase();
        str = str.substr(str.length - 6);
        var hxa = '0' + round$3(a * 255).toString(16);
        hxa = hxa.substr(hxa.length - 2);
        switch (mode.toLowerCase()) {
            case 'rgba': return ("#" + str + hxa);
            case 'argb': return ("#" + hxa + str);
            default: return ("#" + str);
        }
    };

    var rgb2hex_1 = rgb2hex$2;

    var RE_HEX = /^#?([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$/;
    var RE_HEXA = /^#?([A-Fa-f0-9]{8}|[A-Fa-f0-9]{4})$/;

    var hex2rgb$1 = function (hex) {
        if (hex.match(RE_HEX)) {
            // remove optional leading #
            if (hex.length === 4 || hex.length === 7) {
                hex = hex.substr(1);
            }
            // expand short-notation to full six-digit
            if (hex.length === 3) {
                hex = hex.split('');
                hex = hex[0]+hex[0]+hex[1]+hex[1]+hex[2]+hex[2];
            }
            var u = parseInt(hex, 16);
            var r = u >> 16;
            var g = u >> 8 & 0xFF;
            var b = u & 0xFF;
            return [r,g,b,1];
        }

        // match rgba hex format, eg #FF000077
        if (hex.match(RE_HEXA)) {
            if (hex.length === 5 || hex.length === 9) {
                // remove optional leading #
                hex = hex.substr(1);
            }
            // expand short-notation to full eight-digit
            if (hex.length === 4) {
                hex = hex.split('');
                hex = hex[0]+hex[0]+hex[1]+hex[1]+hex[2]+hex[2]+hex[3]+hex[3];
            }
            var u$1 = parseInt(hex, 16);
            var r$1 = u$1 >> 24 & 0xFF;
            var g$1 = u$1 >> 16 & 0xFF;
            var b$1 = u$1 >> 8 & 0xFF;
            var a = Math.round((u$1 & 0xFF) / 0xFF * 100) / 100;
            return [r$1,g$1,b$1,a];
        }

        // we used to check for css colors here
        // if _input.css? and rgb = _input.css hex
        //     return rgb

        throw new Error(("unknown hex color: " + hex));
    };

    var hex2rgb_1 = hex2rgb$1;

    var chroma$f = chroma_1;
    var Color$y = Color_1;
    var type$i = utils.type;
    var input$b = input$h;

    var rgb2hex$1 = rgb2hex_1;

    Color$y.prototype.hex = function(mode) {
        return rgb2hex$1(this._rgb, mode);
    };

    chroma$f.hex = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        return new (Function.prototype.bind.apply( Color$y, [ null ].concat( args, ['hex']) ));
    };

    input$b.format.hex = hex2rgb_1;
    input$b.autodetect.push({
        p: 4,
        test: function (h) {
            var rest = [], len = arguments.length - 1;
            while ( len-- > 0 ) rest[ len ] = arguments[ len + 1 ];

            if (!rest.length && type$i(h) === 'string' && [3,4,5,6,7,8,9].indexOf(h.length) >= 0) {
                return 'hex';
            }
        }
    });

    var unpack$o = utils.unpack;
    var TWOPI$2 = utils.TWOPI;
    var min$2 = Math.min;
    var sqrt$4 = Math.sqrt;
    var acos = Math.acos;

    var rgb2hsi$1 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        /*
        borrowed from here:
        http://hummer.stanford.edu/museinfo/doc/examples/humdrum/keyscape2/rgb2hsi.cpp
        */
        var ref = unpack$o(args, 'rgb');
        var r = ref[0];
        var g = ref[1];
        var b = ref[2];
        r /= 255;
        g /= 255;
        b /= 255;
        var h;
        var min_ = min$2(r,g,b);
        var i = (r+g+b) / 3;
        var s = i > 0 ? 1 - min_/i : 0;
        if (s === 0) {
            h = NaN;
        } else {
            h = ((r-g)+(r-b)) / 2;
            h /= sqrt$4((r-g)*(r-g) + (r-b)*(g-b));
            h = acos(h);
            if (b > g) {
                h = TWOPI$2 - h;
            }
            h /= TWOPI$2;
        }
        return [h*360,s,i];
    };

    var rgb2hsi_1 = rgb2hsi$1;

    var unpack$n = utils.unpack;
    var limit = utils.limit;
    var TWOPI$1 = utils.TWOPI;
    var PITHIRD = utils.PITHIRD;
    var cos$4 = Math.cos;

    /*
     * hue [0..360]
     * saturation [0..1]
     * intensity [0..1]
     */
    var hsi2rgb = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        /*
        borrowed from here:
        http://hummer.stanford.edu/museinfo/doc/examples/humdrum/keyscape2/hsi2rgb.cpp
        */
        args = unpack$n(args, 'hsi');
        var h = args[0];
        var s = args[1];
        var i = args[2];
        var r,g,b;

        if (isNaN(h)) { h = 0; }
        if (isNaN(s)) { s = 0; }
        // normalize hue
        if (h > 360) { h -= 360; }
        if (h < 0) { h += 360; }
        h /= 360;
        if (h < 1/3) {
            b = (1-s)/3;
            r = (1+s*cos$4(TWOPI$1*h)/cos$4(PITHIRD-TWOPI$1*h))/3;
            g = 1 - (b+r);
        } else if (h < 2/3) {
            h -= 1/3;
            r = (1-s)/3;
            g = (1+s*cos$4(TWOPI$1*h)/cos$4(PITHIRD-TWOPI$1*h))/3;
            b = 1 - (r+g);
        } else {
            h -= 2/3;
            g = (1-s)/3;
            b = (1+s*cos$4(TWOPI$1*h)/cos$4(PITHIRD-TWOPI$1*h))/3;
            r = 1 - (g+b);
        }
        r = limit(i*r*3);
        g = limit(i*g*3);
        b = limit(i*b*3);
        return [r*255, g*255, b*255, args.length > 3 ? args[3] : 1];
    };

    var hsi2rgb_1 = hsi2rgb;

    var unpack$m = utils.unpack;
    var type$h = utils.type;
    var chroma$e = chroma_1;
    var Color$x = Color_1;
    var input$a = input$h;

    var rgb2hsi = rgb2hsi_1;

    Color$x.prototype.hsi = function() {
        return rgb2hsi(this._rgb);
    };

    chroma$e.hsi = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        return new (Function.prototype.bind.apply( Color$x, [ null ].concat( args, ['hsi']) ));
    };

    input$a.format.hsi = hsi2rgb_1;

    input$a.autodetect.push({
        p: 2,
        test: function () {
            var args = [], len = arguments.length;
            while ( len-- ) args[ len ] = arguments[ len ];

            args = unpack$m(args, 'hsi');
            if (type$h(args) === 'array' && args.length === 3) {
                return 'hsi';
            }
        }
    });

    var unpack$l = utils.unpack;
    var type$g = utils.type;
    var chroma$d = chroma_1;
    var Color$w = Color_1;
    var input$9 = input$h;

    var rgb2hsl$1 = rgb2hsl_1;

    Color$w.prototype.hsl = function() {
        return rgb2hsl$1(this._rgb);
    };

    chroma$d.hsl = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        return new (Function.prototype.bind.apply( Color$w, [ null ].concat( args, ['hsl']) ));
    };

    input$9.format.hsl = hsl2rgb_1;

    input$9.autodetect.push({
        p: 2,
        test: function () {
            var args = [], len = arguments.length;
            while ( len-- ) args[ len ] = arguments[ len ];

            args = unpack$l(args, 'hsl');
            if (type$g(args) === 'array' && args.length === 3) {
                return 'hsl';
            }
        }
    });

    var unpack$k = utils.unpack;
    var min$1 = Math.min;
    var max$1 = Math.max;

    /*
     * supported arguments:
     * - rgb2hsv(r,g,b)
     * - rgb2hsv([r,g,b])
     * - rgb2hsv({r,g,b})
     */
    var rgb2hsl = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        args = unpack$k(args, 'rgb');
        var r = args[0];
        var g = args[1];
        var b = args[2];
        var min_ = min$1(r, g, b);
        var max_ = max$1(r, g, b);
        var delta = max_ - min_;
        var h,s,v;
        v = max_ / 255.0;
        if (max_ === 0) {
            h = Number.NaN;
            s = 0;
        } else {
            s = delta / max_;
            if (r === max_) { h = (g - b) / delta; }
            if (g === max_) { h = 2+(b - r) / delta; }
            if (b === max_) { h = 4+(r - g) / delta; }
            h *= 60;
            if (h < 0) { h += 360; }
        }
        return [h, s, v]
    };

    var rgb2hsv$1 = rgb2hsl;

    var unpack$j = utils.unpack;
    var floor$2 = Math.floor;

    var hsv2rgb = function () {
        var assign, assign$1, assign$2, assign$3, assign$4, assign$5;

        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];
        args = unpack$j(args, 'hsv');
        var h = args[0];
        var s = args[1];
        var v = args[2];
        var r,g,b;
        v *= 255;
        if (s === 0) {
            r = g = b = v;
        } else {
            if (h === 360) { h = 0; }
            if (h > 360) { h -= 360; }
            if (h < 0) { h += 360; }
            h /= 60;

            var i = floor$2(h);
            var f = h - i;
            var p = v * (1 - s);
            var q = v * (1 - s * f);
            var t = v * (1 - s * (1 - f));

            switch (i) {
                case 0: (assign = [v, t, p], r = assign[0], g = assign[1], b = assign[2]); break
                case 1: (assign$1 = [q, v, p], r = assign$1[0], g = assign$1[1], b = assign$1[2]); break
                case 2: (assign$2 = [p, v, t], r = assign$2[0], g = assign$2[1], b = assign$2[2]); break
                case 3: (assign$3 = [p, q, v], r = assign$3[0], g = assign$3[1], b = assign$3[2]); break
                case 4: (assign$4 = [t, p, v], r = assign$4[0], g = assign$4[1], b = assign$4[2]); break
                case 5: (assign$5 = [v, p, q], r = assign$5[0], g = assign$5[1], b = assign$5[2]); break
            }
        }
        return [r,g,b,args.length > 3?args[3]:1];
    };

    var hsv2rgb_1 = hsv2rgb;

    var unpack$i = utils.unpack;
    var type$f = utils.type;
    var chroma$c = chroma_1;
    var Color$v = Color_1;
    var input$8 = input$h;

    var rgb2hsv = rgb2hsv$1;

    Color$v.prototype.hsv = function() {
        return rgb2hsv(this._rgb);
    };

    chroma$c.hsv = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        return new (Function.prototype.bind.apply( Color$v, [ null ].concat( args, ['hsv']) ));
    };

    input$8.format.hsv = hsv2rgb_1;

    input$8.autodetect.push({
        p: 2,
        test: function () {
            var args = [], len = arguments.length;
            while ( len-- ) args[ len ] = arguments[ len ];

            args = unpack$i(args, 'hsv');
            if (type$f(args) === 'array' && args.length === 3) {
                return 'hsv';
            }
        }
    });

    var labConstants = {
        // Corresponds roughly to RGB brighter/darker
        Kn: 18,

        // D65 standard referent
        Xn: 0.950470,
        Yn: 1,
        Zn: 1.088830,

        t0: 0.137931034,  // 4 / 29
        t1: 0.206896552,  // 6 / 29
        t2: 0.12841855,   // 3 * t1 * t1
        t3: 0.008856452,  // t1 * t1 * t1
    };

    var LAB_CONSTANTS$3 = labConstants;
    var unpack$h = utils.unpack;
    var pow$a = Math.pow;

    var rgb2lab$2 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        var ref = unpack$h(args, 'rgb');
        var r = ref[0];
        var g = ref[1];
        var b = ref[2];
        var ref$1 = rgb2xyz(r,g,b);
        var x = ref$1[0];
        var y = ref$1[1];
        var z = ref$1[2];
        var l = 116 * y - 16;
        return [l < 0 ? 0 : l, 500 * (x - y), 200 * (y - z)];
    };

    var rgb_xyz = function (r) {
        if ((r /= 255) <= 0.04045) { return r / 12.92; }
        return pow$a((r + 0.055) / 1.055, 2.4);
    };

    var xyz_lab = function (t) {
        if (t > LAB_CONSTANTS$3.t3) { return pow$a(t, 1 / 3); }
        return t / LAB_CONSTANTS$3.t2 + LAB_CONSTANTS$3.t0;
    };

    var rgb2xyz = function (r,g,b) {
        r = rgb_xyz(r);
        g = rgb_xyz(g);
        b = rgb_xyz(b);
        var x = xyz_lab((0.4124564 * r + 0.3575761 * g + 0.1804375 * b) / LAB_CONSTANTS$3.Xn);
        var y = xyz_lab((0.2126729 * r + 0.7151522 * g + 0.0721750 * b) / LAB_CONSTANTS$3.Yn);
        var z = xyz_lab((0.0193339 * r + 0.1191920 * g + 0.9503041 * b) / LAB_CONSTANTS$3.Zn);
        return [x,y,z];
    };

    var rgb2lab_1 = rgb2lab$2;

    var LAB_CONSTANTS$2 = labConstants;
    var unpack$g = utils.unpack;
    var pow$9 = Math.pow;

    /*
     * L* [0..100]
     * a [-100..100]
     * b [-100..100]
     */
    var lab2rgb$1 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        args = unpack$g(args, 'lab');
        var l = args[0];
        var a = args[1];
        var b = args[2];
        var x,y,z, r,g,b_;

        y = (l + 16) / 116;
        x = isNaN(a) ? y : y + a / 500;
        z = isNaN(b) ? y : y - b / 200;

        y = LAB_CONSTANTS$2.Yn * lab_xyz(y);
        x = LAB_CONSTANTS$2.Xn * lab_xyz(x);
        z = LAB_CONSTANTS$2.Zn * lab_xyz(z);

        r = xyz_rgb(3.2404542 * x - 1.5371385 * y - 0.4985314 * z);  // D65 -> sRGB
        g = xyz_rgb(-0.9692660 * x + 1.8760108 * y + 0.0415560 * z);
        b_ = xyz_rgb(0.0556434 * x - 0.2040259 * y + 1.0572252 * z);

        return [r,g,b_,args.length > 3 ? args[3] : 1];
    };

    var xyz_rgb = function (r) {
        return 255 * (r <= 0.00304 ? 12.92 * r : 1.055 * pow$9(r, 1 / 2.4) - 0.055)
    };

    var lab_xyz = function (t) {
        return t > LAB_CONSTANTS$2.t1 ? t * t * t : LAB_CONSTANTS$2.t2 * (t - LAB_CONSTANTS$2.t0)
    };

    var lab2rgb_1 = lab2rgb$1;

    var unpack$f = utils.unpack;
    var type$e = utils.type;
    var chroma$b = chroma_1;
    var Color$u = Color_1;
    var input$7 = input$h;

    var rgb2lab$1 = rgb2lab_1;

    Color$u.prototype.lab = function() {
        return rgb2lab$1(this._rgb);
    };

    chroma$b.lab = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        return new (Function.prototype.bind.apply( Color$u, [ null ].concat( args, ['lab']) ));
    };

    input$7.format.lab = lab2rgb_1;

    input$7.autodetect.push({
        p: 2,
        test: function () {
            var args = [], len = arguments.length;
            while ( len-- ) args[ len ] = arguments[ len ];

            args = unpack$f(args, 'lab');
            if (type$e(args) === 'array' && args.length === 3) {
                return 'lab';
            }
        }
    });

    var unpack$e = utils.unpack;
    var RAD2DEG = utils.RAD2DEG;
    var sqrt$3 = Math.sqrt;
    var atan2$2 = Math.atan2;
    var round$2 = Math.round;

    var lab2lch$2 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        var ref = unpack$e(args, 'lab');
        var l = ref[0];
        var a = ref[1];
        var b = ref[2];
        var c = sqrt$3(a * a + b * b);
        var h = (atan2$2(b, a) * RAD2DEG + 360) % 360;
        if (round$2(c*10000) === 0) { h = Number.NaN; }
        return [l, c, h];
    };

    var lab2lch_1 = lab2lch$2;

    var unpack$d = utils.unpack;
    var rgb2lab = rgb2lab_1;
    var lab2lch$1 = lab2lch_1;

    var rgb2lch$1 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        var ref = unpack$d(args, 'rgb');
        var r = ref[0];
        var g = ref[1];
        var b = ref[2];
        var ref$1 = rgb2lab(r,g,b);
        var l = ref$1[0];
        var a = ref$1[1];
        var b_ = ref$1[2];
        return lab2lch$1(l,a,b_);
    };

    var rgb2lch_1 = rgb2lch$1;

    var unpack$c = utils.unpack;
    var DEG2RAD = utils.DEG2RAD;
    var sin$3 = Math.sin;
    var cos$3 = Math.cos;

    var lch2lab$2 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        /*
        Convert from a qualitative parameter h and a quantitative parameter l to a 24-bit pixel.
        These formulas were invented by David Dalrymple to obtain maximum contrast without going
        out of gamut if the parameters are in the range 0-1.

        A saturation multiplier was added by Gregor Aisch
        */
        var ref = unpack$c(args, 'lch');
        var l = ref[0];
        var c = ref[1];
        var h = ref[2];
        if (isNaN(h)) { h = 0; }
        h = h * DEG2RAD;
        return [l, cos$3(h) * c, sin$3(h) * c]
    };

    var lch2lab_1 = lch2lab$2;

    var unpack$b = utils.unpack;
    var lch2lab$1 = lch2lab_1;
    var lab2rgb = lab2rgb_1;

    var lch2rgb$1 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        args = unpack$b(args, 'lch');
        var l = args[0];
        var c = args[1];
        var h = args[2];
        var ref = lch2lab$1 (l,c,h);
        var L = ref[0];
        var a = ref[1];
        var b_ = ref[2];
        var ref$1 = lab2rgb (L,a,b_);
        var r = ref$1[0];
        var g = ref$1[1];
        var b = ref$1[2];
        return [r, g, b, args.length > 3 ? args[3] : 1];
    };

    var lch2rgb_1 = lch2rgb$1;

    var unpack$a = utils.unpack;
    var lch2rgb = lch2rgb_1;

    var hcl2rgb = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        var hcl = unpack$a(args, 'hcl').reverse();
        return lch2rgb.apply(void 0, hcl);
    };

    var hcl2rgb_1 = hcl2rgb;

    var unpack$9 = utils.unpack;
    var type$d = utils.type;
    var chroma$a = chroma_1;
    var Color$t = Color_1;
    var input$6 = input$h;

    var rgb2lch = rgb2lch_1;

    Color$t.prototype.lch = function() { return rgb2lch(this._rgb); };
    Color$t.prototype.hcl = function() { return rgb2lch(this._rgb).reverse(); };

    chroma$a.lch = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        return new (Function.prototype.bind.apply( Color$t, [ null ].concat( args, ['lch']) ));
    };
    chroma$a.hcl = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        return new (Function.prototype.bind.apply( Color$t, [ null ].concat( args, ['hcl']) ));
    };

    input$6.format.lch = lch2rgb_1;
    input$6.format.hcl = hcl2rgb_1;

    ['lch','hcl'].forEach(function (m) { return input$6.autodetect.push({
        p: 2,
        test: function () {
            var args = [], len = arguments.length;
            while ( len-- ) args[ len ] = arguments[ len ];

            args = unpack$9(args, m);
            if (type$d(args) === 'array' && args.length === 3) {
                return m;
            }
        }
    }); });

    /**
    	X11 color names

    	http://www.w3.org/TR/css3-color/#svg-color
    */

    var w3cx11$1 = {
        aliceblue: '#f0f8ff',
        antiquewhite: '#faebd7',
        aqua: '#00ffff',
        aquamarine: '#7fffd4',
        azure: '#f0ffff',
        beige: '#f5f5dc',
        bisque: '#ffe4c4',
        black: '#000000',
        blanchedalmond: '#ffebcd',
        blue: '#0000ff',
        blueviolet: '#8a2be2',
        brown: '#a52a2a',
        burlywood: '#deb887',
        cadetblue: '#5f9ea0',
        chartreuse: '#7fff00',
        chocolate: '#d2691e',
        coral: '#ff7f50',
        cornflower: '#6495ed',
        cornflowerblue: '#6495ed',
        cornsilk: '#fff8dc',
        crimson: '#dc143c',
        cyan: '#00ffff',
        darkblue: '#00008b',
        darkcyan: '#008b8b',
        darkgoldenrod: '#b8860b',
        darkgray: '#a9a9a9',
        darkgreen: '#006400',
        darkgrey: '#a9a9a9',
        darkkhaki: '#bdb76b',
        darkmagenta: '#8b008b',
        darkolivegreen: '#556b2f',
        darkorange: '#ff8c00',
        darkorchid: '#9932cc',
        darkred: '#8b0000',
        darksalmon: '#e9967a',
        darkseagreen: '#8fbc8f',
        darkslateblue: '#483d8b',
        darkslategray: '#2f4f4f',
        darkslategrey: '#2f4f4f',
        darkturquoise: '#00ced1',
        darkviolet: '#9400d3',
        deeppink: '#ff1493',
        deepskyblue: '#00bfff',
        dimgray: '#696969',
        dimgrey: '#696969',
        dodgerblue: '#1e90ff',
        firebrick: '#b22222',
        floralwhite: '#fffaf0',
        forestgreen: '#228b22',
        fuchsia: '#ff00ff',
        gainsboro: '#dcdcdc',
        ghostwhite: '#f8f8ff',
        gold: '#ffd700',
        goldenrod: '#daa520',
        gray: '#808080',
        green: '#008000',
        greenyellow: '#adff2f',
        grey: '#808080',
        honeydew: '#f0fff0',
        hotpink: '#ff69b4',
        indianred: '#cd5c5c',
        indigo: '#4b0082',
        ivory: '#fffff0',
        khaki: '#f0e68c',
        laserlemon: '#ffff54',
        lavender: '#e6e6fa',
        lavenderblush: '#fff0f5',
        lawngreen: '#7cfc00',
        lemonchiffon: '#fffacd',
        lightblue: '#add8e6',
        lightcoral: '#f08080',
        lightcyan: '#e0ffff',
        lightgoldenrod: '#fafad2',
        lightgoldenrodyellow: '#fafad2',
        lightgray: '#d3d3d3',
        lightgreen: '#90ee90',
        lightgrey: '#d3d3d3',
        lightpink: '#ffb6c1',
        lightsalmon: '#ffa07a',
        lightseagreen: '#20b2aa',
        lightskyblue: '#87cefa',
        lightslategray: '#778899',
        lightslategrey: '#778899',
        lightsteelblue: '#b0c4de',
        lightyellow: '#ffffe0',
        lime: '#00ff00',
        limegreen: '#32cd32',
        linen: '#faf0e6',
        magenta: '#ff00ff',
        maroon: '#800000',
        maroon2: '#7f0000',
        maroon3: '#b03060',
        mediumaquamarine: '#66cdaa',
        mediumblue: '#0000cd',
        mediumorchid: '#ba55d3',
        mediumpurple: '#9370db',
        mediumseagreen: '#3cb371',
        mediumslateblue: '#7b68ee',
        mediumspringgreen: '#00fa9a',
        mediumturquoise: '#48d1cc',
        mediumvioletred: '#c71585',
        midnightblue: '#191970',
        mintcream: '#f5fffa',
        mistyrose: '#ffe4e1',
        moccasin: '#ffe4b5',
        navajowhite: '#ffdead',
        navy: '#000080',
        oldlace: '#fdf5e6',
        olive: '#808000',
        olivedrab: '#6b8e23',
        orange: '#ffa500',
        orangered: '#ff4500',
        orchid: '#da70d6',
        palegoldenrod: '#eee8aa',
        palegreen: '#98fb98',
        paleturquoise: '#afeeee',
        palevioletred: '#db7093',
        papayawhip: '#ffefd5',
        peachpuff: '#ffdab9',
        peru: '#cd853f',
        pink: '#ffc0cb',
        plum: '#dda0dd',
        powderblue: '#b0e0e6',
        purple: '#800080',
        purple2: '#7f007f',
        purple3: '#a020f0',
        rebeccapurple: '#663399',
        red: '#ff0000',
        rosybrown: '#bc8f8f',
        royalblue: '#4169e1',
        saddlebrown: '#8b4513',
        salmon: '#fa8072',
        sandybrown: '#f4a460',
        seagreen: '#2e8b57',
        seashell: '#fff5ee',
        sienna: '#a0522d',
        silver: '#c0c0c0',
        skyblue: '#87ceeb',
        slateblue: '#6a5acd',
        slategray: '#708090',
        slategrey: '#708090',
        snow: '#fffafa',
        springgreen: '#00ff7f',
        steelblue: '#4682b4',
        tan: '#d2b48c',
        teal: '#008080',
        thistle: '#d8bfd8',
        tomato: '#ff6347',
        turquoise: '#40e0d0',
        violet: '#ee82ee',
        wheat: '#f5deb3',
        white: '#ffffff',
        whitesmoke: '#f5f5f5',
        yellow: '#ffff00',
        yellowgreen: '#9acd32'
    };

    var w3cx11_1 = w3cx11$1;

    var Color$s = Color_1;
    var input$5 = input$h;
    var type$c = utils.type;

    var w3cx11 = w3cx11_1;
    var hex2rgb = hex2rgb_1;
    var rgb2hex = rgb2hex_1;

    Color$s.prototype.name = function() {
        var hex = rgb2hex(this._rgb, 'rgb');
        for (var i = 0, list = Object.keys(w3cx11); i < list.length; i += 1) {
            var n = list[i];

            if (w3cx11[n] === hex) { return n.toLowerCase(); }
        }
        return hex;
    };

    input$5.format.named = function (name) {
        name = name.toLowerCase();
        if (w3cx11[name]) { return hex2rgb(w3cx11[name]); }
        throw new Error('unknown color name: '+name);
    };

    input$5.autodetect.push({
        p: 5,
        test: function (h) {
            var rest = [], len = arguments.length - 1;
            while ( len-- > 0 ) rest[ len ] = arguments[ len + 1 ];

            if (!rest.length && type$c(h) === 'string' && w3cx11[h.toLowerCase()]) {
                return 'named';
            }
        }
    });

    var unpack$8 = utils.unpack;

    var rgb2num$1 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        var ref = unpack$8(args, 'rgb');
        var r = ref[0];
        var g = ref[1];
        var b = ref[2];
        return (r << 16) + (g << 8) + b;
    };

    var rgb2num_1 = rgb2num$1;

    var type$b = utils.type;

    var num2rgb = function (num) {
        if (type$b(num) == "number" && num >= 0 && num <= 0xFFFFFF) {
            var r = num >> 16;
            var g = (num >> 8) & 0xFF;
            var b = num & 0xFF;
            return [r,g,b,1];
        }
        throw new Error("unknown num color: "+num);
    };

    var num2rgb_1 = num2rgb;

    var chroma$9 = chroma_1;
    var Color$r = Color_1;
    var input$4 = input$h;
    var type$a = utils.type;

    var rgb2num = rgb2num_1;

    Color$r.prototype.num = function() {
        return rgb2num(this._rgb);
    };

    chroma$9.num = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        return new (Function.prototype.bind.apply( Color$r, [ null ].concat( args, ['num']) ));
    };

    input$4.format.num = num2rgb_1;

    input$4.autodetect.push({
        p: 5,
        test: function () {
            var args = [], len = arguments.length;
            while ( len-- ) args[ len ] = arguments[ len ];

            if (args.length === 1 && type$a(args[0]) === 'number' && args[0] >= 0 && args[0] <= 0xFFFFFF) {
                return 'num';
            }
        }
    });

    var chroma$8 = chroma_1;
    var Color$q = Color_1;
    var input$3 = input$h;
    var unpack$7 = utils.unpack;
    var type$9 = utils.type;
    var round$1 = Math.round;

    Color$q.prototype.rgb = function(rnd) {
        if ( rnd === void 0 ) rnd=true;

        if (rnd === false) { return this._rgb.slice(0,3); }
        return this._rgb.slice(0,3).map(round$1);
    };

    Color$q.prototype.rgba = function(rnd) {
        if ( rnd === void 0 ) rnd=true;

        return this._rgb.slice(0,4).map(function (v,i) {
            return i<3 ? (rnd === false ? v : round$1(v)) : v;
        });
    };

    chroma$8.rgb = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        return new (Function.prototype.bind.apply( Color$q, [ null ].concat( args, ['rgb']) ));
    };

    input$3.format.rgb = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        var rgba = unpack$7(args, 'rgba');
        if (rgba[3] === undefined) { rgba[3] = 1; }
        return rgba;
    };

    input$3.autodetect.push({
        p: 3,
        test: function () {
            var args = [], len = arguments.length;
            while ( len-- ) args[ len ] = arguments[ len ];

            args = unpack$7(args, 'rgba');
            if (type$9(args) === 'array' && (args.length === 3 ||
                args.length === 4 && type$9(args[3]) == 'number' && args[3] >= 0 && args[3] <= 1)) {
                return 'rgb';
            }
        }
    });

    /*
     * Based on implementation by Neil Bartlett
     * https://github.com/neilbartlett/color-temperature
     */

    var log$1 = Math.log;

    var temperature2rgb$1 = function (kelvin) {
        var temp = kelvin / 100;
        var r,g,b;
        if (temp < 66) {
            r = 255;
            g = temp < 6 ? 0 : -155.25485562709179 - 0.44596950469579133 * (g = temp-2) + 104.49216199393888 * log$1(g);
            b = temp < 20 ? 0 : -254.76935184120902 + 0.8274096064007395 * (b = temp-10) + 115.67994401066147 * log$1(b);
        } else {
            r = 351.97690566805693 + 0.114206453784165 * (r = temp-55) - 40.25366309332127 * log$1(r);
            g = 325.4494125711974 + 0.07943456536662342 * (g = temp-50) - 28.0852963507957 * log$1(g);
            b = 255;
        }
        return [r,g,b,1];
    };

    var temperature2rgb_1 = temperature2rgb$1;

    /*
     * Based on implementation by Neil Bartlett
     * https://github.com/neilbartlett/color-temperature
     **/

    var temperature2rgb = temperature2rgb_1;
    var unpack$6 = utils.unpack;
    var round = Math.round;

    var rgb2temperature$1 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        var rgb = unpack$6(args, 'rgb');
        var r = rgb[0], b = rgb[2];
        var minTemp = 1000;
        var maxTemp = 40000;
        var eps = 0.4;
        var temp;
        while (maxTemp - minTemp > eps) {
            temp = (maxTemp + minTemp) * 0.5;
            var rgb$1 = temperature2rgb(temp);
            if ((rgb$1[2] / rgb$1[0]) >= (b / r)) {
                maxTemp = temp;
            } else {
                minTemp = temp;
            }
        }
        return round(temp);
    };

    var rgb2temperature_1 = rgb2temperature$1;

    var chroma$7 = chroma_1;
    var Color$p = Color_1;
    var input$2 = input$h;

    var rgb2temperature = rgb2temperature_1;

    Color$p.prototype.temp =
    Color$p.prototype.kelvin =
    Color$p.prototype.temperature = function() {
        return rgb2temperature(this._rgb);
    };

    chroma$7.temp =
    chroma$7.kelvin =
    chroma$7.temperature = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        return new (Function.prototype.bind.apply( Color$p, [ null ].concat( args, ['temp']) ));
    };

    input$2.format.temp =
    input$2.format.kelvin =
    input$2.format.temperature = temperature2rgb_1;

    var unpack$5 = utils.unpack;
    var cbrt = Math.cbrt;
    var pow$8 = Math.pow;
    var sign$1 = Math.sign;

    var rgb2oklab$2 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        // OKLab color space implementation taken from
        // https://bottosson.github.io/posts/oklab/
        var ref = unpack$5(args, 'rgb');
        var r = ref[0];
        var g = ref[1];
        var b = ref[2];
        var ref$1 = [rgb2lrgb(r / 255), rgb2lrgb(g / 255), rgb2lrgb(b / 255)];
        var lr = ref$1[0];
        var lg = ref$1[1];
        var lb = ref$1[2];
        var l = cbrt(0.4122214708 * lr + 0.5363325363 * lg + 0.0514459929 * lb);
        var m = cbrt(0.2119034982 * lr + 0.6806995451 * lg + 0.1073969566 * lb);
        var s = cbrt(0.0883024619 * lr + 0.2817188376 * lg + 0.6299787005 * lb);

        return [
            0.2104542553 * l + 0.793617785 * m - 0.0040720468 * s,
            1.9779984951 * l - 2.428592205 * m + 0.4505937099 * s,
            0.0259040371 * l + 0.7827717662 * m - 0.808675766 * s
        ];
    };

    var rgb2oklab_1 = rgb2oklab$2;

    function rgb2lrgb(c) {
        var abs = Math.abs(c);
        if (abs < 0.04045) {
            return c / 12.92;
        }
        return (sign$1(c) || 1) * pow$8((abs + 0.055) / 1.055, 2.4);
    }

    var unpack$4 = utils.unpack;
    var pow$7 = Math.pow;
    var sign = Math.sign;

    /*
     * L* [0..100]
     * a [-100..100]
     * b [-100..100]
     */
    var oklab2rgb$1 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        args = unpack$4(args, 'lab');
        var L = args[0];
        var a = args[1];
        var b = args[2];

        var l = pow$7(L + 0.3963377774 * a + 0.2158037573 * b, 3);
        var m = pow$7(L - 0.1055613458 * a - 0.0638541728 * b, 3);
        var s = pow$7(L - 0.0894841775 * a - 1.291485548 * b, 3);

        return [
            255 * lrgb2rgb(+4.0767416621 * l - 3.3077115913 * m + 0.2309699292 * s),
            255 * lrgb2rgb(-1.2684380046 * l + 2.6097574011 * m - 0.3413193965 * s),
            255 * lrgb2rgb(-0.0041960863 * l - 0.7034186147 * m + 1.707614701 * s),
            args.length > 3 ? args[3] : 1
        ];
    };

    var oklab2rgb_1 = oklab2rgb$1;

    function lrgb2rgb(c) {
        var abs = Math.abs(c);
        if (abs > 0.0031308) {
            return (sign(c) || 1) * (1.055 * pow$7(abs, 1 / 2.4) - 0.055);
        }
        return c * 12.92;
    }

    var unpack$3 = utils.unpack;
    var type$8 = utils.type;
    var chroma$6 = chroma_1;
    var Color$o = Color_1;
    var input$1 = input$h;

    var rgb2oklab$1 = rgb2oklab_1;

    Color$o.prototype.oklab = function () {
        return rgb2oklab$1(this._rgb);
    };

    chroma$6.oklab = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        return new (Function.prototype.bind.apply( Color$o, [ null ].concat( args, ['oklab']) ));
    };

    input$1.format.oklab = oklab2rgb_1;

    input$1.autodetect.push({
        p: 3,
        test: function () {
            var args = [], len = arguments.length;
            while ( len-- ) args[ len ] = arguments[ len ];

            args = unpack$3(args, 'oklab');
            if (type$8(args) === 'array' && args.length === 3) {
                return 'oklab';
            }
        }
    });

    var unpack$2 = utils.unpack;
    var rgb2oklab = rgb2oklab_1;
    var lab2lch = lab2lch_1;

    var rgb2oklch$1 = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        var ref = unpack$2(args, 'rgb');
        var r = ref[0];
        var g = ref[1];
        var b = ref[2];
        var ref$1 = rgb2oklab(r, g, b);
        var l = ref$1[0];
        var a = ref$1[1];
        var b_ = ref$1[2];
        return lab2lch(l, a, b_);
    };

    var rgb2oklch_1 = rgb2oklch$1;

    var unpack$1 = utils.unpack;
    var lch2lab = lch2lab_1;
    var oklab2rgb = oklab2rgb_1;

    var oklch2rgb = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        args = unpack$1(args, 'lch');
        var l = args[0];
        var c = args[1];
        var h = args[2];
        var ref = lch2lab(l, c, h);
        var L = ref[0];
        var a = ref[1];
        var b_ = ref[2];
        var ref$1 = oklab2rgb(L, a, b_);
        var r = ref$1[0];
        var g = ref$1[1];
        var b = ref$1[2];
        return [r, g, b, args.length > 3 ? args[3] : 1];
    };

    var oklch2rgb_1 = oklch2rgb;

    var unpack = utils.unpack;
    var type$7 = utils.type;
    var chroma$5 = chroma_1;
    var Color$n = Color_1;
    var input = input$h;

    var rgb2oklch = rgb2oklch_1;

    Color$n.prototype.oklch = function () {
        return rgb2oklch(this._rgb);
    };

    chroma$5.oklch = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        return new (Function.prototype.bind.apply( Color$n, [ null ].concat( args, ['oklch']) ));
    };

    input.format.oklch = oklch2rgb_1;

    input.autodetect.push({
        p: 3,
        test: function () {
            var args = [], len = arguments.length;
            while ( len-- ) args[ len ] = arguments[ len ];

            args = unpack(args, 'oklch');
            if (type$7(args) === 'array' && args.length === 3) {
                return 'oklch';
            }
        }
    });

    var Color$m = Color_1;
    var type$6 = utils.type;

    Color$m.prototype.alpha = function(a, mutate) {
        if ( mutate === void 0 ) mutate=false;

        if (a !== undefined && type$6(a) === 'number') {
            if (mutate) {
                this._rgb[3] = a;
                return this;
            }
            return new Color$m([this._rgb[0], this._rgb[1], this._rgb[2], a], 'rgb');
        }
        return this._rgb[3];
    };

    var Color$l = Color_1;

    Color$l.prototype.clipped = function() {
        return this._rgb._clipped || false;
    };

    var Color$k = Color_1;
    var LAB_CONSTANTS$1 = labConstants;

    Color$k.prototype.darken = function(amount) {
    	if ( amount === void 0 ) amount=1;

    	var me = this;
    	var lab = me.lab();
    	lab[0] -= LAB_CONSTANTS$1.Kn * amount;
    	return new Color$k(lab, 'lab').alpha(me.alpha(), true);
    };

    Color$k.prototype.brighten = function(amount) {
    	if ( amount === void 0 ) amount=1;

    	return this.darken(-amount);
    };

    Color$k.prototype.darker = Color$k.prototype.darken;
    Color$k.prototype.brighter = Color$k.prototype.brighten;

    var Color$j = Color_1;

    Color$j.prototype.get = function (mc) {
        var ref = mc.split('.');
        var mode = ref[0];
        var channel = ref[1];
        var src = this[mode]();
        if (channel) {
            var i = mode.indexOf(channel) - (mode.substr(0, 2) === 'ok' ? 2 : 0);
            if (i > -1) { return src[i]; }
            throw new Error(("unknown channel " + channel + " in mode " + mode));
        } else {
            return src;
        }
    };

    var Color$i = Color_1;
    var type$5 = utils.type;
    var pow$6 = Math.pow;

    var EPS = 1e-7;
    var MAX_ITER = 20;

    Color$i.prototype.luminance = function(lum) {
        if (lum !== undefined && type$5(lum) === 'number') {
            if (lum === 0) {
                // return pure black
                return new Color$i([0,0,0,this._rgb[3]], 'rgb');
            }
            if (lum === 1) {
                // return pure white
                return new Color$i([255,255,255,this._rgb[3]], 'rgb');
            }
            // compute new color using...
            var cur_lum = this.luminance();
            var mode = 'rgb';
            var max_iter = MAX_ITER;

            var test = function (low, high) {
                var mid = low.interpolate(high, 0.5, mode);
                var lm = mid.luminance();
                if (Math.abs(lum - lm) < EPS || !max_iter--) {
                    // close enough
                    return mid;
                }
                return lm > lum ? test(low, mid) : test(mid, high);
            };

            var rgb = (cur_lum > lum ? test(new Color$i([0,0,0]), this) : test(this, new Color$i([255,255,255]))).rgb();
            return new Color$i(rgb.concat( [this._rgb[3]]));
        }
        return rgb2luminance.apply(void 0, (this._rgb).slice(0,3));
    };


    var rgb2luminance = function (r,g,b) {
        // relative luminance
        // see http://www.w3.org/TR/2008/REC-WCAG20-20081211/#relativeluminancedef
        r = luminance_x(r);
        g = luminance_x(g);
        b = luminance_x(b);
        return 0.2126 * r + 0.7152 * g + 0.0722 * b;
    };

    var luminance_x = function (x) {
        x /= 255;
        return x <= 0.03928 ? x/12.92 : pow$6((x+0.055)/1.055, 2.4);
    };

    var interpolator$1 = {};

    var Color$h = Color_1;
    var type$4 = utils.type;
    var interpolator = interpolator$1;

    var mix$1 = function (col1, col2, f) {
        if ( f === void 0 ) f=0.5;
        var rest = [], len = arguments.length - 3;
        while ( len-- > 0 ) rest[ len ] = arguments[ len + 3 ];

        var mode = rest[0] || 'lrgb';
        if (!interpolator[mode] && !rest.length) {
            // fall back to the first supported mode
            mode = Object.keys(interpolator)[0];
        }
        if (!interpolator[mode]) {
            throw new Error(("interpolation mode " + mode + " is not defined"));
        }
        if (type$4(col1) !== 'object') { col1 = new Color$h(col1); }
        if (type$4(col2) !== 'object') { col2 = new Color$h(col2); }
        return interpolator[mode](col1, col2, f)
            .alpha(col1.alpha() + f * (col2.alpha() - col1.alpha()));
    };

    var Color$g = Color_1;
    var mix = mix$1;

    Color$g.prototype.mix =
    Color$g.prototype.interpolate = function(col2, f) {
    	if ( f === void 0 ) f=0.5;
    	var rest = [], len = arguments.length - 2;
    	while ( len-- > 0 ) rest[ len ] = arguments[ len + 2 ];

    	return mix.apply(void 0, [ this, col2, f ].concat( rest ));
    };

    var Color$f = Color_1;

    Color$f.prototype.premultiply = function(mutate) {
    	if ( mutate === void 0 ) mutate=false;

    	var rgb = this._rgb;
    	var a = rgb[3];
    	if (mutate) {
    		this._rgb = [rgb[0]*a, rgb[1]*a, rgb[2]*a, a];
    		return this;
    	} else {
    		return new Color$f([rgb[0]*a, rgb[1]*a, rgb[2]*a, a], 'rgb');
    	}
    };

    var Color$e = Color_1;
    var LAB_CONSTANTS = labConstants;

    Color$e.prototype.saturate = function(amount) {
    	if ( amount === void 0 ) amount=1;

    	var me = this;
    	var lch = me.lch();
    	lch[1] += LAB_CONSTANTS.Kn * amount;
    	if (lch[1] < 0) { lch[1] = 0; }
    	return new Color$e(lch, 'lch').alpha(me.alpha(), true);
    };

    Color$e.prototype.desaturate = function(amount) {
    	if ( amount === void 0 ) amount=1;

    	return this.saturate(-amount);
    };

    var Color$d = Color_1;
    var type$3 = utils.type;

    Color$d.prototype.set = function (mc, value, mutate) {
        if ( mutate === void 0 ) mutate = false;

        var ref = mc.split('.');
        var mode = ref[0];
        var channel = ref[1];
        var src = this[mode]();
        if (channel) {
            var i = mode.indexOf(channel) - (mode.substr(0, 2) === 'ok' ? 2 : 0);
            if (i > -1) {
                if (type$3(value) == 'string') {
                    switch (value.charAt(0)) {
                        case '+':
                            src[i] += +value;
                            break;
                        case '-':
                            src[i] += +value;
                            break;
                        case '*':
                            src[i] *= +value.substr(1);
                            break;
                        case '/':
                            src[i] /= +value.substr(1);
                            break;
                        default:
                            src[i] = +value;
                    }
                } else if (type$3(value) === 'number') {
                    src[i] = value;
                } else {
                    throw new Error("unsupported value for Color.set");
                }
                var out = new Color$d(src, mode);
                if (mutate) {
                    this._rgb = out._rgb;
                    return this;
                }
                return out;
            }
            throw new Error(("unknown channel " + channel + " in mode " + mode));
        } else {
            return src;
        }
    };

    var Color$c = Color_1;

    var rgb = function (col1, col2, f) {
        var xyz0 = col1._rgb;
        var xyz1 = col2._rgb;
        return new Color$c(
            xyz0[0] + f * (xyz1[0]-xyz0[0]),
            xyz0[1] + f * (xyz1[1]-xyz0[1]),
            xyz0[2] + f * (xyz1[2]-xyz0[2]),
            'rgb'
        )
    };

    // register interpolator
    interpolator$1.rgb = rgb;

    var Color$b = Color_1;
    var sqrt$2 = Math.sqrt;
    var pow$5 = Math.pow;

    var lrgb = function (col1, col2, f) {
        var ref = col1._rgb;
        var x1 = ref[0];
        var y1 = ref[1];
        var z1 = ref[2];
        var ref$1 = col2._rgb;
        var x2 = ref$1[0];
        var y2 = ref$1[1];
        var z2 = ref$1[2];
        return new Color$b(
            sqrt$2(pow$5(x1,2) * (1-f) + pow$5(x2,2) * f),
            sqrt$2(pow$5(y1,2) * (1-f) + pow$5(y2,2) * f),
            sqrt$2(pow$5(z1,2) * (1-f) + pow$5(z2,2) * f),
            'rgb'
        )
    };

    // register interpolator
    interpolator$1.lrgb = lrgb;

    var Color$a = Color_1;

    var lab = function (col1, col2, f) {
        var xyz0 = col1.lab();
        var xyz1 = col2.lab();
        return new Color$a(
            xyz0[0] + f * (xyz1[0]-xyz0[0]),
            xyz0[1] + f * (xyz1[1]-xyz0[1]),
            xyz0[2] + f * (xyz1[2]-xyz0[2]),
            'lab'
        )
    };

    // register interpolator
    interpolator$1.lab = lab;

    var Color$9 = Color_1;

    var _hsx = function (col1, col2, f, m) {
        var assign, assign$1;

        var xyz0, xyz1;
        if (m === 'hsl') {
            xyz0 = col1.hsl();
            xyz1 = col2.hsl();
        } else if (m === 'hsv') {
            xyz0 = col1.hsv();
            xyz1 = col2.hsv();
        } else if (m === 'hcg') {
            xyz0 = col1.hcg();
            xyz1 = col2.hcg();
        } else if (m === 'hsi') {
            xyz0 = col1.hsi();
            xyz1 = col2.hsi();
        } else if (m === 'lch' || m === 'hcl') {
            m = 'hcl';
            xyz0 = col1.hcl();
            xyz1 = col2.hcl();
        } else if (m === 'oklch') {
            xyz0 = col1.oklch().reverse();
            xyz1 = col2.oklch().reverse();
        }

        var hue0, hue1, sat0, sat1, lbv0, lbv1;
        if (m.substr(0, 1) === 'h' || m === 'oklch') {
            (assign = xyz0, hue0 = assign[0], sat0 = assign[1], lbv0 = assign[2]);
            (assign$1 = xyz1, hue1 = assign$1[0], sat1 = assign$1[1], lbv1 = assign$1[2]);
        }

        var sat, hue, lbv, dh;

        if (!isNaN(hue0) && !isNaN(hue1)) {
            // both colors have hue
            if (hue1 > hue0 && hue1 - hue0 > 180) {
                dh = hue1 - (hue0 + 360);
            } else if (hue1 < hue0 && hue0 - hue1 > 180) {
                dh = hue1 + 360 - hue0;
            } else {
                dh = hue1 - hue0;
            }
            hue = hue0 + f * dh;
        } else if (!isNaN(hue0)) {
            hue = hue0;
            if ((lbv1 == 1 || lbv1 == 0) && m != 'hsv') { sat = sat0; }
        } else if (!isNaN(hue1)) {
            hue = hue1;
            if ((lbv0 == 1 || lbv0 == 0) && m != 'hsv') { sat = sat1; }
        } else {
            hue = Number.NaN;
        }

        if (sat === undefined) { sat = sat0 + f * (sat1 - sat0); }
        lbv = lbv0 + f * (lbv1 - lbv0);
        return m === 'oklch' ? new Color$9([lbv, sat, hue], m) : new Color$9([hue, sat, lbv], m);
    };

    var interpolate_hsx$5 = _hsx;

    var lch = function (col1, col2, f) {
    	return interpolate_hsx$5(col1, col2, f, 'lch');
    };

    // register interpolator
    interpolator$1.lch = lch;
    interpolator$1.hcl = lch;

    var Color$8 = Color_1;

    var num = function (col1, col2, f) {
        var c1 = col1.num();
        var c2 = col2.num();
        return new Color$8(c1 + f * (c2-c1), 'num')
    };

    // register interpolator
    interpolator$1.num = num;

    var interpolate_hsx$4 = _hsx;

    var hcg = function (col1, col2, f) {
    	return interpolate_hsx$4(col1, col2, f, 'hcg');
    };

    // register interpolator
    interpolator$1.hcg = hcg;

    var interpolate_hsx$3 = _hsx;

    var hsi = function (col1, col2, f) {
    	return interpolate_hsx$3(col1, col2, f, 'hsi');
    };

    // register interpolator
    interpolator$1.hsi = hsi;

    var interpolate_hsx$2 = _hsx;

    var hsl = function (col1, col2, f) {
    	return interpolate_hsx$2(col1, col2, f, 'hsl');
    };

    // register interpolator
    interpolator$1.hsl = hsl;

    var interpolate_hsx$1 = _hsx;

    var hsv = function (col1, col2, f) {
    	return interpolate_hsx$1(col1, col2, f, 'hsv');
    };

    // register interpolator
    interpolator$1.hsv = hsv;

    var Color$7 = Color_1;

    var oklab = function (col1, col2, f) {
        var xyz0 = col1.oklab();
        var xyz1 = col2.oklab();
        return new Color$7(
            xyz0[0] + f * (xyz1[0] - xyz0[0]),
            xyz0[1] + f * (xyz1[1] - xyz0[1]),
            xyz0[2] + f * (xyz1[2] - xyz0[2]),
            'oklab'
        );
    };

    // register interpolator
    interpolator$1.oklab = oklab;

    var interpolate_hsx = _hsx;

    var oklch = function (col1, col2, f) {
        return interpolate_hsx(col1, col2, f, 'oklch');
    };

    // register interpolator
    interpolator$1.oklch = oklch;

    var Color$6 = Color_1;
    var clip_rgb$1 = utils.clip_rgb;
    var pow$4 = Math.pow;
    var sqrt$1 = Math.sqrt;
    var PI$1 = Math.PI;
    var cos$2 = Math.cos;
    var sin$2 = Math.sin;
    var atan2$1 = Math.atan2;

    var average = function (colors, mode, weights) {
        if ( mode === void 0 ) mode='lrgb';
        if ( weights === void 0 ) weights=null;

        var l = colors.length;
        if (!weights) { weights = Array.from(new Array(l)).map(function () { return 1; }); }
        // normalize weights
        var k = l / weights.reduce(function(a, b) { return a + b; });
        weights.forEach(function (w,i) { weights[i] *= k; });
        // convert colors to Color objects
        colors = colors.map(function (c) { return new Color$6(c); });
        if (mode === 'lrgb') {
            return _average_lrgb(colors, weights)
        }
        var first = colors.shift();
        var xyz = first.get(mode);
        var cnt = [];
        var dx = 0;
        var dy = 0;
        // initial color
        for (var i=0; i<xyz.length; i++) {
            xyz[i] = (xyz[i] || 0) * weights[0];
            cnt.push(isNaN(xyz[i]) ? 0 : weights[0]);
            if (mode.charAt(i) === 'h' && !isNaN(xyz[i])) {
                var A = xyz[i] / 180 * PI$1;
                dx += cos$2(A) * weights[0];
                dy += sin$2(A) * weights[0];
            }
        }

        var alpha = first.alpha() * weights[0];
        colors.forEach(function (c,ci) {
            var xyz2 = c.get(mode);
            alpha += c.alpha() * weights[ci+1];
            for (var i=0; i<xyz.length; i++) {
                if (!isNaN(xyz2[i])) {
                    cnt[i] += weights[ci+1];
                    if (mode.charAt(i) === 'h') {
                        var A = xyz2[i] / 180 * PI$1;
                        dx += cos$2(A) * weights[ci+1];
                        dy += sin$2(A) * weights[ci+1];
                    } else {
                        xyz[i] += xyz2[i] * weights[ci+1];
                    }
                }
            }
        });

        for (var i$1=0; i$1<xyz.length; i$1++) {
            if (mode.charAt(i$1) === 'h') {
                var A$1 = atan2$1(dy / cnt[i$1], dx / cnt[i$1]) / PI$1 * 180;
                while (A$1 < 0) { A$1 += 360; }
                while (A$1 >= 360) { A$1 -= 360; }
                xyz[i$1] = A$1;
            } else {
                xyz[i$1] = xyz[i$1]/cnt[i$1];
            }
        }
        alpha /= l;
        return (new Color$6(xyz, mode)).alpha(alpha > 0.99999 ? 1 : alpha, true);
    };


    var _average_lrgb = function (colors, weights) {
        var l = colors.length;
        var xyz = [0,0,0,0];
        for (var i=0; i < colors.length; i++) {
            var col = colors[i];
            var f = weights[i] / l;
            var rgb = col._rgb;
            xyz[0] += pow$4(rgb[0],2) * f;
            xyz[1] += pow$4(rgb[1],2) * f;
            xyz[2] += pow$4(rgb[2],2) * f;
            xyz[3] += rgb[3] * f;
        }
        xyz[0] = sqrt$1(xyz[0]);
        xyz[1] = sqrt$1(xyz[1]);
        xyz[2] = sqrt$1(xyz[2]);
        if (xyz[3] > 0.9999999) { xyz[3] = 1; }
        return new Color$6(clip_rgb$1(xyz));
    };

    // minimal multi-purpose interface

    // @requires utils color analyze

    var chroma$4 = chroma_1;
    var type$2 = utils.type;

    var pow$3 = Math.pow;

    var scale$2 = function(colors) {

        // constructor
        var _mode = 'rgb';
        var _nacol = chroma$4('#ccc');
        var _spread = 0;
        // const _fixed = false;
        var _domain = [0, 1];
        var _pos = [];
        var _padding = [0,0];
        var _classes = false;
        var _colors = [];
        var _out = false;
        var _min = 0;
        var _max = 1;
        var _correctLightness = false;
        var _colorCache = {};
        var _useCache = true;
        var _gamma = 1;

        // private methods

        var setColors = function(colors) {
            colors = colors || ['#fff', '#000'];
            if (colors && type$2(colors) === 'string' && chroma$4.brewer &&
                chroma$4.brewer[colors.toLowerCase()]) {
                colors = chroma$4.brewer[colors.toLowerCase()];
            }
            if (type$2(colors) === 'array') {
                // handle single color
                if (colors.length === 1) {
                    colors = [colors[0], colors[0]];
                }
                // make a copy of the colors
                colors = colors.slice(0);
                // convert to chroma classes
                for (var c=0; c<colors.length; c++) {
                    colors[c] = chroma$4(colors[c]);
                }
                // auto-fill color position
                _pos.length = 0;
                for (var c$1=0; c$1<colors.length; c$1++) {
                    _pos.push(c$1/(colors.length-1));
                }
            }
            resetCache();
            return _colors = colors;
        };

        var getClass = function(value) {
            if (_classes != null) {
                var n = _classes.length-1;
                var i = 0;
                while (i < n && value >= _classes[i]) {
                    i++;
                }
                return i-1;
            }
            return 0;
        };

        var tMapLightness = function (t) { return t; };
        var tMapDomain = function (t) { return t; };

        // const classifyValue = function(value) {
        //     let val = value;
        //     if (_classes.length > 2) {
        //         const n = _classes.length-1;
        //         const i = getClass(value);
        //         const minc = _classes[0] + ((_classes[1]-_classes[0]) * (0 + (_spread * 0.5)));  // center of 1st class
        //         const maxc = _classes[n-1] + ((_classes[n]-_classes[n-1]) * (1 - (_spread * 0.5)));  // center of last class
        //         val = _min + ((((_classes[i] + ((_classes[i+1] - _classes[i]) * 0.5)) - minc) / (maxc-minc)) * (_max - _min));
        //     }
        //     return val;
        // };

        var getColor = function(val, bypassMap) {
            var col, t;
            if (bypassMap == null) { bypassMap = false; }
            if (isNaN(val) || (val === null)) { return _nacol; }
            if (!bypassMap) {
                if (_classes && (_classes.length > 2)) {
                    // find the class
                    var c = getClass(val);
                    t = c / (_classes.length-2);
                } else if (_max !== _min) {
                    // just interpolate between min/max
                    t = (val - _min) / (_max - _min);
                } else {
                    t = 1;
                }
            } else {
                t = val;
            }

            // domain map
            t = tMapDomain(t);

            if (!bypassMap) {
                t = tMapLightness(t);  // lightness correction
            }

            if (_gamma !== 1) { t = pow$3(t, _gamma); }

            t = _padding[0] + (t * (1 - _padding[0] - _padding[1]));

            t = Math.min(1, Math.max(0, t));

            var k = Math.floor(t * 10000);

            if (_useCache && _colorCache[k]) {
                col = _colorCache[k];
            } else {
                if (type$2(_colors) === 'array') {
                    //for i in [0.._pos.length-1]
                    for (var i=0; i<_pos.length; i++) {
                        var p = _pos[i];
                        if (t <= p) {
                            col = _colors[i];
                            break;
                        }
                        if ((t >= p) && (i === (_pos.length-1))) {
                            col = _colors[i];
                            break;
                        }
                        if (t > p && t < _pos[i+1]) {
                            t = (t-p)/(_pos[i+1]-p);
                            col = chroma$4.interpolate(_colors[i], _colors[i+1], t, _mode);
                            break;
                        }
                    }
                } else if (type$2(_colors) === 'function') {
                    col = _colors(t);
                }
                if (_useCache) { _colorCache[k] = col; }
            }
            return col;
        };

        var resetCache = function () { return _colorCache = {}; };

        setColors(colors);

        // public interface

        var f = function(v) {
            var c = chroma$4(getColor(v));
            if (_out && c[_out]) { return c[_out](); } else { return c; }
        };

        f.classes = function(classes) {
            if (classes != null) {
                if (type$2(classes) === 'array') {
                    _classes = classes;
                    _domain = [classes[0], classes[classes.length-1]];
                } else {
                    var d = chroma$4.analyze(_domain);
                    if (classes === 0) {
                        _classes = [d.min, d.max];
                    } else {
                        _classes = chroma$4.limits(d, 'e', classes);
                    }
                }
                return f;
            }
            return _classes;
        };


        f.domain = function(domain) {
            if (!arguments.length) {
                return _domain;
            }
            _min = domain[0];
            _max = domain[domain.length-1];
            _pos = [];
            var k = _colors.length;
            if ((domain.length === k) && (_min !== _max)) {
                // update positions
                for (var i = 0, list = Array.from(domain); i < list.length; i += 1) {
                    var d = list[i];

                  _pos.push((d-_min) / (_max-_min));
                }
            } else {
                for (var c=0; c<k; c++) {
                    _pos.push(c/(k-1));
                }
                if (domain.length > 2) {
                    // set domain map
                    var tOut = domain.map(function (d,i) { return i/(domain.length-1); });
                    var tBreaks = domain.map(function (d) { return (d - _min) / (_max - _min); });
                    if (!tBreaks.every(function (val, i) { return tOut[i] === val; })) {
                        tMapDomain = function (t) {
                            if (t <= 0 || t >= 1) { return t; }
                            var i = 0;
                            while (t >= tBreaks[i+1]) { i++; }
                            var f = (t - tBreaks[i]) / (tBreaks[i+1] - tBreaks[i]);
                            var out = tOut[i] + f * (tOut[i+1] - tOut[i]);
                            return out;
                        };
                    }

                }
            }
            _domain = [_min, _max];
            return f;
        };

        f.mode = function(_m) {
            if (!arguments.length) {
                return _mode;
            }
            _mode = _m;
            resetCache();
            return f;
        };

        f.range = function(colors, _pos) {
            setColors(colors);
            return f;
        };

        f.out = function(_o) {
            _out = _o;
            return f;
        };

        f.spread = function(val) {
            if (!arguments.length) {
                return _spread;
            }
            _spread = val;
            return f;
        };

        f.correctLightness = function(v) {
            if (v == null) { v = true; }
            _correctLightness = v;
            resetCache();
            if (_correctLightness) {
                tMapLightness = function(t) {
                    var L0 = getColor(0, true).lab()[0];
                    var L1 = getColor(1, true).lab()[0];
                    var pol = L0 > L1;
                    var L_actual = getColor(t, true).lab()[0];
                    var L_ideal = L0 + ((L1 - L0) * t);
                    var L_diff = L_actual - L_ideal;
                    var t0 = 0;
                    var t1 = 1;
                    var max_iter = 20;
                    while ((Math.abs(L_diff) > 1e-2) && (max_iter-- > 0)) {
                        (function() {
                            if (pol) { L_diff *= -1; }
                            if (L_diff < 0) {
                                t0 = t;
                                t += (t1 - t) * 0.5;
                            } else {
                                t1 = t;
                                t += (t0 - t) * 0.5;
                            }
                            L_actual = getColor(t, true).lab()[0];
                            return L_diff = L_actual - L_ideal;
                        })();
                    }
                    return t;
                };
            } else {
                tMapLightness = function (t) { return t; };
            }
            return f;
        };

        f.padding = function(p) {
            if (p != null) {
                if (type$2(p) === 'number') {
                    p = [p,p];
                }
                _padding = p;
                return f;
            } else {
                return _padding;
            }
        };

        f.colors = function(numColors, out) {
            // If no arguments are given, return the original colors that were provided
            if (arguments.length < 2) { out = 'hex'; }
            var result = [];

            if (arguments.length === 0) {
                result = _colors.slice(0);

            } else if (numColors === 1) {
                result = [f(0.5)];

            } else if (numColors > 1) {
                var dm = _domain[0];
                var dd = _domain[1] - dm;
                result = __range__(0, numColors, false).map(function (i) { return f( dm + ((i/(numColors-1)) * dd) ); });

            } else { // returns all colors based on the defined classes
                colors = [];
                var samples = [];
                if (_classes && (_classes.length > 2)) {
                    for (var i = 1, end = _classes.length, asc = 1 <= end; asc ? i < end : i > end; asc ? i++ : i--) {
                        samples.push((_classes[i-1]+_classes[i])*0.5);
                    }
                } else {
                    samples = _domain;
                }
                result = samples.map(function (v) { return f(v); });
            }

            if (chroma$4[out]) {
                result = result.map(function (c) { return c[out](); });
            }
            return result;
        };

        f.cache = function(c) {
            if (c != null) {
                _useCache = c;
                return f;
            } else {
                return _useCache;
            }
        };

        f.gamma = function(g) {
            if (g != null) {
                _gamma = g;
                return f;
            } else {
                return _gamma;
            }
        };

        f.nodata = function(d) {
            if (d != null) {
                _nacol = chroma$4(d);
                return f;
            } else {
                return _nacol;
            }
        };

        return f;
    };

    function __range__(left, right, inclusive) {
      var range = [];
      var ascending = left < right;
      var end = !inclusive ? right : ascending ? right + 1 : right - 1;
      for (var i = left; ascending ? i < end : i > end; ascending ? i++ : i--) {
        range.push(i);
      }
      return range;
    }

    //
    // interpolates between a set of colors uzing a bezier spline
    //

    // @requires utils lab
    var Color$5 = Color_1;

    var scale$1 = scale$2;

    // nth row of the pascal triangle
    var binom_row = function(n) {
        var row = [1, 1];
        for (var i = 1; i < n; i++) {
            var newrow = [1];
            for (var j = 1; j <= row.length; j++) {
                newrow[j] = (row[j] || 0) + row[j - 1];
            }
            row = newrow;
        }
        return row;
    };

    var bezier = function(colors) {
        var assign, assign$1, assign$2;

        var I, lab0, lab1, lab2;
        colors = colors.map(function (c) { return new Color$5(c); });
        if (colors.length === 2) {
            // linear interpolation
            (assign = colors.map(function (c) { return c.lab(); }), lab0 = assign[0], lab1 = assign[1]);
            I = function(t) {
                var lab = ([0, 1, 2].map(function (i) { return lab0[i] + (t * (lab1[i] - lab0[i])); }));
                return new Color$5(lab, 'lab');
            };
        } else if (colors.length === 3) {
            // quadratic bezier interpolation
            (assign$1 = colors.map(function (c) { return c.lab(); }), lab0 = assign$1[0], lab1 = assign$1[1], lab2 = assign$1[2]);
            I = function(t) {
                var lab = ([0, 1, 2].map(function (i) { return ((1-t)*(1-t) * lab0[i]) + (2 * (1-t) * t * lab1[i]) + (t * t * lab2[i]); }));
                return new Color$5(lab, 'lab');
            };
        } else if (colors.length === 4) {
            // cubic bezier interpolation
            var lab3;
            (assign$2 = colors.map(function (c) { return c.lab(); }), lab0 = assign$2[0], lab1 = assign$2[1], lab2 = assign$2[2], lab3 = assign$2[3]);
            I = function(t) {
                var lab = ([0, 1, 2].map(function (i) { return ((1-t)*(1-t)*(1-t) * lab0[i]) + (3 * (1-t) * (1-t) * t * lab1[i]) + (3 * (1-t) * t * t * lab2[i]) + (t*t*t * lab3[i]); }));
                return new Color$5(lab, 'lab');
            };
        } else if (colors.length >= 5) {
            // general case (degree n bezier)
            var labs, row, n;
            labs = colors.map(function (c) { return c.lab(); });
            n = colors.length - 1;
            row = binom_row(n);
            I = function (t) {
                var u = 1 - t;
                var lab = ([0, 1, 2].map(function (i) { return labs.reduce(function (sum, el, j) { return (sum + row[j] * Math.pow( u, (n - j) ) * Math.pow( t, j ) * el[i]); }, 0); }));
                return new Color$5(lab, 'lab');
            };
        } else {
            throw new RangeError("No point in running bezier with only one color.")
        }
        return I;
    };

    var bezier_1 = function (colors) {
        var f = bezier(colors);
        f.scale = function () { return scale$1(f); };
        return f;
    };

    /*
     * interpolates between a set of colors uzing a bezier spline
     * blend mode formulas taken from http://www.venture-ware.com/kevin/coding/lets-learn-math-photoshop-blend-modes/
     */

    var chroma$3 = chroma_1;

    var blend = function (bottom, top, mode) {
        if (!blend[mode]) {
            throw new Error('unknown blend mode ' + mode);
        }
        return blend[mode](bottom, top);
    };

    var blend_f = function (f) { return function (bottom,top) {
            var c0 = chroma$3(top).rgb();
            var c1 = chroma$3(bottom).rgb();
            return chroma$3.rgb(f(c0, c1));
        }; };

    var each = function (f) { return function (c0, c1) {
            var out = [];
            out[0] = f(c0[0], c1[0]);
            out[1] = f(c0[1], c1[1]);
            out[2] = f(c0[2], c1[2]);
            return out;
        }; };

    var normal = function (a) { return a; };
    var multiply = function (a,b) { return a * b / 255; };
    var darken = function (a,b) { return a > b ? b : a; };
    var lighten = function (a,b) { return a > b ? a : b; };
    var screen = function (a,b) { return 255 * (1 - (1-a/255) * (1-b/255)); };
    var overlay = function (a,b) { return b < 128 ? 2 * a * b / 255 : 255 * (1 - 2 * (1 - a / 255 ) * ( 1 - b / 255 )); };
    var burn = function (a,b) { return 255 * (1 - (1 - b / 255) / (a/255)); };
    var dodge = function (a,b) {
        if (a === 255) { return 255; }
        a = 255 * (b / 255) / (1 - a / 255);
        return a > 255 ? 255 : a
    };

    // # add = (a,b) ->
    // #     if (a + b > 255) then 255 else a + b

    blend.normal = blend_f(each(normal));
    blend.multiply = blend_f(each(multiply));
    blend.screen = blend_f(each(screen));
    blend.overlay = blend_f(each(overlay));
    blend.darken = blend_f(each(darken));
    blend.lighten = blend_f(each(lighten));
    blend.dodge = blend_f(each(dodge));
    blend.burn = blend_f(each(burn));
    // blend.add = blend_f(each(add));

    var blend_1 = blend;

    // cubehelix interpolation
    // based on D.A. Green "A colour scheme for the display of astronomical intensity images"
    // http://astron-soc.in/bulletin/11June/289392011.pdf

    var type$1 = utils.type;
    var clip_rgb = utils.clip_rgb;
    var TWOPI = utils.TWOPI;
    var pow$2 = Math.pow;
    var sin$1 = Math.sin;
    var cos$1 = Math.cos;
    var chroma$2 = chroma_1;

    var cubehelix = function(start, rotations, hue, gamma, lightness) {
        if ( start === void 0 ) start=300;
        if ( rotations === void 0 ) rotations=-1.5;
        if ( hue === void 0 ) hue=1;
        if ( gamma === void 0 ) gamma=1;
        if ( lightness === void 0 ) lightness=[0,1];

        var dh = 0, dl;
        if (type$1(lightness) === 'array') {
            dl = lightness[1] - lightness[0];
        } else {
            dl = 0;
            lightness = [lightness, lightness];
        }

        var f = function(fract) {
            var a = TWOPI * (((start+120)/360) + (rotations * fract));
            var l = pow$2(lightness[0] + (dl * fract), gamma);
            var h = dh !== 0 ? hue[0] + (fract * dh) : hue;
            var amp = (h * l * (1-l)) / 2;
            var cos_a = cos$1(a);
            var sin_a = sin$1(a);
            var r = l + (amp * ((-0.14861 * cos_a) + (1.78277* sin_a)));
            var g = l + (amp * ((-0.29227 * cos_a) - (0.90649* sin_a)));
            var b = l + (amp * (+1.97294 * cos_a));
            return chroma$2(clip_rgb([r*255,g*255,b*255,1]));
        };

        f.start = function(s) {
            if ((s == null)) { return start; }
            start = s;
            return f;
        };

        f.rotations = function(r) {
            if ((r == null)) { return rotations; }
            rotations = r;
            return f;
        };

        f.gamma = function(g) {
            if ((g == null)) { return gamma; }
            gamma = g;
            return f;
        };

        f.hue = function(h) {
            if ((h == null)) { return hue; }
            hue = h;
            if (type$1(hue) === 'array') {
                dh = hue[1] - hue[0];
                if (dh === 0) { hue = hue[1]; }
            } else {
                dh = 0;
            }
            return f;
        };

        f.lightness = function(h) {
            if ((h == null)) { return lightness; }
            if (type$1(h) === 'array') {
                lightness = h;
                dl = h[1] - h[0];
            } else {
                lightness = [h,h];
                dl = 0;
            }
            return f;
        };

        f.scale = function () { return chroma$2.scale(f); };

        f.hue(hue);

        return f;
    };

    var Color$4 = Color_1;
    var digits = '0123456789abcdef';

    var floor$1 = Math.floor;
    var random = Math.random;

    var random_1 = function () {
        var code = '#';
        for (var i=0; i<6; i++) {
            code += digits.charAt(floor$1(random() * 16));
        }
        return new Color$4(code, 'hex');
    };

    var type = type$p;
    var log = Math.log;
    var pow$1 = Math.pow;
    var floor = Math.floor;
    var abs$1 = Math.abs;


    var analyze = function (data, key) {
        if ( key === void 0 ) key=null;

        var r = {
            min: Number.MAX_VALUE,
            max: Number.MAX_VALUE*-1,
            sum: 0,
            values: [],
            count: 0
        };
        if (type(data) === 'object') {
            data = Object.values(data);
        }
        data.forEach(function (val) {
            if (key && type(val) === 'object') { val = val[key]; }
            if (val !== undefined && val !== null && !isNaN(val)) {
                r.values.push(val);
                r.sum += val;
                if (val < r.min) { r.min = val; }
                if (val > r.max) { r.max = val; }
                r.count += 1;
            }
        });

        r.domain = [r.min, r.max];

        r.limits = function (mode, num) { return limits(r, mode, num); };

        return r;
    };


    var limits = function (data, mode, num) {
        if ( mode === void 0 ) mode='equal';
        if ( num === void 0 ) num=7;

        if (type(data) == 'array') {
            data = analyze(data);
        }
        var min = data.min;
        var max = data.max;
        var values = data.values.sort(function (a,b) { return a-b; });

        if (num === 1) { return [min,max]; }

        var limits = [];

        if (mode.substr(0,1) === 'c') { // continuous
            limits.push(min);
            limits.push(max);
        }

        if (mode.substr(0,1) === 'e') { // equal interval
            limits.push(min);
            for (var i=1; i<num; i++) {
                limits.push(min+((i/num)*(max-min)));
            }
            limits.push(max);
        }

        else if (mode.substr(0,1) === 'l') { // log scale
            if (min <= 0) {
                throw new Error('Logarithmic scales are only possible for values > 0');
            }
            var min_log = Math.LOG10E * log(min);
            var max_log = Math.LOG10E * log(max);
            limits.push(min);
            for (var i$1=1; i$1<num; i$1++) {
                limits.push(pow$1(10, min_log + ((i$1/num) * (max_log - min_log))));
            }
            limits.push(max);
        }

        else if (mode.substr(0,1) === 'q') { // quantile scale
            limits.push(min);
            for (var i$2=1; i$2<num; i$2++) {
                var p = ((values.length-1) * i$2)/num;
                var pb = floor(p);
                if (pb === p) {
                    limits.push(values[pb]);
                } else { // p > pb
                    var pr = p - pb;
                    limits.push((values[pb]*(1-pr)) + (values[pb+1]*pr));
                }
            }
            limits.push(max);

        }

        else if (mode.substr(0,1) === 'k') { // k-means clustering
            /*
            implementation based on
            http://code.google.com/p/figue/source/browse/trunk/figue.js#336
            simplified for 1-d input values
            */
            var cluster;
            var n = values.length;
            var assignments = new Array(n);
            var clusterSizes = new Array(num);
            var repeat = true;
            var nb_iters = 0;
            var centroids = null;

            // get seed values
            centroids = [];
            centroids.push(min);
            for (var i$3=1; i$3<num; i$3++) {
                centroids.push(min + ((i$3/num) * (max-min)));
            }
            centroids.push(max);

            while (repeat) {
                // assignment step
                for (var j=0; j<num; j++) {
                    clusterSizes[j] = 0;
                }
                for (var i$4=0; i$4<n; i$4++) {
                    var value = values[i$4];
                    var mindist = Number.MAX_VALUE;
                    var best = (void 0);
                    for (var j$1=0; j$1<num; j$1++) {
                        var dist = abs$1(centroids[j$1]-value);
                        if (dist < mindist) {
                            mindist = dist;
                            best = j$1;
                        }
                        clusterSizes[best]++;
                        assignments[i$4] = best;
                    }
                }

                // update centroids step
                var newCentroids = new Array(num);
                for (var j$2=0; j$2<num; j$2++) {
                    newCentroids[j$2] = null;
                }
                for (var i$5=0; i$5<n; i$5++) {
                    cluster = assignments[i$5];
                    if (newCentroids[cluster] === null) {
                        newCentroids[cluster] = values[i$5];
                    } else {
                        newCentroids[cluster] += values[i$5];
                    }
                }
                for (var j$3=0; j$3<num; j$3++) {
                    newCentroids[j$3] *= 1/clusterSizes[j$3];
                }

                // check convergence
                repeat = false;
                for (var j$4=0; j$4<num; j$4++) {
                    if (newCentroids[j$4] !== centroids[j$4]) {
                        repeat = true;
                        break;
                    }
                }

                centroids = newCentroids;
                nb_iters++;

                if (nb_iters > 200) {
                    repeat = false;
                }
            }

            // finished k-means clustering
            // the next part is borrowed from gabrielflor.it
            var kClusters = {};
            for (var j$5=0; j$5<num; j$5++) {
                kClusters[j$5] = [];
            }
            for (var i$6=0; i$6<n; i$6++) {
                cluster = assignments[i$6];
                kClusters[cluster].push(values[i$6]);
            }
            var tmpKMeansBreaks = [];
            for (var j$6=0; j$6<num; j$6++) {
                tmpKMeansBreaks.push(kClusters[j$6][0]);
                tmpKMeansBreaks.push(kClusters[j$6][kClusters[j$6].length-1]);
            }
            tmpKMeansBreaks = tmpKMeansBreaks.sort(function (a,b){ return a-b; });
            limits.push(tmpKMeansBreaks[0]);
            for (var i$7=1; i$7 < tmpKMeansBreaks.length; i$7+= 2) {
                var v = tmpKMeansBreaks[i$7];
                if (!isNaN(v) && (limits.indexOf(v) === -1)) {
                    limits.push(v);
                }
            }
        }
        return limits;
    };

    var analyze_1 = {analyze: analyze, limits: limits};

    var Color$3 = Color_1;


    var contrast = function (a, b) {
        // WCAG contrast ratio
        // see http://www.w3.org/TR/2008/REC-WCAG20-20081211/#contrast-ratiodef
        a = new Color$3(a);
        b = new Color$3(b);
        var l1 = a.luminance();
        var l2 = b.luminance();
        return l1 > l2 ? (l1 + 0.05) / (l2 + 0.05) : (l2 + 0.05) / (l1 + 0.05);
    };

    var Color$2 = Color_1;
    var sqrt = Math.sqrt;
    var pow = Math.pow;
    var min = Math.min;
    var max = Math.max;
    var atan2 = Math.atan2;
    var abs = Math.abs;
    var cos = Math.cos;
    var sin = Math.sin;
    var exp = Math.exp;
    var PI = Math.PI;

    var deltaE = function(a, b, Kl, Kc, Kh) {
        if ( Kl === void 0 ) Kl=1;
        if ( Kc === void 0 ) Kc=1;
        if ( Kh === void 0 ) Kh=1;

        // Delta E (CIE 2000)
        // see http://www.brucelindbloom.com/index.html?Eqn_DeltaE_CIE2000.html
        var rad2deg = function(rad) {
            return 360 * rad / (2 * PI);
        };
        var deg2rad = function(deg) {
            return (2 * PI * deg) / 360;
        };
        a = new Color$2(a);
        b = new Color$2(b);
        var ref = Array.from(a.lab());
        var L1 = ref[0];
        var a1 = ref[1];
        var b1 = ref[2];
        var ref$1 = Array.from(b.lab());
        var L2 = ref$1[0];
        var a2 = ref$1[1];
        var b2 = ref$1[2];
        var avgL = (L1 + L2)/2;
        var C1 = sqrt(pow(a1, 2) + pow(b1, 2));
        var C2 = sqrt(pow(a2, 2) + pow(b2, 2));
        var avgC = (C1 + C2)/2;
        var G = 0.5*(1-sqrt(pow(avgC, 7)/(pow(avgC, 7) + pow(25, 7))));
        var a1p = a1*(1+G);
        var a2p = a2*(1+G);
        var C1p = sqrt(pow(a1p, 2) + pow(b1, 2));
        var C2p = sqrt(pow(a2p, 2) + pow(b2, 2));
        var avgCp = (C1p + C2p)/2;
        var arctan1 = rad2deg(atan2(b1, a1p));
        var arctan2 = rad2deg(atan2(b2, a2p));
        var h1p = arctan1 >= 0 ? arctan1 : arctan1 + 360;
        var h2p = arctan2 >= 0 ? arctan2 : arctan2 + 360;
        var avgHp = abs(h1p - h2p) > 180 ? (h1p + h2p + 360)/2 : (h1p + h2p)/2;
        var T = 1 - 0.17*cos(deg2rad(avgHp - 30)) + 0.24*cos(deg2rad(2*avgHp)) + 0.32*cos(deg2rad(3*avgHp + 6)) - 0.2*cos(deg2rad(4*avgHp - 63));
        var deltaHp = h2p - h1p;
        deltaHp = abs(deltaHp) <= 180 ? deltaHp : h2p <= h1p ? deltaHp + 360 : deltaHp - 360;
        deltaHp = 2*sqrt(C1p*C2p)*sin(deg2rad(deltaHp)/2);
        var deltaL = L2 - L1;
        var deltaCp = C2p - C1p;    
        var sl = 1 + (0.015*pow(avgL - 50, 2))/sqrt(20 + pow(avgL - 50, 2));
        var sc = 1 + 0.045*avgCp;
        var sh = 1 + 0.015*avgCp*T;
        var deltaTheta = 30*exp(-pow((avgHp - 275)/25, 2));
        var Rc = 2*sqrt(pow(avgCp, 7)/(pow(avgCp, 7) + pow(25, 7)));
        var Rt = -Rc*sin(2*deg2rad(deltaTheta));
        var result = sqrt(pow(deltaL/(Kl*sl), 2) + pow(deltaCp/(Kc*sc), 2) + pow(deltaHp/(Kh*sh), 2) + Rt*(deltaCp/(Kc*sc))*(deltaHp/(Kh*sh)));
        return max(0, min(100, result));
    };

    var Color$1 = Color_1;

    // simple Euclidean distance
    var distance = function(a, b, mode) {
        if ( mode === void 0 ) mode='lab';

        // Delta E (CIE 1976)
        // see http://www.brucelindbloom.com/index.html?Equations.html
        a = new Color$1(a);
        b = new Color$1(b);
        var l1 = a.get(mode);
        var l2 = b.get(mode);
        var sum_sq = 0;
        for (var i in l1) {
            var d = (l1[i] || 0) - (l2[i] || 0);
            sum_sq += d*d;
        }
        return Math.sqrt(sum_sq);
    };

    var Color = Color_1;

    var valid = function () {
        var args = [], len = arguments.length;
        while ( len-- ) args[ len ] = arguments[ len ];

        try {
            new (Function.prototype.bind.apply( Color, [ null ].concat( args) ));
            return true;
        } catch (e) {
            return false;
        }
    };

    // some pre-defined color scales:
    var chroma$1 = chroma_1;

    var scale = scale$2;

    var scales = {
    	cool: function cool() { return scale([chroma$1.hsl(180,1,.9), chroma$1.hsl(250,.7,.4)]) },
    	hot: function hot() { return scale(['#000','#f00','#ff0','#fff']).mode('rgb') }
    };

    /**
        ColorBrewer colors for chroma.js

        Copyright (c) 2002 Cynthia Brewer, Mark Harrower, and The
        Pennsylvania State University.

        Licensed under the Apache License, Version 2.0 (the "License");
        you may not use this file except in compliance with the License.
        You may obtain a copy of the License at
        http://www.apache.org/licenses/LICENSE-2.0

        Unless required by applicable law or agreed to in writing, software distributed
        under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
        CONDITIONS OF ANY KIND, either express or implied. See the License for the
        specific language governing permissions and limitations under the License.
    */

    var colorbrewer = {
        // sequential
        OrRd: ['#fff7ec', '#fee8c8', '#fdd49e', '#fdbb84', '#fc8d59', '#ef6548', '#d7301f', '#b30000', '#7f0000'],
        PuBu: ['#fff7fb', '#ece7f2', '#d0d1e6', '#a6bddb', '#74a9cf', '#3690c0', '#0570b0', '#045a8d', '#023858'],
        BuPu: ['#f7fcfd', '#e0ecf4', '#bfd3e6', '#9ebcda', '#8c96c6', '#8c6bb1', '#88419d', '#810f7c', '#4d004b'],
        Oranges: ['#fff5eb', '#fee6ce', '#fdd0a2', '#fdae6b', '#fd8d3c', '#f16913', '#d94801', '#a63603', '#7f2704'],
        BuGn: ['#f7fcfd', '#e5f5f9', '#ccece6', '#99d8c9', '#66c2a4', '#41ae76', '#238b45', '#006d2c', '#00441b'],
        YlOrBr: ['#ffffe5', '#fff7bc', '#fee391', '#fec44f', '#fe9929', '#ec7014', '#cc4c02', '#993404', '#662506'],
        YlGn: ['#ffffe5', '#f7fcb9', '#d9f0a3', '#addd8e', '#78c679', '#41ab5d', '#238443', '#006837', '#004529'],
        Reds: ['#fff5f0', '#fee0d2', '#fcbba1', '#fc9272', '#fb6a4a', '#ef3b2c', '#cb181d', '#a50f15', '#67000d'],
        RdPu: ['#fff7f3', '#fde0dd', '#fcc5c0', '#fa9fb5', '#f768a1', '#dd3497', '#ae017e', '#7a0177', '#49006a'],
        Greens: ['#f7fcf5', '#e5f5e0', '#c7e9c0', '#a1d99b', '#74c476', '#41ab5d', '#238b45', '#006d2c', '#00441b'],
        YlGnBu: ['#ffffd9', '#edf8b1', '#c7e9b4', '#7fcdbb', '#41b6c4', '#1d91c0', '#225ea8', '#253494', '#081d58'],
        Purples: ['#fcfbfd', '#efedf5', '#dadaeb', '#bcbddc', '#9e9ac8', '#807dba', '#6a51a3', '#54278f', '#3f007d'],
        GnBu: ['#f7fcf0', '#e0f3db', '#ccebc5', '#a8ddb5', '#7bccc4', '#4eb3d3', '#2b8cbe', '#0868ac', '#084081'],
        Greys: ['#ffffff', '#f0f0f0', '#d9d9d9', '#bdbdbd', '#969696', '#737373', '#525252', '#252525', '#000000'],
        YlOrRd: ['#ffffcc', '#ffeda0', '#fed976', '#feb24c', '#fd8d3c', '#fc4e2a', '#e31a1c', '#bd0026', '#800026'],
        PuRd: ['#f7f4f9', '#e7e1ef', '#d4b9da', '#c994c7', '#df65b0', '#e7298a', '#ce1256', '#980043', '#67001f'],
        Blues: ['#f7fbff', '#deebf7', '#c6dbef', '#9ecae1', '#6baed6', '#4292c6', '#2171b5', '#08519c', '#08306b'],
        PuBuGn: ['#fff7fb', '#ece2f0', '#d0d1e6', '#a6bddb', '#67a9cf', '#3690c0', '#02818a', '#016c59', '#014636'],
        Viridis: ['#440154', '#482777', '#3f4a8a', '#31678e', '#26838f', '#1f9d8a', '#6cce5a', '#b6de2b', '#fee825'],

        // diverging

        Spectral: ['#9e0142', '#d53e4f', '#f46d43', '#fdae61', '#fee08b', '#ffffbf', '#e6f598', '#abdda4', '#66c2a5', '#3288bd', '#5e4fa2'],
        RdYlGn: ['#a50026', '#d73027', '#f46d43', '#fdae61', '#fee08b', '#ffffbf', '#d9ef8b', '#a6d96a', '#66bd63', '#1a9850', '#006837'],
        RdBu: ['#67001f', '#b2182b', '#d6604d', '#f4a582', '#fddbc7', '#f7f7f7', '#d1e5f0', '#92c5de', '#4393c3', '#2166ac', '#053061'],
        PiYG: ['#8e0152', '#c51b7d', '#de77ae', '#f1b6da', '#fde0ef', '#f7f7f7', '#e6f5d0', '#b8e186', '#7fbc41', '#4d9221', '#276419'],
        PRGn: ['#40004b', '#762a83', '#9970ab', '#c2a5cf', '#e7d4e8', '#f7f7f7', '#d9f0d3', '#a6dba0', '#5aae61', '#1b7837', '#00441b'],
        RdYlBu: ['#a50026', '#d73027', '#f46d43', '#fdae61', '#fee090', '#ffffbf', '#e0f3f8', '#abd9e9', '#74add1', '#4575b4', '#313695'],
        BrBG: ['#543005', '#8c510a', '#bf812d', '#dfc27d', '#f6e8c3', '#f5f5f5', '#c7eae5', '#80cdc1', '#35978f', '#01665e', '#003c30'],
        RdGy: ['#67001f', '#b2182b', '#d6604d', '#f4a582', '#fddbc7', '#ffffff', '#e0e0e0', '#bababa', '#878787', '#4d4d4d', '#1a1a1a'],
        PuOr: ['#7f3b08', '#b35806', '#e08214', '#fdb863', '#fee0b6', '#f7f7f7', '#d8daeb', '#b2abd2', '#8073ac', '#542788', '#2d004b'],

        // qualitative

        Set2: ['#66c2a5', '#fc8d62', '#8da0cb', '#e78ac3', '#a6d854', '#ffd92f', '#e5c494', '#b3b3b3'],
        Accent: ['#7fc97f', '#beaed4', '#fdc086', '#ffff99', '#386cb0', '#f0027f', '#bf5b17', '#666666'],
        Set1: ['#e41a1c', '#377eb8', '#4daf4a', '#984ea3', '#ff7f00', '#ffff33', '#a65628', '#f781bf', '#999999'],
        Set3: ['#8dd3c7', '#ffffb3', '#bebada', '#fb8072', '#80b1d3', '#fdb462', '#b3de69', '#fccde5', '#d9d9d9', '#bc80bd', '#ccebc5', '#ffed6f'],
        Dark2: ['#1b9e77', '#d95f02', '#7570b3', '#e7298a', '#66a61e', '#e6ab02', '#a6761d', '#666666'],
        Paired: ['#a6cee3', '#1f78b4', '#b2df8a', '#33a02c', '#fb9a99', '#e31a1c', '#fdbf6f', '#ff7f00', '#cab2d6', '#6a3d9a', '#ffff99', '#b15928'],
        Pastel2: ['#b3e2cd', '#fdcdac', '#cbd5e8', '#f4cae4', '#e6f5c9', '#fff2ae', '#f1e2cc', '#cccccc'],
        Pastel1: ['#fbb4ae', '#b3cde3', '#ccebc5', '#decbe4', '#fed9a6', '#ffffcc', '#e5d8bd', '#fddaec', '#f2f2f2'],
    };

    // add lowercase aliases for case-insensitive matches
    for (var i = 0, list = Object.keys(colorbrewer); i < list.length; i += 1) {
        var key = list[i];

        colorbrewer[key.toLowerCase()] = colorbrewer[key];
    }

    var colorbrewer_1 = colorbrewer;

    var chroma = chroma_1;

    // feel free to comment out anything to rollup
    // a smaller chroma.js built

    // io --> convert colors

















    // operators --> modify existing Colors










    // interpolators












    // generators -- > create new colors
    chroma.average = average;
    chroma.bezier = bezier_1;
    chroma.blend = blend_1;
    chroma.cubehelix = cubehelix;
    chroma.mix = chroma.interpolate = mix$1;
    chroma.random = random_1;
    chroma.scale = scale$2;

    // other utility methods
    chroma.analyze = analyze_1.analyze;
    chroma.contrast = contrast;
    chroma.deltaE = deltaE;
    chroma.distance = distance;
    chroma.limits = analyze_1.limits;
    chroma.valid = valid;

    // scale
    chroma.scales = scales;

    // colors
    chroma.colors = w3cx11_1;
    chroma.brewer = colorbrewer_1;

    var chroma_js = chroma;

    return chroma_js;

}));
}(chroma$1));

var chroma = chroma$1.exports;

/*
    ## Color 字典数据

    字典数据来源 [A nicer color palette for the web](http://clrs.cc/)
*/
var DICT$1 = {
  // name value nicer
  navy: {
    value: '#000080',
    nicer: '#001F3F'
  },
  blue: {
    value: '#0000ff',
    nicer: '#0074D9'
  },
  aqua: {
    value: '#00ffff',
    nicer: '#7FDBFF'
  },
  teal: {
    value: '#008080',
    nicer: '#39CCCC'
  },
  olive: {
    value: '#008000',
    nicer: '#3D9970'
  },
  green: {
    value: '#008000',
    nicer: '#2ECC40'
  },
  lime: {
    value: '#00ff00',
    nicer: '#01FF70'
  },
  yellow: {
    value: '#ffff00',
    nicer: '#FFDC00'
  },
  orange: {
    value: '#ffa500',
    nicer: '#FF851B'
  },
  red: {
    value: '#ff0000',
    nicer: '#FF4136'
  },
  maroon: {
    value: '#800000',
    nicer: '#85144B'
  },
  fuchsia: {
    value: '#ff00ff',
    nicer: '#F012BE'
  },
  purple: {
    value: '#800080',
    nicer: '#B10DC9'
  },
  silver: {
    value: '#c0c0c0',
    nicer: '#DDDDDD'
  },
  gray: {
    value: '#808080',
    nicer: '#AAAAAA'
  },
  black: {
    value: '#000000',
    nicer: '#111111'
  },
  white: {
    value: '#FFFFFF',
    nicer: '#FFFFFF'
  }
};

var RandomColor = [{
  hex: "#5c2223",
  name: "暗玉紫"
}, {
  hex: "#eea2a4",
  name: "牡丹粉红"
}, {
  hex: "#5a191b",
  name: "栗紫"
}, {
  hex: "#f07c82",
  name: "香叶红"
}, {
  hex: "#5a1216",
  name: "葡萄酱紫"
}, {
  hex: "#ed5a65",
  name: "艳红"
}, {
  hex: "#c04851",
  name: "玉红"
}, {
  hex: "#ee3f4d",
  name: "茶花红"
}, {
  hex: "#c02c38",
  name: "高粱红"
}, {
  hex: "#a7535a",
  name: "满江红"
}, {
  hex: "#e3b4b8",
  name: "鼠鼻红"
}, {
  hex: "#f0a1a8",
  name: "合欢红"
}, {
  hex: "#f1939c",
  name: "春梅红"
}, {
  hex: "#a61b29",
  name: "苋菜红"
}, {
  hex: "#894e54",
  name: "烟红"
}, {
  hex: "#c45a65",
  name: "莓红"
}, {
  hex: "#d11a2d",
  name: "鹅冠红"
}, {
  hex: "#c21f30",
  name: "枫叶红"
}, {
  hex: "#de1c31",
  name: "唐菖蒲红"
}, {
  hex: "#7c1823",
  name: "枣红"
}, {
  hex: "#541e24",
  name: "猪肝紫"
}, {
  hex: "#4c1f24",
  name: "葡萄紫"
}, {
  hex: "#82202b",
  name: "暗紫苑红"
}, {
  hex: "#82111f",
  name: "殷红"
}, {
  hex: "#ef475d",
  name: "草茉莉红"
}, {
  hex: "#4d1018",
  name: "酱紫"
}, {
  hex: "#ed556a",
  name: "山茶红"
}, {
  hex: "#7a7374",
  name: "锌灰"
}, {
  hex: "#f03752",
  name: "海棠红"
}, {
  hex: "#e6d2d5",
  name: "蓟粉红"
}, {
  hex: "#f0c9cf",
  name: "石蕊红"
}, {
  hex: "#ee2746",
  name: "淡曙红"
}, {
  hex: "#2b1216",
  name: "李紫"
}, {
  hex: "#ee4863",
  name: "石竹红"
}, {
  hex: "#e77c8e",
  name: "淡茜红"
}, {
  hex: "#500a16",
  name: "金鱼紫"
}, {
  hex: "#c27c88",
  name: "山黎豆红"
}, {
  hex: "#73575c",
  name: "鼠背灰"
}, {
  hex: "#ee4866",
  name: "淡蕊香红"
}, {
  hex: "#621624",
  name: "甘蔗紫"
}, {
  hex: "#ce5777",
  name: "月季红"
}, {
  hex: "#cc163a",
  name: "尖晶玉红"
}, {
  hex: "#f1c4cd",
  name: "水红"
}, {
  hex: "#eeb8c3",
  name: "姜红"
}, {
  hex: "#856d72",
  name: "芦灰"
}, {
  hex: "#2d0c13",
  name: "茄皮紫"
}, {
  hex: "#36282b",
  name: "苍蝇灰"
}, {
  hex: "#bf3553",
  name: "锦葵红"
}, {
  hex: "#ec9bad",
  name: "粉团花红"
}, {
  hex: "#63071c",
  name: "石竹紫"
}, {
  hex: "#30161c",
  name: "卵石紫"
}, {
  hex: "#eea6b7",
  name: "晶红"
}, {
  hex: "#e9ccd3",
  name: "芝兰紫"
}, {
  hex: "#eba0b3",
  name: "芍药耕红"
}, {
  hex: "#4f383e",
  name: "暮云灰"
}, {
  hex: "#ed9db2",
  name: "豇豆红"
}, {
  hex: "#ec8aa4",
  name: "报春红"
}, {
  hex: "#ec7696",
  name: "淡绛红"
}, {
  hex: "#ea7293",
  name: "凤仙花红"
}, {
  hex: "#ef82a0",
  name: "霞光红"
}, {
  hex: "#ec2c64",
  name: "喜蛋红"
}, {
  hex: "#eb507e",
  name: "夹竹桃红"
}, {
  hex: "#eb3c70",
  name: "松叶牡丹红"
}, {
  hex: "#ea517f",
  name: "莲瓣红"
}, {
  hex: "#de7897",
  name: "白芨红"
}, {
  hex: "#b598a1",
  name: "隐红灰"
}, {
  hex: "#ed2f6a",
  name: "榲桲红"
}, {
  hex: "#c5708b",
  name: "酢酱草红"
}, {
  hex: "#33141e",
  name: "火鹅紫"
}, {
  hex: "#621d34",
  name: "鹞冠紫"
}, {
  hex: "#ef3473",
  name: "品红"
}, {
  hex: "#382129",
  name: "磨石紫"
}, {
  hex: "#310f1b",
  name: "墨紫"
}, {
  hex: "#381924",
  name: "檀紫"
}, {
  hex: "#e16c96",
  name: "初荷红"
}, {
  hex: "#951c48",
  name: "菜头紫"
}, {
  hex: "#62102e",
  name: "葡萄酒红"
}, {
  hex: "#e0c8d1",
  name: "淡青紫"
}, {
  hex: "#d13c74",
  name: "菠根红"
}, {
  hex: "#4b1e2f",
  name: "海象紫"
}, {
  hex: "#ec4e8a",
  name: "兔眼红"
}, {
  hex: "#de3f7c",
  name: "嫩菱红"
}, {
  hex: "#a8456b",
  name: "洋葱紫"
}, {
  hex: "#ce5e8a",
  name: "吊钟花红"
}, {
  hex: "#461629",
  name: "绀紫"
}, {
  hex: "#ee2c79",
  name: "紫荆红"
}, {
  hex: "#ef498b",
  name: "扁豆花红"
}, {
  hex: "#ede3e7",
  name: "马鞭草紫"
}, {
  hex: "#ec2d7a",
  name: "藏花红"
}, {
  hex: "#482936",
  name: "斑鸠灰"
}, {
  hex: "#440e25",
  name: "古铜紫"
}, {
  hex: "#d2568c",
  name: "丹紫红"
}, {
  hex: "#e9d7df",
  name: "丁香淡紫"
}, {
  hex: "#d2357d",
  name: "玫瑰红"
}, {
  hex: "#36292f",
  name: "古鼎灰"
}, {
  hex: "#d276a3",
  name: "菱锰红"
}, {
  hex: "#c06f98",
  name: "樱草紫"
}, {
  hex: "#cc5595",
  name: "龙须红"
}, {
  hex: "#c35691",
  name: "电气石红"
}, {
  hex: "#ba2f7b",
  name: "玫瑰紫"
}, {
  hex: "#9b1e64",
  name: "苋菜紫"
}, {
  hex: "#5d3f51",
  name: "紫灰"
}, {
  hex: "#4e2a40",
  name: "龙睛鱼紫"
}, {
  hex: "#bc84a8",
  name: "青蛤壳紫"
}, {
  hex: "#c08eaf",
  name: "萝兰紫"
}, {
  hex: "#411c35",
  name: "荸荠紫"
}, {
  hex: "#ad6598",
  name: "豆蔻紫"
}, {
  hex: "#a35c8f",
  name: "扁豆紫"
}, {
  hex: "#681752",
  name: "牵牛紫"
}, {
  hex: "#894276",
  name: "芓紫"
}, {
  hex: "#7e2065",
  name: "葛巾紫"
}, {
  hex: "#8b2671",
  name: "青莲"
}, {
  hex: "#983680",
  name: "芥花紫"
}, {
  hex: "#c8adc4",
  name: "凤信紫"
}, {
  hex: "#1c0d1a",
  name: "深牵牛紫"
}, {
  hex: "#7e1671",
  name: "魏紫"
}, {
  hex: "#1e131d",
  name: "乌梅紫"
}, {
  hex: "#813c85",
  name: "桔梗紫"
}, {
  hex: "#d1c2d3",
  name: "淡牵牛紫"
}, {
  hex: "#3e3841",
  name: "剑锋紫"
}, {
  hex: "#815c94",
  name: "蕈紫"
}, {
  hex: "#806d9e",
  name: "槿紫"
}, {
  hex: "#e2e1e4",
  name: "芡食白"
}, {
  hex: "#322f3b",
  name: "龙葵紫"
}, {
  hex: "#8076a3",
  name: "藤萝紫"
}, {
  hex: "#35333c",
  name: "沙鱼灰"
}, {
  hex: "#22202e",
  name: "暗龙胆紫"
}, {
  hex: "#131124",
  name: "暗蓝紫"
}, {
  hex: "#302f4b",
  name: "野葡萄紫"
}, {
  hex: "#525288",
  name: "野菊紫"
}, {
  hex: "#2f2f35",
  name: "水牛灰"
}, {
  hex: "#ccccd6",
  name: "远山紫"
}, {
  hex: "#74759b",
  name: "螺甸紫"
}, {
  hex: "#1f2040",
  name: "晶石紫"
}, {
  hex: "#2e317c",
  name: "满天星紫"
}, {
  hex: "#a7a8bd",
  name: "淡蓝紫"
}, {
  hex: "#61649f",
  name: "山梗紫"
}, {
  hex: "#2d2e36",
  name: "牛角灰"
}, {
  hex: "#5e616d",
  name: "鱼尾灰"
}, {
  hex: "#47484c",
  name: "瓦罐灰"
}, {
  hex: "#0f1423",
  name: "钢蓝"
}, {
  hex: "#131824",
  name: "燕颔蓝"
}, {
  hex: "#475164",
  name: "鲸鱼灰"
}, {
  hex: "#2b333e",
  name: "青灰"
}, {
  hex: "#1c2938",
  name: "鸽蓝"
}, {
  hex: "#101f30",
  name: "暗蓝"
}, {
  hex: "#142334",
  name: "钢青"
}, {
  hex: "#15559a",
  name: "海涛蓝"
}, {
  hex: "#0f59a4",
  name: "飞燕草蓝"
}, {
  hex: "#1661ab",
  name: "靛青"
}, {
  hex: "#3170a7",
  name: "安安蓝"
}, {
  hex: "#346c9c",
  name: "海军蓝"
}, {
  hex: "#2775b6",
  name: "景泰蓝"
}, {
  hex: "#2b73af",
  name: "品蓝"
}, {
  hex: "#2474b5",
  name: "尼罗蓝"
}, {
  hex: "#4e7ca1",
  name: "蝶翅蓝"
}, {
  hex: "#2376b7",
  name: "花青"
}, {
  hex: "#144a74",
  name: "鷃蓝"
}, {
  hex: "#93b5cf",
  name: "星蓝"
}, {
  hex: "#2177b8",
  name: "虹蓝"
}, {
  hex: "#126bae",
  name: "柏林蓝"
}, {
  hex: "#1772b4",
  name: "群青"
}, {
  hex: "#baccd9",
  name: "云水蓝"
}, {
  hex: "#619ac3",
  name: "羽扇豆蓝"
}, {
  hex: "#495c69",
  name: "战舰灰"
}, {
  hex: "#8fb2c9",
  name: "晴山蓝"
}, {
  hex: "#5698c3",
  name: "睛蓝"
}, {
  hex: "#11659a",
  name: "搪磁蓝"
}, {
  hex: "#2983bb",
  name: "潮蓝"
}, {
  hex: "#1677b3",
  name: "天蓝"
}, {
  hex: "#c4cbcf",
  name: "大理石灰"
}, {
  hex: "#1177b0",
  name: "牵牛花蓝"
}, {
  hex: "#2486b9",
  name: "宝石蓝"
}, {
  hex: "#5e7987",
  name: "淡蓝灰"
}, {
  hex: "#74787a",
  name: "嫩灰"
}, {
  hex: "#cdd1d3",
  name: "银鱼白"
}, {
  hex: "#1781b5",
  name: "釉蓝"
}, {
  hex: "#66a9c9",
  name: "涧石蓝"
}, {
  hex: "#d0dfe6",
  name: "远天蓝"
}, {
  hex: "#2f90b9",
  name: "云山蓝"
}, {
  hex: "#8abcd1",
  name: "秋波蓝"
}, {
  hex: "#c3d7df",
  name: "井天蓝"
}, {
  hex: "#158bb8",
  name: "鸢尾蓝"
}, {
  hex: "#d8e3e7",
  name: "云峰白"
}, {
  hex: "#b2bbbe",
  name: "星灰"
}, {
  hex: "#1a94bc",
  name: "钴蓝"
}, {
  hex: "#5cb3cc",
  name: "碧青"
}, {
  hex: "#134857",
  name: "苍蓝"
}, {
  hex: "#132c33",
  name: "深灰蓝"
}, {
  hex: "#21373d",
  name: "灰蓝"
}, {
  hex: "#b0d5df",
  name: "湖水蓝"
}, {
  hex: "#22a2c3",
  name: "海青"
}, {
  hex: "#474b4c",
  name: "黄昏灰"
}, {
  hex: "#63bbd0",
  name: "霁青"
}, {
  hex: "#126e82",
  name: "玉鈫蓝"
}, {
  hex: "#0f95b0",
  name: "胆矾蓝"
}, {
  hex: "#1491a8",
  name: "樫鸟蓝"
}, {
  hex: "#c7d2d4",
  name: "鸥蓝"
}, {
  hex: "#1e9eb3",
  name: "翠蓝"
}, {
  hex: "#3b818c",
  name: "蜻蜓蓝"
}, {
  hex: "#0eb0c9",
  name: "孔雀蓝"
}, {
  hex: "#29b7cb",
  name: "蔚蓝"
}, {
  hex: "#51c4d3",
  name: "瀑布蓝"
}, {
  hex: "#7cabb1",
  name: "闪蓝"
}, {
  hex: "#10aec2",
  name: "甸子蓝"
}, {
  hex: "#648e93",
  name: "晚波蓝"
}, {
  hex: "#93d5dc",
  name: "清水蓝"
}, {
  hex: "#617172",
  name: "夏云灰"
}, {
  hex: "#c6e6e8",
  name: "海天蓝"
}, {
  hex: "#869d9d",
  name: "虾壳青"
}, {
  hex: "#57c3c2",
  name: "石绿"
}, {
  hex: "#c4d7d6",
  name: "穹灰"
}, {
  hex: "#12aa9c",
  name: "美蝶绿"
}, {
  hex: "#737c7b",
  name: "垩灰"
}, {
  hex: "#12a182",
  name: "蓝绿"
}, {
  hex: "#1ba784",
  name: "竹绿"
}, {
  hex: "#428675",
  name: "亚丁绿"
}, {
  hex: "#c0c4c3",
  name: "月影白"
}, {
  hex: "#248067",
  name: "海王绿"
}, {
  hex: "#1a3b32",
  name: "深海绿"
}, {
  hex: "#314a43",
  name: "绿灰"
}, {
  hex: "#2c9678",
  name: "青矾绿"
}, {
  hex: "#223e36",
  name: "苍绿"
}, {
  hex: "#497568",
  name: "飞泉绿"
}, {
  hex: "#141e1b",
  name: "莽丛绿"
}, {
  hex: "#69a794",
  name: "梧枝绿"
}, {
  hex: "#2bae85",
  name: "铜绿"
}, {
  hex: "#9abeaf",
  name: "草原远绿"
}, {
  hex: "#45b787",
  name: "蛙绿"
}, {
  hex: "#92b3a5",
  name: "浪花绿"
}, {
  hex: "#1f2623",
  name: "苷蓝绿"
}, {
  hex: "#83cbac",
  name: "粉绿"
}, {
  hex: "#70887d",
  name: "淡绿灰"
}, {
  hex: "#55bb8a",
  name: "麦苗绿"
}, {
  hex: "#20a162",
  name: "翠绿"
}, {
  hex: "#40a070",
  name: "葱绿"
}, {
  hex: "#1a6840",
  name: "荷叶绿"
}, {
  hex: "#61ac85",
  name: "淡绿"
}, {
  hex: "#68b88e",
  name: "田园绿"
}, {
  hex: "#a4cab6",
  name: "玉簪绿"
}, {
  hex: "#3c9566",
  name: "蟾绿"
}, {
  hex: "#5dbe8a",
  name: "蔻梢绿"
}, {
  hex: "#207f4c",
  name: "薄荷绿"
}, {
  hex: "#eef7f2",
  name: "月白"
}, {
  hex: "#579572",
  name: "蛋白石绿"
}, {
  hex: "#b9dec9",
  name: "竹篁绿"
}, {
  hex: "#229453",
  name: "孔雀绿"
}, {
  hex: "#20894d",
  name: "宫殿绿"
}, {
  hex: "#15231b",
  name: "云杉绿"
}, {
  hex: "#66c18c",
  name: "毛绿"
}, {
  hex: "#a4aca7",
  name: "冰山蓝"
}, {
  hex: "#8a988e",
  name: "明灰"
}, {
  hex: "#9eccab",
  name: "明绿"
}, {
  hex: "#83a78d",
  name: "松霜绿"
}, {
  hex: "#485b4d",
  name: "白屈菜绿"
}, {
  hex: "#5d655f",
  name: "狼烟灰"
}, {
  hex: "#6e8b74",
  name: "瓦松绿"
}, {
  hex: "#2b312c",
  name: "槲寄生绿"
}, {
  hex: "#c6dfc8",
  name: "淡翠绿"
}, {
  hex: "#41b349",
  name: "玉髓绿"
}, {
  hex: "#43b244",
  name: "鲜绿"
}, {
  hex: "#253d24",
  name: "油绿"
}, {
  hex: "#41ae3c",
  name: "宝石绿"
}, {
  hex: "#add5a2",
  name: "嘉陵水绿"
}, {
  hex: "#5e665b",
  name: "田螺绿"
}, {
  hex: "#8cc269",
  name: "水绿"
}, {
  hex: "#5bae23",
  name: "鹦鹉绿"
}, {
  hex: "#dfecd5",
  name: "艾背绿"
}, {
  hex: "#cad3c3",
  name: "艾绿"
}, {
  hex: "#9fa39a",
  name: "镍灰"
}, {
  hex: "#b2cf87",
  name: "橄榄石绿"
}, {
  hex: "#96c24e",
  name: "芽绿"
}, {
  hex: "#f0f5e5",
  name: "嫩菊绿"
}, {
  hex: "#b7d07a",
  name: "芦苇绿"
}, {
  hex: "#d0deaa",
  name: "姚黄"
}, {
  hex: "#373834",
  name: "蒽油绿"
}, {
  hex: "#bacf65",
  name: "苹果绿"
}, {
  hex: "#e2e7bf",
  name: "海沬绿"
}, {
  hex: "#bec936",
  name: "橄榄黄绿"
}, {
  hex: "#d2d97a",
  name: "槐花黄绿"
}, {
  hex: "#e2d849",
  name: "蝶黄"
}, {
  hex: "#fffef8",
  name: "象牙白"
}, {
  hex: "#5e5314",
  name: "橄榄绿"
}, {
  hex: "#fffef9",
  name: "雪白"
}, {
  hex: "#ad9e5f",
  name: "淡灰绿"
}, {
  hex: "#fed71a",
  name: "佛手黄"
}, {
  hex: "#f9f4dc",
  name: "乳白"
}, {
  hex: "#e4bf11",
  name: "香蕉黄"
}, {
  hex: "#d2b116",
  name: "新禾绿"
}, {
  hex: "#fbda41",
  name: "油菜花黄"
}, {
  hex: "#eed045",
  name: "秋葵黄"
}, {
  hex: "#f1ca17",
  name: "柚黄"
}, {
  hex: "#d2b42c",
  name: "草黄"
}, {
  hex: "#f2ce2b",
  name: "硫华黄"
}, {
  hex: "#e2c027",
  name: "姜黄"
}, {
  hex: "#645822",
  name: "潭水绿"
}, {
  hex: "#fcd217",
  name: "金瓜黄"
}, {
  hex: "#f8df70",
  name: "麦秆黄"
}, {
  hex: "#dfc243",
  name: "蒿黄"
}, {
  hex: "#f8df72",
  name: "茉莉黄"
}, {
  hex: "#ffd111",
  name: "藤黄"
}, {
  hex: "#ddc871",
  name: "芒果黄"
}, {
  hex: "#fffefa",
  name: "海参灰"
}, {
  hex: "#867018",
  name: "碧螺春绿"
}, {
  hex: "#887322",
  name: "苔绿"
}, {
  hex: "#fcd337",
  name: "柠檬黄"
}, {
  hex: "#8e804b",
  name: "草灰绿"
}, {
  hex: "#fecc11",
  name: "向日葵黄"
}, {
  hex: "#fccb16",
  name: "素馨黄"
}, {
  hex: "#ffc90c",
  name: "乳鸭黄"
}, {
  hex: "#b7ae8f",
  name: "月灰"
}, {
  hex: "#f8d86a",
  name: "葵扇黄"
}, {
  hex: "#fbcd31",
  name: "大豆黄"
}, {
  hex: "#fcc307",
  name: "金盏黄"
}, {
  hex: "#e9ddb6",
  name: "菊蕾白"
}, {
  hex: "#fcc515",
  name: "黄连黄"
}, {
  hex: "#f7e8aa",
  name: "杏仁黄"
}, {
  hex: "#e8b004",
  name: "谷黄"
}, {
  hex: "#f9c116",
  name: "木瓜黄"
}, {
  hex: "#f9d770",
  name: "淡茧黄"
}, {
  hex: "#fbc82f",
  name: "雅梨黄"
}, {
  hex: "#f1f0ed",
  name: "银白"
}, {
  hex: "#5b4913",
  name: "棕榈绿"
}, {
  hex: "#f6c430",
  name: "鹦鹉冠黄"
}, {
  hex: "#b78d12",
  name: "枯绿"
}, {
  hex: "#f9bd10",
  name: "浅烙黄"
}, {
  hex: "#f9d367",
  name: "淡密黄"
}, {
  hex: "#d9a40e",
  name: "芥黄"
}, {
  hex: "#ebb10d",
  name: "栀子黄"
}, {
  hex: "#584717",
  name: "暗海水绿"
}, {
  hex: "#f7de98",
  name: "篾黄"
}, {
  hex: "#f9f1db",
  name: "蚌肉白"
}, {
  hex: "#f4ce69",
  name: "炒米黄"
}, {
  hex: "#feba07",
  name: "琥珀黄"
}, {
  hex: "#8a6913",
  name: "灰绿"
}, {
  hex: "#876818",
  name: "粽叶绿"
}, {
  hex: "#b6a476",
  name: "尘灰"
}, {
  hex: "#fcb70a",
  name: "鼬黄"
}, {
  hex: "#f0d695",
  name: "象牙黄"
}, {
  hex: "#87723e",
  name: "鲛青"
}, {
  hex: "#f8e8c1",
  name: "豆汁黄"
}, {
  hex: "#d6a01d",
  name: "土黄"
}, {
  hex: "#f7da94",
  name: "香水玫瑰黄"
}, {
  hex: "#eaad1a",
  name: "虎皮黄"
}, {
  hex: "#fbb612",
  name: "鸡蛋黄"
}, {
  hex: "#b5aa90",
  name: "银鼠灰"
}, {
  hex: "#f7f4ed",
  name: "鱼肚白"
}, {
  hex: "#f8bc31",
  name: "初熟杏黄"
}, {
  hex: "#b78b26",
  name: "山鸡黄"
}, {
  hex: "#e5d3aa",
  name: "莲子白"
}, {
  hex: "#695e45",
  name: "蟹壳灰"
}, {
  hex: "#e5b751",
  name: "沙石黄"
}, {
  hex: "#f3bf4c",
  name: "甘草黄"
}, {
  hex: "#685e48",
  name: "燕羽灰"
}, {
  hex: "#fbb929",
  name: "鹅掌黄"
}, {
  hex: "#f9d27d",
  name: "麦芽糖黄"
}, {
  hex: "#e2c17c",
  name: "浅驼色"
}, {
  hex: "#b4a992",
  name: "百灵鸟灰"
}, {
  hex: "#f6dead",
  name: "酪黄"
}, {
  hex: "#f2e6ce",
  name: "荔肉白"
}, {
  hex: "#f8e0b0",
  name: "淡肉色"
}, {
  hex: "#393733",
  name: "河豚灰"
}, {
  hex: "#835e1d",
  name: "蜴蜊绿"
}, {
  hex: "#f8f4ed",
  name: "汉白玉"
}, {
  hex: "#fca104",
  name: "橙皮黄"
}, {
  hex: "#815f25",
  name: "莱阳梨黄"
}, {
  hex: "#fca106",
  name: "枇杷黄"
}, {
  hex: "#ffa60f",
  name: "金叶黄"
}, {
  hex: "#806332",
  name: "苍黄"
}, {
  hex: "#fbf2e3",
  name: "粉白"
}, {
  hex: "#fba414",
  name: "淡橘橙"
}, {
  hex: "#e4dfd7",
  name: "珍珠灰"
}, {
  hex: "#826b48",
  name: "龟背黄"
}, {
  hex: "#dad4cb",
  name: "浅灰"
}, {
  hex: "#bbb5ac",
  name: "铅灰"
}, {
  hex: "#bbb5ac",
  name: "中灰"
}, {
  hex: "#ff9900",
  name: "雄黄"
}, {
  hex: "#fbb957",
  name: "蜜黄"
}, {
  hex: "#dc9123",
  name: "风帆黄"
}, {
  hex: "#c09351",
  name: "桂皮淡棕"
}, {
  hex: "#f4a83a",
  name: "金莺黄"
}, {
  hex: "#f7c173",
  name: "肉色"
}, {
  hex: "#e7a23f",
  name: "凋叶棕"
}, {
  hex: "#533c1b",
  name: "古铜绿"
}, {
  hex: "#f9e8d0",
  name: "落英淡粉"
}, {
  hex: "#de9e44",
  name: "软木黄"
}, {
  hex: "#f9cb8b",
  name: "瓜瓤粉"
}, {
  hex: "#f9a633",
  name: "榴萼黄"
}, {
  hex: "#daa45a",
  name: "玳瑁黄"
}, {
  hex: "#553b18",
  name: "焦茶绿"
}, {
  hex: "#513c20",
  name: "蟹壳绿"
}, {
  hex: "#986524",
  name: "山鸡褐"
}, {
  hex: "#97846c",
  name: "猴毛灰"
}, {
  hex: "#e3bd8d",
  name: "鹿角棕"
}, {
  hex: "#4d4030",
  name: "淡松烟"
}, {
  hex: "#fb8b05",
  name: "万寿菊黄"
}, {
  hex: "#f8c387",
  name: "蛋壳黄"
}, {
  hex: "#f28e16",
  name: "杏黄"
}, {
  hex: "#503e2a",
  name: "橄榄灰"
}, {
  hex: "#4a4035",
  name: "鹤灰"
}, {
  hex: "#cfccc9",
  name: "玛瑙灰"
}, {
  hex: "#c1b2a3",
  name: "淡银灰"
}, {
  hex: "#867e76",
  name: "瓦灰"
}, {
  hex: "#847c74",
  name: "夜灰"
}, {
  hex: "#fc8c23",
  name: "北瓜黄"
}, {
  hex: "#fbecde",
  name: "荷花白"
}, {
  hex: "#4f4032",
  name: "松鼠灰"
}, {
  hex: "#fbeee2",
  name: "淡米粉"
}, {
  hex: "#81776e",
  name: "深灰"
}, {
  hex: "#9a8878",
  name: "海鸥灰"
}, {
  hex: "#5d3d21",
  name: "茶褐"
}, {
  hex: "#66462a",
  name: "驼色"
}, {
  hex: "#918072",
  name: "银灰"
}, {
  hex: "#d99156",
  name: "鹿皮褐"
}, {
  hex: "#c1651a",
  name: "槟榔综"
}, {
  hex: "#d4c4b7",
  name: "晓灰"
}, {
  hex: "#be7e4a",
  name: "淡赭"
}, {
  hex: "#5c3719",
  name: "古铜褐"
}, {
  hex: "#de7622",
  name: "麂棕"
}, {
  hex: "#db8540",
  name: "醉瓜肉"
}, {
  hex: "#80766e",
  name: "雁灰"
}, {
  hex: "#f09c5a",
  name: "鲑鱼红"
}, {
  hex: "#f97d1c",
  name: "橘橙"
}, {
  hex: "#f26b1f",
  name: "金黄"
}, {
  hex: "#f8b37f",
  name: "玫瑰粉"
}, {
  hex: "#fa7e23",
  name: "美人焦橙"
}, {
  hex: "#f9e9cd",
  name: "米色"
}, {
  hex: "#b7a091",
  name: "蛛网灰"
}, {
  hex: "#945833",
  name: "淡咖啡"
}, {
  hex: "#f0945d",
  name: "海螺橙"
}, {
  hex: "#964d22",
  name: "岩石棕"
}, {
  hex: "#954416",
  name: "芒果棕"
}, {
  hex: "#e16723",
  name: "陶瓷红"
}, {
  hex: "#fc7930",
  name: "菠萝红"
}, {
  hex: "#cf7543",
  name: "余烬红"
}, {
  hex: "#f86b1d",
  name: "金莲花橙"
}, {
  hex: "#cd6227",
  name: "火砖红"
}, {
  hex: "#f6dcce",
  name: "初桃粉红"
}, {
  hex: "#d85916",
  name: "铁棕"
}, {
  hex: "#f7cfba",
  name: "介壳淡粉红"
}, {
  hex: "#f27635",
  name: "蟹壳红"
}, {
  hex: "#e46828",
  name: "金驼"
}, {
  hex: "#fc6315",
  name: "燕颔红"
}, {
  hex: "#b7511d",
  name: "淡可可棕"
}, {
  hex: "#ea8958",
  name: "晨曦红"
}, {
  hex: "#e8b49a",
  name: "玉粉红"
}, {
  hex: "#fb9968",
  name: "野蔷薇红"
}, {
  hex: "#edc3ae",
  name: "藕荷"
}, {
  hex: "#363433",
  name: "长石灰"
}, {
  hex: "#8b614d",
  name: "中红灰"
}, {
  hex: "#aa6a4c",
  name: "火泥棕"
}, {
  hex: "#a6522c",
  name: "绀红"
}, {
  hex: "#fa5d19",
  name: "莓酱红"
}, {
  hex: "#71361d",
  name: "丁香棕"
}, {
  hex: "#b89485",
  name: "淡玫瑰灰"
}, {
  hex: "#f68c60",
  name: "瓜瓤红"
}, {
  hex: "#f6ad8f",
  name: "淡藏花红"
}, {
  hex: "#732e12",
  name: "筍皮棕"
}, {
  hex: "#f7cdbc",
  name: "润红"
}, {
  hex: "#ef632b",
  name: "龙睛鱼红"
}, {
  hex: "#8c4b31",
  name: "淡土黄"
}, {
  hex: "#64483d",
  name: "珠母灰"
}, {
  hex: "#f9723d",
  name: "芙蓉红"
}, {
  hex: "#cf4813",
  name: "落霞红"
}, {
  hex: "#ee8055",
  name: "法螺红"
}, {
  hex: "#f8ebe6",
  name: "草珠红"
}, {
  hex: "#753117",
  name: "咖啡"
}, {
  hex: "#603d30",
  name: "中灰驼"
}, {
  hex: "#883a1e",
  name: "椰壳棕"
}, {
  hex: "#b14b28",
  name: "蟹蝥红"
}, {
  hex: "#873d24",
  name: "淡豆沙"
}, {
  hex: "#f6cec1",
  name: "淡桃红"
}, {
  hex: "#5b423a",
  name: "淡铁灰"
}, {
  hex: "#624941",
  name: "石板灰"
}, {
  hex: "#673424",
  name: "淡栗棕"
}, {
  hex: "#f43e06",
  name: "银朱"
}, {
  hex: "#ef6f48",
  name: "草莓红"
}, {
  hex: "#f4c7ba",
  name: "洋水仙红"
}, {
  hex: "#ed5126",
  name: "朱红"
}, {
  hex: "#f34718",
  name: "榴花红"
}, {
  hex: "#f2481b",
  name: "柿红"
}, {
  hex: "#652b1c",
  name: "可可棕"
}, {
  hex: "#eea08c",
  name: "淡罂粟红"
}, {
  hex: "#f04b22",
  name: "大红"
}, {
  hex: "#692a1b",
  name: "柞叶棕"
}, {
  hex: "#f1441d",
  name: "蜻蜓红"
}, {
  hex: "#773d31",
  name: "橡树棕"
}, {
  hex: "#eeaa9c",
  name: "颊红"
}, {
  hex: "#f0ada0",
  name: "桃红"
}, {
  hex: "#863020",
  name: "火岩棕"
}, {
  hex: "#f2e7e5",
  name: "淡藤萝紫"
}, {
  hex: "#862617",
  name: "赭石"
}, {
  hex: "#f5391c",
  name: "铁水红"
}, {
  hex: "#f03f24",
  name: "胭脂红"
}, {
  hex: "#f33b1f",
  name: "极光红"
}, {
  hex: "#f23e23",
  name: "红汞红"
}, {
  hex: "#f13c22",
  name: "萝卜红"
}, {
  hex: "#f05a46",
  name: "曲红"
}, {
  hex: "#f17666",
  name: "谷鞘红"
}, {
  hex: "#f15642",
  name: "苹果红"
}, {
  hex: "#f25a47",
  name: "桂红"
}, {
  hex: "#f2b9b2",
  name: "粉红"
}, {
  hex: "#592620",
  name: "暗驼棕"
}, {
  hex: "#de2a18",
  name: "夕阳红"
}, {
  hex: "#ed3321",
  name: "樱桃红"
}, {
  hex: "#f04a3a",
  name: "珊瑚红"
}, {
  hex: "#482522",
  name: "火山棕"
}, {
  hex: "#5c1e19",
  name: "栗棕"
}, {
  hex: "#d42517",
  name: "鹤顶红"
}, {
  hex: "#f19790",
  name: "舌红"
}, {
  hex: "#ab372f",
  name: "鹅血石红"
}, {
  hex: "#5a1f1b",
  name: "酱棕"
}, {
  hex: "#ed3b2f",
  name: "鱼鳃红"
}, {
  hex: "#bdaead",
  name: "芦穗灰"
}, {
  hex: "#eb261a",
  name: "丽春红"
}, {
  hex: "#ac1f18",
  name: "覆盆子红"
}, {
  hex: "#483332",
  name: "海报灰"
}, {
  hex: "#481e1c",
  name: "豆沙"
}, {
  hex: "#f1908c",
  name: "榴子红"
}, {
  hex: "#ec2b24",
  name: "秋海棠红"
}, {
  hex: "#efafad",
  name: "无花果红"
}, {
  hex: "#f2cac9",
  name: "淡绯"
}, {
  hex: "#4b2e2b",
  name: "玫瑰灰"
}, {
  hex: "#ed4845",
  name: "淡菽红"
}, {
  hex: "#ed3333",
  name: "枸枢红"
}, {
  hex: "#5d3131",
  name: "貂紫"
}];

/*
    ## Color

    http://llllll.li/randomColor/
        A color generator for JavaScript.
        randomColor generates attractive colors by default. More specifically, randomColor produces bright colors with a reasonably high saturation. This makes randomColor particularly useful for data visualizations and generative art.

    http://randomcolour.com/
        var bg_colour = Math.floor(Math.random() * 16777215).toString(16);
        bg_colour = "#" + ("000000" + bg_colour).slice(-6);
        document.bgColor = bg_colour;

    http://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/
        Creating random colors is actually more difficult than it seems. The randomness itself is easy, but aesthetically pleasing randomness is more difficult.
        https://github.com/devongovett/color-generator

    http://www.paulirish.com/2009/random-hex-color-code-snippets/
        Random Hex Color Code Generator in JavaScript

    http://chancejs.com/#color
        chance.color()
        // => '#79c157'
        chance.color({format: 'hex'})
        // => '#d67118'
        chance.color({format: 'shorthex'})
        // => '#60f'
        chance.color({format: 'rgb'})
        // => 'rgb(110,52,164)'

    http://tool.c7sky.com/webcolor
        网页设计常用色彩搭配表

    https://github.com/One-com/one-color
        An OO-based JavaScript color parser/computation toolkit with support for RGB, HSV, HSL, CMYK, and alpha channels.
        API 很赞

    https://github.com/harthur/color
        JavaScript color conversion and manipulation library

    https://github.com/leaverou/css-colors
        Share & convert CSS colors
    http://leaverou.github.io/css-colors/#slategray
        Type a CSS color keyword, #hex, hsl(), rgba(), whatever:

    色调 hue
        http://baike.baidu.com/view/23368.htm
        色调指的是一幅画中画面色彩的总体倾向，是大的色彩效果。
    饱和度 saturation
        http://baike.baidu.com/view/189644.htm
        饱和度是指色彩的鲜艳程度，也称色彩的纯度。饱和度取决于该色中含色成分和消色成分（灰色）的比例。含色成分越大，饱和度越大；消色成分越大，饱和度越小。
    亮度 brightness
        http://baike.baidu.com/view/34773.htm
        亮度是指发光体（反光体）表面发光（反光）强弱的物理量。
    照度 luminosity
        物体被照亮的程度,采用单位面积所接受的光通量来表示,表示单位为勒[克斯](Lux,lx) ,即 1m / m2 。

    http://stackoverflow.com/questions/1484506/random-color-generator-in-javascript
        var letters = '0123456789ABCDEF'.split('')
        var color = '#'
        for (var i = 0; i < 6; i++) {
            color += letters[Math.floor(Math.random() * 16)]
        }
        return color

        // 随机生成一个无脑的颜色，格式为 '#RRGGBB'。
        // _brainlessColor()
        var color = Math.floor(
            Math.random() *
            (16 * 16 * 16 * 16 * 16 * 16 - 1)
        ).toString(16)
        color = "#" + ("000000" + color).slice(-6)
        return color.toUpperCase()
*/
var _hue = 0;
function _goldenRatioColor() {
  var saturation = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : 0.5;
  var value = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 0.95;
  var _goldenRatio = 0.618033988749895;
  _hue = _hue || Math.random();
  _hue += _goldenRatio;
  _hue %= 1;
  return chroma.hsv(_hue * 360, saturation * 100, value * 100);
} // 使用随机中国色代替随机 数据来自于 http://zhongguose.com
// 随机生成一个有吸引力的颜色，格式为 '#RRGGBB' hex。

function color(name) {
  if (name && name in DICT$1) return DICT$1[name].nicer;
  return sample(RandomColor).hex;
} // #DAC0DE

function hex() {
  return _goldenRatioColor().hex();
} // rgb(128,255,255)

function rgb() {
  var hsv = _goldenRatioColor();

  return hsv.css();
} // rgba(128,255,255,0.3)

function rgba() {
  var hsv = _goldenRatioColor();

  return hsv.alpha(random(0, 1, true)).css();
} // hsl(300,0.8,0.9)

function hsl() {
  var hsv = _goldenRatioColor();

  return hsv.css();
}

var cfirstName$1 = ["王", "李", "张", "刘", "陈", "杨", "赵", "黄", "周", "吴", "徐", "孙", "胡", "朱", "高", "林", "何", "郭", "马", "罗", "梁", "宋", "郑", "谢", "韩", "唐", "冯", "于", "董", "萧", "程", "曹", "袁", "邓", "许", "傅", "沈", "曾", "彭", "吕", "苏", "卢", "蒋", "蔡", "贾", "丁", "魏", "薛", "叶", "阎", "余", "潘", "杜", "戴", "夏", "锺", "汪", "田", "任", "姜", "范", "方", "石", "姚", "谭", "廖", "邹", "熊", "金", "陆", "郝", "孔", "白", "崔", "康", "毛", "邱", "秦", "江", "史", "顾", "侯", "邵", "孟", "龙", "万", "段", "雷", "钱", "汤", "尹", "黎", "易", "常", "武", "乔", "贺", "赖", "龚", "文"];
var clastName$1 = ["伟", "芳", "娜", "秀英", "敏", "静", "丽", "强", "磊", "军", "洋", "勇", "艳", "杰", "娟", "涛", "明", "超", "秀兰", "霞", "平", "刚", "桂英"];
var Name = {
  cfirstName: cfirstName$1,
  clastName: clastName$1
};

/*
    ## Name

    [Beyond the Top 1000 Names](http://www.ssa.gov/oact/babynames/limits.html)
*/
var cfirstName = Name.cfirstName,
    clastName = Name.clastName;

function first() {
  return faker.name.firstName();
} // 随机生成一个常见的英文姓。


function last() {
  return faker.name.lastName();
} // 随机生成一个常见的英文姓名。


var name = decode(function (middle) {
  var mid = middle ? " ".concat(first(), " ") : ' ';
  return "".concat(first(), " ").concat(mid, " ").concat(last());
}, ['boolean']);
/*
    随机生成一个常见的中文姓。
    [世界常用姓氏排行](http://baike.baidu.com/view/1719115.htm)
    [玄派网 - 网络小说创作辅助平台](http://xuanpai.sinaapp.com/)
*/

function cfirst() {
  return sample(cfirstName);
}
/*
    随机生成一个常见的中文名。
    [中国最常见名字前50名_三九算命网](http://www.name999.net/xingming/xingshi/20131004/48.html)
*/


function clast() {
  return sample(clastName);
} // 随机生成一个常见的中文姓名。


function cname() {
  return cfirst() + clast();
}

// Unique ID creation requires a high quality random # generator. In the browser we therefore
// require the crypto API and do not support built-in fallback to lower quality random number
// generators (like Math.random()).
var getRandomValues;
var rnds8 = new Uint8Array(16);
function rng() {
  // lazy load so that environments that need to polyfill have a chance to do so
  if (!getRandomValues) {
    // getRandomValues needs to be invoked in a context where "this" is a Crypto implementation. Also,
    // find the complete implementation of crypto (msCrypto) on IE11.
    getRandomValues = typeof crypto !== 'undefined' && crypto.getRandomValues && crypto.getRandomValues.bind(crypto) || typeof msCrypto !== 'undefined' && typeof msCrypto.getRandomValues === 'function' && msCrypto.getRandomValues.bind(msCrypto);

    if (!getRandomValues) {
      throw new Error('crypto.getRandomValues() not supported. See https://github.com/uuidjs/uuid#getrandomvalues-not-supported');
    }
  }

  return getRandomValues(rnds8);
}

var REGEX = /^(?:[0-9a-f]{8}-[0-9a-f]{4}-[1-5][0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}|00000000-0000-0000-0000-000000000000)$/i;

function validate(uuid) {
  return typeof uuid === 'string' && REGEX.test(uuid);
}

/**
 * Convert array of 16 byte values to UUID string format of the form:
 * XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
 */

var byteToHex = [];

for (var i = 0; i < 256; ++i) {
  byteToHex.push((i + 0x100).toString(16).substr(1));
}

function stringify(arr) {
  var offset = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 0;
  // Note: Be careful editing this code!  It's been tuned for performance
  // and works in ways you may not expect. See https://github.com/uuidjs/uuid/pull/434
  var uuid = (byteToHex[arr[offset + 0]] + byteToHex[arr[offset + 1]] + byteToHex[arr[offset + 2]] + byteToHex[arr[offset + 3]] + '-' + byteToHex[arr[offset + 4]] + byteToHex[arr[offset + 5]] + '-' + byteToHex[arr[offset + 6]] + byteToHex[arr[offset + 7]] + '-' + byteToHex[arr[offset + 8]] + byteToHex[arr[offset + 9]] + '-' + byteToHex[arr[offset + 10]] + byteToHex[arr[offset + 11]] + byteToHex[arr[offset + 12]] + byteToHex[arr[offset + 13]] + byteToHex[arr[offset + 14]] + byteToHex[arr[offset + 15]]).toLowerCase(); // Consistency check for valid UUID.  If this throws, it's likely due to one
  // of the following:
  // - One or more input array values don't map to a hex octet (leading to
  // "undefined" in the uuid)
  // - Invalid input values for the RFC `version` or `variant` fields

  if (!validate(uuid)) {
    throw TypeError('Stringified UUID is invalid');
  }

  return uuid;
}

function v4(options, buf, offset) {
  options = options || {};
  var rnds = options.random || (options.rng || rng)(); // Per 4.4, set bits for version and `clock_seq_hi_and_reserved`

  rnds[6] = rnds[6] & 0x0f | 0x40;
  rnds[8] = rnds[8] & 0x3f | 0x80; // Copy bytes to buffer, if provided

  if (buf) {
    offset = offset || 0;

    for (var i = 0; i < 16; ++i) {
      buf[offset + i] = rnds[i];
    }

    return buf;
  }

  return stringify(rnds);
}

var DICT = [{
  id: "110000",
  pid: "110000",
  name: "北京",
  children: [{
    id: "110100",
    pid: "110000",
    name: "北京市",
    children: [{
      id: "110101",
      pid: "110100",
      name: "东城区"
    }, {
      id: "110102",
      pid: "110100",
      name: "西城区"
    }, {
      id: "110105",
      pid: "110100",
      name: "朝阳区"
    }, {
      id: "110106",
      pid: "110100",
      name: "丰台区"
    }, {
      id: "110107",
      pid: "110100",
      name: "石景山区"
    }, {
      id: "110108",
      pid: "110100",
      name: "海淀区"
    }, {
      id: "110109",
      pid: "110100",
      name: "门头沟区"
    }, {
      id: "110111",
      pid: "110100",
      name: "房山区"
    }, {
      id: "110112",
      pid: "110100",
      name: "通州区"
    }, {
      id: "110113",
      pid: "110100",
      name: "顺义区"
    }, {
      id: "110114",
      pid: "110100",
      name: "昌平区"
    }, {
      id: "110115",
      pid: "110100",
      name: "大兴区"
    }, {
      id: "110116",
      pid: "110100",
      name: "怀柔区"
    }, {
      id: "110117",
      pid: "110100",
      name: "平谷区"
    }]
  }, {
    id: "110228",
    pid: "110000",
    name: "密云县"
  }, {
    id: "110229",
    pid: "110000",
    name: "延庆县"
  }, {
    id: "110230",
    pid: "110000",
    name: "其它区"
  }]
}, {
  id: "120000",
  pid: "120000",
  name: "天津",
  children: [{
    id: "120100",
    pid: "120000",
    name: "天津市",
    children: [{
      id: "120101",
      pid: "120100",
      name: "和平区"
    }, {
      id: "120102",
      pid: "120100",
      name: "河东区"
    }, {
      id: "120103",
      pid: "120100",
      name: "河西区"
    }, {
      id: "120104",
      pid: "120100",
      name: "南开区"
    }, {
      id: "120105",
      pid: "120100",
      name: "河北区"
    }, {
      id: "120106",
      pid: "120100",
      name: "红桥区"
    }, {
      id: "120110",
      pid: "120100",
      name: "东丽区"
    }, {
      id: "120111",
      pid: "120100",
      name: "西青区"
    }, {
      id: "120112",
      pid: "120100",
      name: "津南区"
    }, {
      id: "120113",
      pid: "120100",
      name: "北辰区"
    }, {
      id: "120114",
      pid: "120100",
      name: "武清区"
    }, {
      id: "120115",
      pid: "120100",
      name: "宝坻区"
    }, {
      id: "120116",
      pid: "120100",
      name: "滨海新区"
    }]
  }, {
    id: "120221",
    pid: "120000",
    name: "宁河县"
  }, {
    id: "120223",
    pid: "120000",
    name: "静海县"
  }, {
    id: "120225",
    pid: "120000",
    name: "蓟县"
  }, {
    id: "120226",
    pid: "120000",
    name: "其它区"
  }]
}, {
  id: "130000",
  pid: "130000",
  name: "河北省",
  children: [{
    id: "130100",
    pid: "130000",
    name: "石家庄市",
    children: [{
      id: "130102",
      pid: "130100",
      name: "长安区"
    }, {
      id: "130103",
      pid: "130100",
      name: "桥东区"
    }, {
      id: "130104",
      pid: "130100",
      name: "桥西区"
    }, {
      id: "130105",
      pid: "130100",
      name: "新华区"
    }, {
      id: "130107",
      pid: "130100",
      name: "井陉矿区"
    }, {
      id: "130108",
      pid: "130100",
      name: "裕华区"
    }, {
      id: "130121",
      pid: "130100",
      name: "井陉县"
    }, {
      id: "130123",
      pid: "130100",
      name: "正定县"
    }, {
      id: "130124",
      pid: "130100",
      name: "栾城县"
    }, {
      id: "130125",
      pid: "130100",
      name: "行唐县"
    }, {
      id: "130126",
      pid: "130100",
      name: "灵寿县"
    }, {
      id: "130127",
      pid: "130100",
      name: "高邑县"
    }, {
      id: "130128",
      pid: "130100",
      name: "深泽县"
    }, {
      id: "130129",
      pid: "130100",
      name: "赞皇县"
    }, {
      id: "130130",
      pid: "130100",
      name: "无极县"
    }, {
      id: "130131",
      pid: "130100",
      name: "平山县"
    }, {
      id: "130132",
      pid: "130100",
      name: "元氏县"
    }, {
      id: "130133",
      pid: "130100",
      name: "赵县"
    }, {
      id: "130181",
      pid: "130100",
      name: "辛集市"
    }, {
      id: "130182",
      pid: "130100",
      name: "藁城市"
    }, {
      id: "130183",
      pid: "130100",
      name: "晋州市"
    }, {
      id: "130184",
      pid: "130100",
      name: "新乐市"
    }, {
      id: "130185",
      pid: "130100",
      name: "鹿泉市"
    }, {
      id: "130186",
      pid: "130100",
      name: "其它区"
    }]
  }, {
    id: "130200",
    pid: "130000",
    name: "唐山市",
    children: [{
      id: "130202",
      pid: "130200",
      name: "路南区"
    }, {
      id: "130203",
      pid: "130200",
      name: "路北区"
    }, {
      id: "130204",
      pid: "130200",
      name: "古冶区"
    }, {
      id: "130205",
      pid: "130200",
      name: "开平区"
    }, {
      id: "130207",
      pid: "130200",
      name: "丰南区"
    }, {
      id: "130208",
      pid: "130200",
      name: "丰润区"
    }, {
      id: "130223",
      pid: "130200",
      name: "滦县"
    }, {
      id: "130224",
      pid: "130200",
      name: "滦南县"
    }, {
      id: "130225",
      pid: "130200",
      name: "乐亭县"
    }, {
      id: "130227",
      pid: "130200",
      name: "迁西县"
    }, {
      id: "130229",
      pid: "130200",
      name: "玉田县"
    }, {
      id: "130230",
      pid: "130200",
      name: "曹妃甸区"
    }, {
      id: "130281",
      pid: "130200",
      name: "遵化市"
    }, {
      id: "130283",
      pid: "130200",
      name: "迁安市"
    }, {
      id: "130284",
      pid: "130200",
      name: "其它区"
    }]
  }, {
    id: "130300",
    pid: "130000",
    name: "秦皇岛市",
    children: [{
      id: "130302",
      pid: "130300",
      name: "海港区"
    }, {
      id: "130303",
      pid: "130300",
      name: "山海关区"
    }, {
      id: "130304",
      pid: "130300",
      name: "北戴河区"
    }, {
      id: "130321",
      pid: "130300",
      name: "青龙满族自治县"
    }, {
      id: "130322",
      pid: "130300",
      name: "昌黎县"
    }, {
      id: "130323",
      pid: "130300",
      name: "抚宁县"
    }, {
      id: "130324",
      pid: "130300",
      name: "卢龙县"
    }, {
      id: "130398",
      pid: "130300",
      name: "其它区"
    }]
  }, {
    id: "130400",
    pid: "130000",
    name: "邯郸市",
    children: [{
      id: "130402",
      pid: "130400",
      name: "邯山区"
    }, {
      id: "130403",
      pid: "130400",
      name: "丛台区"
    }, {
      id: "130404",
      pid: "130400",
      name: "复兴区"
    }, {
      id: "130406",
      pid: "130400",
      name: "峰峰矿区"
    }, {
      id: "130421",
      pid: "130400",
      name: "邯郸县"
    }, {
      id: "130423",
      pid: "130400",
      name: "临漳县"
    }, {
      id: "130424",
      pid: "130400",
      name: "成安县"
    }, {
      id: "130425",
      pid: "130400",
      name: "大名县"
    }, {
      id: "130426",
      pid: "130400",
      name: "涉县"
    }, {
      id: "130427",
      pid: "130400",
      name: "磁县"
    }, {
      id: "130428",
      pid: "130400",
      name: "肥乡县"
    }, {
      id: "130429",
      pid: "130400",
      name: "永年县"
    }, {
      id: "130430",
      pid: "130400",
      name: "邱县"
    }, {
      id: "130431",
      pid: "130400",
      name: "鸡泽县"
    }, {
      id: "130432",
      pid: "130400",
      name: "广平县"
    }, {
      id: "130433",
      pid: "130400",
      name: "馆陶县"
    }, {
      id: "130434",
      pid: "130400",
      name: "魏县"
    }, {
      id: "130435",
      pid: "130400",
      name: "曲周县"
    }, {
      id: "130481",
      pid: "130400",
      name: "武安市"
    }, {
      id: "130482",
      pid: "130400",
      name: "其它区"
    }]
  }, {
    id: "130500",
    pid: "130000",
    name: "邢台市",
    children: [{
      id: "130502",
      pid: "130500",
      name: "桥东区"
    }, {
      id: "130503",
      pid: "130500",
      name: "桥西区"
    }, {
      id: "130521",
      pid: "130500",
      name: "邢台县"
    }, {
      id: "130522",
      pid: "130500",
      name: "临城县"
    }, {
      id: "130523",
      pid: "130500",
      name: "内丘县"
    }, {
      id: "130524",
      pid: "130500",
      name: "柏乡县"
    }, {
      id: "130525",
      pid: "130500",
      name: "隆尧县"
    }, {
      id: "130526",
      pid: "130500",
      name: "任县"
    }, {
      id: "130527",
      pid: "130500",
      name: "南和县"
    }, {
      id: "130528",
      pid: "130500",
      name: "宁晋县"
    }, {
      id: "130529",
      pid: "130500",
      name: "巨鹿县"
    }, {
      id: "130530",
      pid: "130500",
      name: "新河县"
    }, {
      id: "130531",
      pid: "130500",
      name: "广宗县"
    }, {
      id: "130532",
      pid: "130500",
      name: "平乡县"
    }, {
      id: "130533",
      pid: "130500",
      name: "威县"
    }, {
      id: "130534",
      pid: "130500",
      name: "清河县"
    }, {
      id: "130535",
      pid: "130500",
      name: "临西县"
    }, {
      id: "130581",
      pid: "130500",
      name: "南宫市"
    }, {
      id: "130582",
      pid: "130500",
      name: "沙河市"
    }, {
      id: "130583",
      pid: "130500",
      name: "其它区"
    }]
  }, {
    id: "130600",
    pid: "130000",
    name: "保定市",
    children: [{
      id: "130602",
      pid: "130600",
      name: "新市区"
    }, {
      id: "130603",
      pid: "130600",
      name: "北市区"
    }, {
      id: "130604",
      pid: "130600",
      name: "南市区"
    }, {
      id: "130621",
      pid: "130600",
      name: "满城县"
    }, {
      id: "130622",
      pid: "130600",
      name: "清苑县"
    }, {
      id: "130623",
      pid: "130600",
      name: "涞水县"
    }, {
      id: "130624",
      pid: "130600",
      name: "阜平县"
    }, {
      id: "130625",
      pid: "130600",
      name: "徐水县"
    }, {
      id: "130626",
      pid: "130600",
      name: "定兴县"
    }, {
      id: "130627",
      pid: "130600",
      name: "唐县"
    }, {
      id: "130628",
      pid: "130600",
      name: "高阳县"
    }, {
      id: "130629",
      pid: "130600",
      name: "容城县"
    }, {
      id: "130630",
      pid: "130600",
      name: "涞源县"
    }, {
      id: "130631",
      pid: "130600",
      name: "望都县"
    }, {
      id: "130632",
      pid: "130600",
      name: "安新县"
    }, {
      id: "130633",
      pid: "130600",
      name: "易县"
    }, {
      id: "130634",
      pid: "130600",
      name: "曲阳县"
    }, {
      id: "130635",
      pid: "130600",
      name: "蠡县"
    }, {
      id: "130636",
      pid: "130600",
      name: "顺平县"
    }, {
      id: "130637",
      pid: "130600",
      name: "博野县"
    }, {
      id: "130638",
      pid: "130600",
      name: "雄县"
    }, {
      id: "130681",
      pid: "130600",
      name: "涿州市"
    }, {
      id: "130682",
      pid: "130600",
      name: "定州市"
    }, {
      id: "130683",
      pid: "130600",
      name: "安国市"
    }, {
      id: "130684",
      pid: "130600",
      name: "高碑店市"
    }, {
      id: "130699",
      pid: "130600",
      name: "其它区"
    }]
  }, {
    id: "130700",
    pid: "130000",
    name: "张家口市",
    children: [{
      id: "130702",
      pid: "130700",
      name: "桥东区"
    }, {
      id: "130703",
      pid: "130700",
      name: "桥西区"
    }, {
      id: "130705",
      pid: "130700",
      name: "宣化区"
    }, {
      id: "130706",
      pid: "130700",
      name: "下花园区"
    }, {
      id: "130721",
      pid: "130700",
      name: "宣化县"
    }, {
      id: "130722",
      pid: "130700",
      name: "张北县"
    }, {
      id: "130723",
      pid: "130700",
      name: "康保县"
    }, {
      id: "130724",
      pid: "130700",
      name: "沽源县"
    }, {
      id: "130725",
      pid: "130700",
      name: "尚义县"
    }, {
      id: "130726",
      pid: "130700",
      name: "蔚县"
    }, {
      id: "130727",
      pid: "130700",
      name: "阳原县"
    }, {
      id: "130728",
      pid: "130700",
      name: "怀安县"
    }, {
      id: "130729",
      pid: "130700",
      name: "万全县"
    }, {
      id: "130730",
      pid: "130700",
      name: "怀来县"
    }, {
      id: "130731",
      pid: "130700",
      name: "涿鹿县"
    }, {
      id: "130732",
      pid: "130700",
      name: "赤城县"
    }, {
      id: "130733",
      pid: "130700",
      name: "崇礼县"
    }, {
      id: "130734",
      pid: "130700",
      name: "其它区"
    }]
  }, {
    id: "130800",
    pid: "130000",
    name: "承德市",
    children: [{
      id: "130802",
      pid: "130800",
      name: "双桥区"
    }, {
      id: "130803",
      pid: "130800",
      name: "双滦区"
    }, {
      id: "130804",
      pid: "130800",
      name: "鹰手营子矿区"
    }, {
      id: "130821",
      pid: "130800",
      name: "承德县"
    }, {
      id: "130822",
      pid: "130800",
      name: "兴隆县"
    }, {
      id: "130823",
      pid: "130800",
      name: "平泉县"
    }, {
      id: "130824",
      pid: "130800",
      name: "滦平县"
    }, {
      id: "130825",
      pid: "130800",
      name: "隆化县"
    }, {
      id: "130826",
      pid: "130800",
      name: "丰宁满族自治县"
    }, {
      id: "130827",
      pid: "130800",
      name: "宽城满族自治县"
    }, {
      id: "130828",
      pid: "130800",
      name: "围场满族蒙古族自治县"
    }, {
      id: "130829",
      pid: "130800",
      name: "其它区"
    }]
  }, {
    id: "130900",
    pid: "130000",
    name: "沧州市",
    children: [{
      id: "130902",
      pid: "130900",
      name: "新华区"
    }, {
      id: "130903",
      pid: "130900",
      name: "运河区"
    }, {
      id: "130921",
      pid: "130900",
      name: "沧县"
    }, {
      id: "130922",
      pid: "130900",
      name: "青县"
    }, {
      id: "130923",
      pid: "130900",
      name: "东光县"
    }, {
      id: "130924",
      pid: "130900",
      name: "海兴县"
    }, {
      id: "130925",
      pid: "130900",
      name: "盐山县"
    }, {
      id: "130926",
      pid: "130900",
      name: "肃宁县"
    }, {
      id: "130927",
      pid: "130900",
      name: "南皮县"
    }, {
      id: "130928",
      pid: "130900",
      name: "吴桥县"
    }, {
      id: "130929",
      pid: "130900",
      name: "献县"
    }, {
      id: "130930",
      pid: "130900",
      name: "孟村回族自治县"
    }, {
      id: "130981",
      pid: "130900",
      name: "泊头市"
    }, {
      id: "130982",
      pid: "130900",
      name: "任丘市"
    }, {
      id: "130983",
      pid: "130900",
      name: "黄骅市"
    }, {
      id: "130984",
      pid: "130900",
      name: "河间市"
    }, {
      id: "130985",
      pid: "130900",
      name: "其它区"
    }]
  }, {
    id: "131000",
    pid: "130000",
    name: "廊坊市",
    children: [{
      id: "131002",
      pid: "131000",
      name: "安次区"
    }, {
      id: "131003",
      pid: "131000",
      name: "广阳区"
    }, {
      id: "131022",
      pid: "131000",
      name: "固安县"
    }, {
      id: "131023",
      pid: "131000",
      name: "永清县"
    }, {
      id: "131024",
      pid: "131000",
      name: "香河县"
    }, {
      id: "131025",
      pid: "131000",
      name: "大城县"
    }, {
      id: "131026",
      pid: "131000",
      name: "文安县"
    }, {
      id: "131028",
      pid: "131000",
      name: "大厂回族自治县"
    }, {
      id: "131081",
      pid: "131000",
      name: "霸州市"
    }, {
      id: "131082",
      pid: "131000",
      name: "三河市"
    }, {
      id: "131083",
      pid: "131000",
      name: "其它区"
    }]
  }, {
    id: "131100",
    pid: "130000",
    name: "衡水市",
    children: [{
      id: "131102",
      pid: "131100",
      name: "桃城区"
    }, {
      id: "131121",
      pid: "131100",
      name: "枣强县"
    }, {
      id: "131122",
      pid: "131100",
      name: "武邑县"
    }, {
      id: "131123",
      pid: "131100",
      name: "武强县"
    }, {
      id: "131124",
      pid: "131100",
      name: "饶阳县"
    }, {
      id: "131125",
      pid: "131100",
      name: "安平县"
    }, {
      id: "131126",
      pid: "131100",
      name: "故城县"
    }, {
      id: "131127",
      pid: "131100",
      name: "景县"
    }, {
      id: "131128",
      pid: "131100",
      name: "阜城县"
    }, {
      id: "131181",
      pid: "131100",
      name: "冀州市"
    }, {
      id: "131182",
      pid: "131100",
      name: "深州市"
    }, {
      id: "131183",
      pid: "131100",
      name: "其它区"
    }]
  }]
}, {
  id: "140000",
  pid: "140000",
  name: "山西省",
  children: [{
    id: "140100",
    pid: "140000",
    name: "太原市",
    children: [{
      id: "140105",
      pid: "140100",
      name: "小店区"
    }, {
      id: "140106",
      pid: "140100",
      name: "迎泽区"
    }, {
      id: "140107",
      pid: "140100",
      name: "杏花岭区"
    }, {
      id: "140108",
      pid: "140100",
      name: "尖草坪区"
    }, {
      id: "140109",
      pid: "140100",
      name: "万柏林区"
    }, {
      id: "140110",
      pid: "140100",
      name: "晋源区"
    }, {
      id: "140121",
      pid: "140100",
      name: "清徐县"
    }, {
      id: "140122",
      pid: "140100",
      name: "阳曲县"
    }, {
      id: "140123",
      pid: "140100",
      name: "娄烦县"
    }, {
      id: "140181",
      pid: "140100",
      name: "古交市"
    }, {
      id: "140182",
      pid: "140100",
      name: "其它区"
    }]
  }, {
    id: "140200",
    pid: "140000",
    name: "大同市",
    children: [{
      id: "140202",
      pid: "140200",
      name: "城区"
    }, {
      id: "140203",
      pid: "140200",
      name: "矿区"
    }, {
      id: "140211",
      pid: "140200",
      name: "南郊区"
    }, {
      id: "140212",
      pid: "140200",
      name: "新荣区"
    }, {
      id: "140221",
      pid: "140200",
      name: "阳高县"
    }, {
      id: "140222",
      pid: "140200",
      name: "天镇县"
    }, {
      id: "140223",
      pid: "140200",
      name: "广灵县"
    }, {
      id: "140224",
      pid: "140200",
      name: "灵丘县"
    }, {
      id: "140225",
      pid: "140200",
      name: "浑源县"
    }, {
      id: "140226",
      pid: "140200",
      name: "左云县"
    }, {
      id: "140227",
      pid: "140200",
      name: "大同县"
    }, {
      id: "140228",
      pid: "140200",
      name: "其它区"
    }]
  }, {
    id: "140300",
    pid: "140000",
    name: "阳泉市",
    children: [{
      id: "140302",
      pid: "140300",
      name: "城区"
    }, {
      id: "140303",
      pid: "140300",
      name: "矿区"
    }, {
      id: "140311",
      pid: "140300",
      name: "郊区"
    }, {
      id: "140321",
      pid: "140300",
      name: "平定县"
    }, {
      id: "140322",
      pid: "140300",
      name: "盂县"
    }, {
      id: "140323",
      pid: "140300",
      name: "其它区"
    }]
  }, {
    id: "140400",
    pid: "140000",
    name: "长治市",
    children: [{
      id: "140421",
      pid: "140400",
      name: "长治县"
    }, {
      id: "140423",
      pid: "140400",
      name: "襄垣县"
    }, {
      id: "140424",
      pid: "140400",
      name: "屯留县"
    }, {
      id: "140425",
      pid: "140400",
      name: "平顺县"
    }, {
      id: "140426",
      pid: "140400",
      name: "黎城县"
    }, {
      id: "140427",
      pid: "140400",
      name: "壶关县"
    }, {
      id: "140428",
      pid: "140400",
      name: "长子县"
    }, {
      id: "140429",
      pid: "140400",
      name: "武乡县"
    }, {
      id: "140430",
      pid: "140400",
      name: "沁县"
    }, {
      id: "140431",
      pid: "140400",
      name: "沁源县"
    }, {
      id: "140481",
      pid: "140400",
      name: "潞城市"
    }, {
      id: "140482",
      pid: "140400",
      name: "城区"
    }, {
      id: "140483",
      pid: "140400",
      name: "郊区"
    }, {
      id: "140485",
      pid: "140400",
      name: "其它区"
    }]
  }, {
    id: "140500",
    pid: "140000",
    name: "晋城市",
    children: [{
      id: "140502",
      pid: "140500",
      name: "城区"
    }, {
      id: "140521",
      pid: "140500",
      name: "沁水县"
    }, {
      id: "140522",
      pid: "140500",
      name: "阳城县"
    }, {
      id: "140524",
      pid: "140500",
      name: "陵川县"
    }, {
      id: "140525",
      pid: "140500",
      name: "泽州县"
    }, {
      id: "140581",
      pid: "140500",
      name: "高平市"
    }, {
      id: "140582",
      pid: "140500",
      name: "其它区"
    }]
  }, {
    id: "140600",
    pid: "140000",
    name: "朔州市",
    children: [{
      id: "140602",
      pid: "140600",
      name: "朔城区"
    }, {
      id: "140603",
      pid: "140600",
      name: "平鲁区"
    }, {
      id: "140621",
      pid: "140600",
      name: "山阴县"
    }, {
      id: "140622",
      pid: "140600",
      name: "应县"
    }, {
      id: "140623",
      pid: "140600",
      name: "右玉县"
    }, {
      id: "140624",
      pid: "140600",
      name: "怀仁县"
    }, {
      id: "140625",
      pid: "140600",
      name: "其它区"
    }]
  }, {
    id: "140700",
    pid: "140000",
    name: "晋中市",
    children: [{
      id: "140702",
      pid: "140700",
      name: "榆次区"
    }, {
      id: "140721",
      pid: "140700",
      name: "榆社县"
    }, {
      id: "140722",
      pid: "140700",
      name: "左权县"
    }, {
      id: "140723",
      pid: "140700",
      name: "和顺县"
    }, {
      id: "140724",
      pid: "140700",
      name: "昔阳县"
    }, {
      id: "140725",
      pid: "140700",
      name: "寿阳县"
    }, {
      id: "140726",
      pid: "140700",
      name: "太谷县"
    }, {
      id: "140727",
      pid: "140700",
      name: "祁县"
    }, {
      id: "140728",
      pid: "140700",
      name: "平遥县"
    }, {
      id: "140729",
      pid: "140700",
      name: "灵石县"
    }, {
      id: "140781",
      pid: "140700",
      name: "介休市"
    }, {
      id: "140782",
      pid: "140700",
      name: "其它区"
    }]
  }, {
    id: "140800",
    pid: "140000",
    name: "运城市",
    children: [{
      id: "140802",
      pid: "140800",
      name: "盐湖区"
    }, {
      id: "140821",
      pid: "140800",
      name: "临猗县"
    }, {
      id: "140822",
      pid: "140800",
      name: "万荣县"
    }, {
      id: "140823",
      pid: "140800",
      name: "闻喜县"
    }, {
      id: "140824",
      pid: "140800",
      name: "稷山县"
    }, {
      id: "140825",
      pid: "140800",
      name: "新绛县"
    }, {
      id: "140826",
      pid: "140800",
      name: "绛县"
    }, {
      id: "140827",
      pid: "140800",
      name: "垣曲县"
    }, {
      id: "140828",
      pid: "140800",
      name: "夏县"
    }, {
      id: "140829",
      pid: "140800",
      name: "平陆县"
    }, {
      id: "140830",
      pid: "140800",
      name: "芮城县"
    }, {
      id: "140881",
      pid: "140800",
      name: "永济市"
    }, {
      id: "140882",
      pid: "140800",
      name: "河津市"
    }, {
      id: "140883",
      pid: "140800",
      name: "其它区"
    }]
  }, {
    id: "140900",
    pid: "140000",
    name: "忻州市",
    children: [{
      id: "140902",
      pid: "140900",
      name: "忻府区"
    }, {
      id: "140921",
      pid: "140900",
      name: "定襄县"
    }, {
      id: "140922",
      pid: "140900",
      name: "五台县"
    }, {
      id: "140923",
      pid: "140900",
      name: "代县"
    }, {
      id: "140924",
      pid: "140900",
      name: "繁峙县"
    }, {
      id: "140925",
      pid: "140900",
      name: "宁武县"
    }, {
      id: "140926",
      pid: "140900",
      name: "静乐县"
    }, {
      id: "140927",
      pid: "140900",
      name: "神池县"
    }, {
      id: "140928",
      pid: "140900",
      name: "五寨县"
    }, {
      id: "140929",
      pid: "140900",
      name: "岢岚县"
    }, {
      id: "140930",
      pid: "140900",
      name: "河曲县"
    }, {
      id: "140931",
      pid: "140900",
      name: "保德县"
    }, {
      id: "140932",
      pid: "140900",
      name: "偏关县"
    }, {
      id: "140981",
      pid: "140900",
      name: "原平市"
    }, {
      id: "140982",
      pid: "140900",
      name: "其它区"
    }]
  }, {
    id: "141000",
    pid: "140000",
    name: "临汾市",
    children: [{
      id: "141002",
      pid: "141000",
      name: "尧都区"
    }, {
      id: "141021",
      pid: "141000",
      name: "曲沃县"
    }, {
      id: "141022",
      pid: "141000",
      name: "翼城县"
    }, {
      id: "141023",
      pid: "141000",
      name: "襄汾县"
    }, {
      id: "141024",
      pid: "141000",
      name: "洪洞县"
    }, {
      id: "141025",
      pid: "141000",
      name: "古县"
    }, {
      id: "141026",
      pid: "141000",
      name: "安泽县"
    }, {
      id: "141027",
      pid: "141000",
      name: "浮山县"
    }, {
      id: "141028",
      pid: "141000",
      name: "吉县"
    }, {
      id: "141029",
      pid: "141000",
      name: "乡宁县"
    }, {
      id: "141030",
      pid: "141000",
      name: "大宁县"
    }, {
      id: "141031",
      pid: "141000",
      name: "隰县"
    }, {
      id: "141032",
      pid: "141000",
      name: "永和县"
    }, {
      id: "141033",
      pid: "141000",
      name: "蒲县"
    }, {
      id: "141034",
      pid: "141000",
      name: "汾西县"
    }, {
      id: "141081",
      pid: "141000",
      name: "侯马市"
    }, {
      id: "141082",
      pid: "141000",
      name: "霍州市"
    }, {
      id: "141083",
      pid: "141000",
      name: "其它区"
    }]
  }, {
    id: "141100",
    pid: "140000",
    name: "吕梁市",
    children: [{
      id: "141102",
      pid: "141100",
      name: "离石区"
    }, {
      id: "141121",
      pid: "141100",
      name: "文水县"
    }, {
      id: "141122",
      pid: "141100",
      name: "交城县"
    }, {
      id: "141123",
      pid: "141100",
      name: "兴县"
    }, {
      id: "141124",
      pid: "141100",
      name: "临县"
    }, {
      id: "141125",
      pid: "141100",
      name: "柳林县"
    }, {
      id: "141126",
      pid: "141100",
      name: "石楼县"
    }, {
      id: "141127",
      pid: "141100",
      name: "岚县"
    }, {
      id: "141128",
      pid: "141100",
      name: "方山县"
    }, {
      id: "141129",
      pid: "141100",
      name: "中阳县"
    }, {
      id: "141130",
      pid: "141100",
      name: "交口县"
    }, {
      id: "141181",
      pid: "141100",
      name: "孝义市"
    }, {
      id: "141182",
      pid: "141100",
      name: "汾阳市"
    }, {
      id: "141183",
      pid: "141100",
      name: "其它区"
    }]
  }]
}, {
  id: "150000",
  pid: "150000",
  name: "内蒙古自治区",
  children: [{
    id: "150100",
    pid: "150000",
    name: "呼和浩特市",
    children: [{
      id: "150102",
      pid: "150100",
      name: "新城区"
    }, {
      id: "150103",
      pid: "150100",
      name: "回民区"
    }, {
      id: "150104",
      pid: "150100",
      name: "玉泉区"
    }, {
      id: "150105",
      pid: "150100",
      name: "赛罕区"
    }, {
      id: "150121",
      pid: "150100",
      name: "土默特左旗"
    }, {
      id: "150122",
      pid: "150100",
      name: "托克托县"
    }, {
      id: "150123",
      pid: "150100",
      name: "和林格尔县"
    }, {
      id: "150124",
      pid: "150100",
      name: "清水河县"
    }, {
      id: "150125",
      pid: "150100",
      name: "武川县"
    }, {
      id: "150126",
      pid: "150100",
      name: "其它区"
    }]
  }, {
    id: "150200",
    pid: "150000",
    name: "包头市",
    children: [{
      id: "150202",
      pid: "150200",
      name: "东河区"
    }, {
      id: "150203",
      pid: "150200",
      name: "昆都仑区"
    }, {
      id: "150204",
      pid: "150200",
      name: "青山区"
    }, {
      id: "150205",
      pid: "150200",
      name: "石拐区"
    }, {
      id: "150206",
      pid: "150200",
      name: "白云鄂博矿区"
    }, {
      id: "150207",
      pid: "150200",
      name: "九原区"
    }, {
      id: "150221",
      pid: "150200",
      name: "土默特右旗"
    }, {
      id: "150222",
      pid: "150200",
      name: "固阳县"
    }, {
      id: "150223",
      pid: "150200",
      name: "达尔罕茂明安联合旗"
    }, {
      id: "150224",
      pid: "150200",
      name: "其它区"
    }]
  }, {
    id: "150300",
    pid: "150000",
    name: "乌海市",
    children: [{
      id: "150302",
      pid: "150300",
      name: "海勃湾区"
    }, {
      id: "150303",
      pid: "150300",
      name: "海南区"
    }, {
      id: "150304",
      pid: "150300",
      name: "乌达区"
    }, {
      id: "150305",
      pid: "150300",
      name: "其它区"
    }]
  }, {
    id: "150400",
    pid: "150000",
    name: "赤峰市",
    children: [{
      id: "150402",
      pid: "150400",
      name: "红山区"
    }, {
      id: "150403",
      pid: "150400",
      name: "元宝山区"
    }, {
      id: "150404",
      pid: "150400",
      name: "松山区"
    }, {
      id: "150421",
      pid: "150400",
      name: "阿鲁科尔沁旗"
    }, {
      id: "150422",
      pid: "150400",
      name: "巴林左旗"
    }, {
      id: "150423",
      pid: "150400",
      name: "巴林右旗"
    }, {
      id: "150424",
      pid: "150400",
      name: "林西县"
    }, {
      id: "150425",
      pid: "150400",
      name: "克什克腾旗"
    }, {
      id: "150426",
      pid: "150400",
      name: "翁牛特旗"
    }, {
      id: "150428",
      pid: "150400",
      name: "喀喇沁旗"
    }, {
      id: "150429",
      pid: "150400",
      name: "宁城县"
    }, {
      id: "150430",
      pid: "150400",
      name: "敖汉旗"
    }, {
      id: "150431",
      pid: "150400",
      name: "其它区"
    }]
  }, {
    id: "150500",
    pid: "150000",
    name: "通辽市",
    children: [{
      id: "150502",
      pid: "150500",
      name: "科尔沁区"
    }, {
      id: "150521",
      pid: "150500",
      name: "科尔沁左翼中旗"
    }, {
      id: "150522",
      pid: "150500",
      name: "科尔沁左翼后旗"
    }, {
      id: "150523",
      pid: "150500",
      name: "开鲁县"
    }, {
      id: "150524",
      pid: "150500",
      name: "库伦旗"
    }, {
      id: "150525",
      pid: "150500",
      name: "奈曼旗"
    }, {
      id: "150526",
      pid: "150500",
      name: "扎鲁特旗"
    }, {
      id: "150581",
      pid: "150500",
      name: "霍林郭勒市"
    }, {
      id: "150582",
      pid: "150500",
      name: "其它区"
    }]
  }, {
    id: "150600",
    pid: "150000",
    name: "鄂尔多斯市",
    children: [{
      id: "150602",
      pid: "150600",
      name: "东胜区"
    }, {
      id: "150621",
      pid: "150600",
      name: "达拉特旗"
    }, {
      id: "150622",
      pid: "150600",
      name: "准格尔旗"
    }, {
      id: "150623",
      pid: "150600",
      name: "鄂托克前旗"
    }, {
      id: "150624",
      pid: "150600",
      name: "鄂托克旗"
    }, {
      id: "150625",
      pid: "150600",
      name: "杭锦旗"
    }, {
      id: "150626",
      pid: "150600",
      name: "乌审旗"
    }, {
      id: "150627",
      pid: "150600",
      name: "伊金霍洛旗"
    }, {
      id: "150628",
      pid: "150600",
      name: "其它区"
    }]
  }, {
    id: "150700",
    pid: "150000",
    name: "呼伦贝尔市",
    children: [{
      id: "150702",
      pid: "150700",
      name: "海拉尔区"
    }, {
      id: "150703",
      pid: "150700",
      name: "扎赉诺尔区"
    }, {
      id: "150721",
      pid: "150700",
      name: "阿荣旗"
    }, {
      id: "150722",
      pid: "150700",
      name: "莫力达瓦达斡尔族自治旗"
    }, {
      id: "150723",
      pid: "150700",
      name: "鄂伦春自治旗"
    }, {
      id: "150724",
      pid: "150700",
      name: "鄂温克族自治旗"
    }, {
      id: "150725",
      pid: "150700",
      name: "陈巴尔虎旗"
    }, {
      id: "150726",
      pid: "150700",
      name: "新巴尔虎左旗"
    }, {
      id: "150727",
      pid: "150700",
      name: "新巴尔虎右旗"
    }, {
      id: "150781",
      pid: "150700",
      name: "满洲里市"
    }, {
      id: "150782",
      pid: "150700",
      name: "牙克石市"
    }, {
      id: "150783",
      pid: "150700",
      name: "扎兰屯市"
    }, {
      id: "150784",
      pid: "150700",
      name: "额尔古纳市"
    }, {
      id: "150785",
      pid: "150700",
      name: "根河市"
    }, {
      id: "150786",
      pid: "150700",
      name: "其它区"
    }]
  }, {
    id: "150800",
    pid: "150000",
    name: "巴彦淖尔市",
    children: [{
      id: "150802",
      pid: "150800",
      name: "临河区"
    }, {
      id: "150821",
      pid: "150800",
      name: "五原县"
    }, {
      id: "150822",
      pid: "150800",
      name: "磴口县"
    }, {
      id: "150823",
      pid: "150800",
      name: "乌拉特前旗"
    }, {
      id: "150824",
      pid: "150800",
      name: "乌拉特中旗"
    }, {
      id: "150825",
      pid: "150800",
      name: "乌拉特后旗"
    }, {
      id: "150826",
      pid: "150800",
      name: "杭锦后旗"
    }, {
      id: "150827",
      pid: "150800",
      name: "其它区"
    }]
  }, {
    id: "150900",
    pid: "150000",
    name: "乌兰察布市",
    children: [{
      id: "150902",
      pid: "150900",
      name: "集宁区"
    }, {
      id: "150921",
      pid: "150900",
      name: "卓资县"
    }, {
      id: "150922",
      pid: "150900",
      name: "化德县"
    }, {
      id: "150923",
      pid: "150900",
      name: "商都县"
    }, {
      id: "150924",
      pid: "150900",
      name: "兴和县"
    }, {
      id: "150925",
      pid: "150900",
      name: "凉城县"
    }, {
      id: "150926",
      pid: "150900",
      name: "察哈尔右翼前旗"
    }, {
      id: "150927",
      pid: "150900",
      name: "察哈尔右翼中旗"
    }, {
      id: "150928",
      pid: "150900",
      name: "察哈尔右翼后旗"
    }, {
      id: "150929",
      pid: "150900",
      name: "四子王旗"
    }, {
      id: "150981",
      pid: "150900",
      name: "丰镇市"
    }, {
      id: "150982",
      pid: "150900",
      name: "其它区"
    }]
  }, {
    id: "152200",
    pid: "150000",
    name: "兴安盟",
    children: [{
      id: "152201",
      pid: "152200",
      name: "乌兰浩特市"
    }, {
      id: "152202",
      pid: "152200",
      name: "阿尔山市"
    }, {
      id: "152221",
      pid: "152200",
      name: "科尔沁右翼前旗"
    }, {
      id: "152222",
      pid: "152200",
      name: "科尔沁右翼中旗"
    }, {
      id: "152223",
      pid: "152200",
      name: "扎赉特旗"
    }, {
      id: "152224",
      pid: "152200",
      name: "突泉县"
    }, {
      id: "152225",
      pid: "152200",
      name: "其它区"
    }]
  }, {
    id: "152500",
    pid: "150000",
    name: "锡林郭勒盟",
    children: [{
      id: "152501",
      pid: "152500",
      name: "二连浩特市"
    }, {
      id: "152502",
      pid: "152500",
      name: "锡林浩特市"
    }, {
      id: "152522",
      pid: "152500",
      name: "阿巴嘎旗"
    }, {
      id: "152523",
      pid: "152500",
      name: "苏尼特左旗"
    }, {
      id: "152524",
      pid: "152500",
      name: "苏尼特右旗"
    }, {
      id: "152525",
      pid: "152500",
      name: "东乌珠穆沁旗"
    }, {
      id: "152526",
      pid: "152500",
      name: "西乌珠穆沁旗"
    }, {
      id: "152527",
      pid: "152500",
      name: "太仆寺旗"
    }, {
      id: "152528",
      pid: "152500",
      name: "镶黄旗"
    }, {
      id: "152529",
      pid: "152500",
      name: "正镶白旗"
    }, {
      id: "152530",
      pid: "152500",
      name: "正蓝旗"
    }, {
      id: "152531",
      pid: "152500",
      name: "多伦县"
    }, {
      id: "152532",
      pid: "152500",
      name: "其它区"
    }]
  }, {
    id: "152900",
    pid: "150000",
    name: "阿拉善盟",
    children: [{
      id: "152921",
      pid: "152900",
      name: "阿拉善左旗"
    }, {
      id: "152922",
      pid: "152900",
      name: "阿拉善右旗"
    }, {
      id: "152923",
      pid: "152900",
      name: "额济纳旗"
    }, {
      id: "152924",
      pid: "152900",
      name: "其它区"
    }]
  }]
}, {
  id: "210000",
  pid: "210000",
  name: "辽宁省",
  children: [{
    id: "210100",
    pid: "210000",
    name: "沈阳市",
    children: [{
      id: "210102",
      pid: "210100",
      name: "和平区"
    }, {
      id: "210103",
      pid: "210100",
      name: "沈河区"
    }, {
      id: "210104",
      pid: "210100",
      name: "大东区"
    }, {
      id: "210105",
      pid: "210100",
      name: "皇姑区"
    }, {
      id: "210106",
      pid: "210100",
      name: "铁西区"
    }, {
      id: "210111",
      pid: "210100",
      name: "苏家屯区"
    }, {
      id: "210112",
      pid: "210100",
      name: "东陵区"
    }, {
      id: "210113",
      pid: "210100",
      name: "新城子区"
    }, {
      id: "210114",
      pid: "210100",
      name: "于洪区"
    }, {
      id: "210122",
      pid: "210100",
      name: "辽中县"
    }, {
      id: "210123",
      pid: "210100",
      name: "康平县"
    }, {
      id: "210124",
      pid: "210100",
      name: "法库县"
    }, {
      id: "210181",
      pid: "210100",
      name: "新民市"
    }, {
      id: "210184",
      pid: "210100",
      name: "沈北新区"
    }, {
      id: "210185",
      pid: "210100",
      name: "其它区"
    }]
  }, {
    id: "210200",
    pid: "210000",
    name: "大连市",
    children: [{
      id: "210202",
      pid: "210200",
      name: "中山区"
    }, {
      id: "210203",
      pid: "210200",
      name: "西岗区"
    }, {
      id: "210204",
      pid: "210200",
      name: "沙河口区"
    }, {
      id: "210211",
      pid: "210200",
      name: "甘井子区"
    }, {
      id: "210212",
      pid: "210200",
      name: "旅顺口区"
    }, {
      id: "210213",
      pid: "210200",
      name: "金州区"
    }, {
      id: "210224",
      pid: "210200",
      name: "长海县"
    }, {
      id: "210281",
      pid: "210200",
      name: "瓦房店市"
    }, {
      id: "210282",
      pid: "210200",
      name: "普兰店市"
    }, {
      id: "210283",
      pid: "210200",
      name: "庄河市"
    }, {
      id: "210298",
      pid: "210200",
      name: "其它区"
    }]
  }, {
    id: "210300",
    pid: "210000",
    name: "鞍山市",
    children: [{
      id: "210302",
      pid: "210300",
      name: "铁东区"
    }, {
      id: "210303",
      pid: "210300",
      name: "铁西区"
    }, {
      id: "210304",
      pid: "210300",
      name: "立山区"
    }, {
      id: "210311",
      pid: "210300",
      name: "千山区"
    }, {
      id: "210321",
      pid: "210300",
      name: "台安县"
    }, {
      id: "210323",
      pid: "210300",
      name: "岫岩满族自治县"
    }, {
      id: "210381",
      pid: "210300",
      name: "海城市"
    }, {
      id: "210382",
      pid: "210300",
      name: "其它区"
    }]
  }, {
    id: "210400",
    pid: "210000",
    name: "抚顺市",
    children: [{
      id: "210402",
      pid: "210400",
      name: "新抚区"
    }, {
      id: "210403",
      pid: "210400",
      name: "东洲区"
    }, {
      id: "210404",
      pid: "210400",
      name: "望花区"
    }, {
      id: "210411",
      pid: "210400",
      name: "顺城区"
    }, {
      id: "210421",
      pid: "210400",
      name: "抚顺县"
    }, {
      id: "210422",
      pid: "210400",
      name: "新宾满族自治县"
    }, {
      id: "210423",
      pid: "210400",
      name: "清原满族自治县"
    }, {
      id: "210424",
      pid: "210400",
      name: "其它区"
    }]
  }, {
    id: "210500",
    pid: "210000",
    name: "本溪市",
    children: [{
      id: "210502",
      pid: "210500",
      name: "平山区"
    }, {
      id: "210503",
      pid: "210500",
      name: "溪湖区"
    }, {
      id: "210504",
      pid: "210500",
      name: "明山区"
    }, {
      id: "210505",
      pid: "210500",
      name: "南芬区"
    }, {
      id: "210521",
      pid: "210500",
      name: "本溪满族自治县"
    }, {
      id: "210522",
      pid: "210500",
      name: "桓仁满族自治县"
    }, {
      id: "210523",
      pid: "210500",
      name: "其它区"
    }]
  }, {
    id: "210600",
    pid: "210000",
    name: "丹东市",
    children: [{
      id: "210602",
      pid: "210600",
      name: "元宝区"
    }, {
      id: "210603",
      pid: "210600",
      name: "振兴区"
    }, {
      id: "210604",
      pid: "210600",
      name: "振安区"
    }, {
      id: "210624",
      pid: "210600",
      name: "宽甸满族自治县"
    }, {
      id: "210681",
      pid: "210600",
      name: "东港市"
    }, {
      id: "210682",
      pid: "210600",
      name: "凤城市"
    }, {
      id: "210683",
      pid: "210600",
      name: "其它区"
    }]
  }, {
    id: "210700",
    pid: "210000",
    name: "锦州市",
    children: [{
      id: "210702",
      pid: "210700",
      name: "古塔区"
    }, {
      id: "210703",
      pid: "210700",
      name: "凌河区"
    }, {
      id: "210711",
      pid: "210700",
      name: "太和区"
    }, {
      id: "210726",
      pid: "210700",
      name: "黑山县"
    }, {
      id: "210727",
      pid: "210700",
      name: "义县"
    }, {
      id: "210781",
      pid: "210700",
      name: "凌海市"
    }, {
      id: "210782",
      pid: "210700",
      name: "北镇市"
    }, {
      id: "210783",
      pid: "210700",
      name: "其它区"
    }]
  }, {
    id: "210800",
    pid: "210000",
    name: "营口市",
    children: [{
      id: "210802",
      pid: "210800",
      name: "站前区"
    }, {
      id: "210803",
      pid: "210800",
      name: "西市区"
    }, {
      id: "210804",
      pid: "210800",
      name: "鲅鱼圈区"
    }, {
      id: "210811",
      pid: "210800",
      name: "老边区"
    }, {
      id: "210881",
      pid: "210800",
      name: "盖州市"
    }, {
      id: "210882",
      pid: "210800",
      name: "大石桥市"
    }, {
      id: "210883",
      pid: "210800",
      name: "其它区"
    }]
  }, {
    id: "210900",
    pid: "210000",
    name: "阜新市",
    children: [{
      id: "210902",
      pid: "210900",
      name: "海州区"
    }, {
      id: "210903",
      pid: "210900",
      name: "新邱区"
    }, {
      id: "210904",
      pid: "210900",
      name: "太平区"
    }, {
      id: "210905",
      pid: "210900",
      name: "清河门区"
    }, {
      id: "210911",
      pid: "210900",
      name: "细河区"
    }, {
      id: "210921",
      pid: "210900",
      name: "阜新蒙古族自治县"
    }, {
      id: "210922",
      pid: "210900",
      name: "彰武县"
    }, {
      id: "210923",
      pid: "210900",
      name: "其它区"
    }]
  }, {
    id: "211000",
    pid: "210000",
    name: "辽阳市",
    children: [{
      id: "211002",
      pid: "211000",
      name: "白塔区"
    }, {
      id: "211003",
      pid: "211000",
      name: "文圣区"
    }, {
      id: "211004",
      pid: "211000",
      name: "宏伟区"
    }, {
      id: "211005",
      pid: "211000",
      name: "弓长岭区"
    }, {
      id: "211011",
      pid: "211000",
      name: "太子河区"
    }, {
      id: "211021",
      pid: "211000",
      name: "辽阳县"
    }, {
      id: "211081",
      pid: "211000",
      name: "灯塔市"
    }, {
      id: "211082",
      pid: "211000",
      name: "其它区"
    }]
  }, {
    id: "211100",
    pid: "210000",
    name: "盘锦市",
    children: [{
      id: "211102",
      pid: "211100",
      name: "双台子区"
    }, {
      id: "211103",
      pid: "211100",
      name: "兴隆台区"
    }, {
      id: "211121",
      pid: "211100",
      name: "大洼县"
    }, {
      id: "211122",
      pid: "211100",
      name: "盘山县"
    }, {
      id: "211123",
      pid: "211100",
      name: "其它区"
    }]
  }, {
    id: "211200",
    pid: "210000",
    name: "铁岭市",
    children: [{
      id: "211202",
      pid: "211200",
      name: "银州区"
    }, {
      id: "211204",
      pid: "211200",
      name: "清河区"
    }, {
      id: "211221",
      pid: "211200",
      name: "铁岭县"
    }, {
      id: "211223",
      pid: "211200",
      name: "西丰县"
    }, {
      id: "211224",
      pid: "211200",
      name: "昌图县"
    }, {
      id: "211281",
      pid: "211200",
      name: "调兵山市"
    }, {
      id: "211282",
      pid: "211200",
      name: "开原市"
    }, {
      id: "211283",
      pid: "211200",
      name: "其它区"
    }]
  }, {
    id: "211300",
    pid: "210000",
    name: "朝阳市",
    children: [{
      id: "211302",
      pid: "211300",
      name: "双塔区"
    }, {
      id: "211303",
      pid: "211300",
      name: "龙城区"
    }, {
      id: "211321",
      pid: "211300",
      name: "朝阳县"
    }, {
      id: "211322",
      pid: "211300",
      name: "建平县"
    }, {
      id: "211324",
      pid: "211300",
      name: "喀喇沁左翼蒙古族自治县"
    }, {
      id: "211381",
      pid: "211300",
      name: "北票市"
    }, {
      id: "211382",
      pid: "211300",
      name: "凌源市"
    }, {
      id: "211383",
      pid: "211300",
      name: "其它区"
    }]
  }, {
    id: "211400",
    pid: "210000",
    name: "葫芦岛市",
    children: [{
      id: "211402",
      pid: "211400",
      name: "连山区"
    }, {
      id: "211403",
      pid: "211400",
      name: "龙港区"
    }, {
      id: "211404",
      pid: "211400",
      name: "南票区"
    }, {
      id: "211421",
      pid: "211400",
      name: "绥中县"
    }, {
      id: "211422",
      pid: "211400",
      name: "建昌县"
    }, {
      id: "211481",
      pid: "211400",
      name: "兴城市"
    }, {
      id: "211482",
      pid: "211400",
      name: "其它区"
    }]
  }]
}, {
  id: "220000",
  pid: "220000",
  name: "吉林省",
  children: [{
    id: "220100",
    pid: "220000",
    name: "长春市",
    children: [{
      id: "220102",
      pid: "220100",
      name: "南关区"
    }, {
      id: "220103",
      pid: "220100",
      name: "宽城区"
    }, {
      id: "220104",
      pid: "220100",
      name: "朝阳区"
    }, {
      id: "220105",
      pid: "220100",
      name: "二道区"
    }, {
      id: "220106",
      pid: "220100",
      name: "绿园区"
    }, {
      id: "220112",
      pid: "220100",
      name: "双阳区"
    }, {
      id: "220122",
      pid: "220100",
      name: "农安县"
    }, {
      id: "220181",
      pid: "220100",
      name: "九台市"
    }, {
      id: "220182",
      pid: "220100",
      name: "榆树市"
    }, {
      id: "220183",
      pid: "220100",
      name: "德惠市"
    }, {
      id: "220188",
      pid: "220100",
      name: "其它区"
    }]
  }, {
    id: "220200",
    pid: "220000",
    name: "吉林市",
    children: [{
      id: "220202",
      pid: "220200",
      name: "昌邑区"
    }, {
      id: "220203",
      pid: "220200",
      name: "龙潭区"
    }, {
      id: "220204",
      pid: "220200",
      name: "船营区"
    }, {
      id: "220211",
      pid: "220200",
      name: "丰满区"
    }, {
      id: "220221",
      pid: "220200",
      name: "永吉县"
    }, {
      id: "220281",
      pid: "220200",
      name: "蛟河市"
    }, {
      id: "220282",
      pid: "220200",
      name: "桦甸市"
    }, {
      id: "220283",
      pid: "220200",
      name: "舒兰市"
    }, {
      id: "220284",
      pid: "220200",
      name: "磐石市"
    }, {
      id: "220285",
      pid: "220200",
      name: "其它区"
    }]
  }, {
    id: "220300",
    pid: "220000",
    name: "四平市",
    children: [{
      id: "220302",
      pid: "220300",
      name: "铁西区"
    }, {
      id: "220303",
      pid: "220300",
      name: "铁东区"
    }, {
      id: "220322",
      pid: "220300",
      name: "梨树县"
    }, {
      id: "220323",
      pid: "220300",
      name: "伊通满族自治县"
    }, {
      id: "220381",
      pid: "220300",
      name: "公主岭市"
    }, {
      id: "220382",
      pid: "220300",
      name: "双辽市"
    }, {
      id: "220383",
      pid: "220300",
      name: "其它区"
    }]
  }, {
    id: "220400",
    pid: "220000",
    name: "辽源市",
    children: [{
      id: "220402",
      pid: "220400",
      name: "龙山区"
    }, {
      id: "220403",
      pid: "220400",
      name: "西安区"
    }, {
      id: "220421",
      pid: "220400",
      name: "东丰县"
    }, {
      id: "220422",
      pid: "220400",
      name: "东辽县"
    }, {
      id: "220423",
      pid: "220400",
      name: "其它区"
    }]
  }, {
    id: "220500",
    pid: "220000",
    name: "通化市",
    children: [{
      id: "220502",
      pid: "220500",
      name: "东昌区"
    }, {
      id: "220503",
      pid: "220500",
      name: "二道江区"
    }, {
      id: "220521",
      pid: "220500",
      name: "通化县"
    }, {
      id: "220523",
      pid: "220500",
      name: "辉南县"
    }, {
      id: "220524",
      pid: "220500",
      name: "柳河县"
    }, {
      id: "220581",
      pid: "220500",
      name: "梅河口市"
    }, {
      id: "220582",
      pid: "220500",
      name: "集安市"
    }, {
      id: "220583",
      pid: "220500",
      name: "其它区"
    }]
  }, {
    id: "220600",
    pid: "220000",
    name: "白山市",
    children: [{
      id: "220602",
      pid: "220600",
      name: "浑江区"
    }, {
      id: "220621",
      pid: "220600",
      name: "抚松县"
    }, {
      id: "220622",
      pid: "220600",
      name: "靖宇县"
    }, {
      id: "220623",
      pid: "220600",
      name: "长白朝鲜族自治县"
    }, {
      id: "220625",
      pid: "220600",
      name: "江源区"
    }, {
      id: "220681",
      pid: "220600",
      name: "临江市"
    }, {
      id: "220682",
      pid: "220600",
      name: "其它区"
    }]
  }, {
    id: "220700",
    pid: "220000",
    name: "松原市",
    children: [{
      id: "220702",
      pid: "220700",
      name: "宁江区"
    }, {
      id: "220721",
      pid: "220700",
      name: "前郭尔罗斯蒙古族自治县"
    }, {
      id: "220722",
      pid: "220700",
      name: "长岭县"
    }, {
      id: "220723",
      pid: "220700",
      name: "乾安县"
    }, {
      id: "220724",
      pid: "220700",
      name: "扶余市"
    }, {
      id: "220725",
      pid: "220700",
      name: "其它区"
    }]
  }, {
    id: "220800",
    pid: "220000",
    name: "白城市",
    children: [{
      id: "220802",
      pid: "220800",
      name: "洮北区"
    }, {
      id: "220821",
      pid: "220800",
      name: "镇赉县"
    }, {
      id: "220822",
      pid: "220800",
      name: "通榆县"
    }, {
      id: "220881",
      pid: "220800",
      name: "洮南市"
    }, {
      id: "220882",
      pid: "220800",
      name: "大安市"
    }, {
      id: "220883",
      pid: "220800",
      name: "其它区"
    }]
  }, {
    id: "222400",
    pid: "220000",
    name: "延边朝鲜族自治州",
    children: [{
      id: "222401",
      pid: "222400",
      name: "延吉市"
    }, {
      id: "222402",
      pid: "222400",
      name: "图们市"
    }, {
      id: "222403",
      pid: "222400",
      name: "敦化市"
    }, {
      id: "222404",
      pid: "222400",
      name: "珲春市"
    }, {
      id: "222405",
      pid: "222400",
      name: "龙井市"
    }, {
      id: "222406",
      pid: "222400",
      name: "和龙市"
    }, {
      id: "222424",
      pid: "222400",
      name: "汪清县"
    }, {
      id: "222426",
      pid: "222400",
      name: "安图县"
    }, {
      id: "222427",
      pid: "222400",
      name: "其它区"
    }]
  }]
}, {
  id: "230000",
  pid: "230000",
  name: "黑龙江省",
  children: [{
    id: "230100",
    pid: "230000",
    name: "哈尔滨市",
    children: [{
      id: "230102",
      pid: "230100",
      name: "道里区"
    }, {
      id: "230103",
      pid: "230100",
      name: "南岗区"
    }, {
      id: "230104",
      pid: "230100",
      name: "道外区"
    }, {
      id: "230106",
      pid: "230100",
      name: "香坊区"
    }, {
      id: "230108",
      pid: "230100",
      name: "平房区"
    }, {
      id: "230109",
      pid: "230100",
      name: "松北区"
    }, {
      id: "230111",
      pid: "230100",
      name: "呼兰区"
    }, {
      id: "230123",
      pid: "230100",
      name: "依兰县"
    }, {
      id: "230124",
      pid: "230100",
      name: "方正县"
    }, {
      id: "230125",
      pid: "230100",
      name: "宾县"
    }, {
      id: "230126",
      pid: "230100",
      name: "巴彦县"
    }, {
      id: "230127",
      pid: "230100",
      name: "木兰县"
    }, {
      id: "230128",
      pid: "230100",
      name: "通河县"
    }, {
      id: "230129",
      pid: "230100",
      name: "延寿县"
    }, {
      id: "230181",
      pid: "230100",
      name: "阿城区"
    }, {
      id: "230182",
      pid: "230100",
      name: "双城市"
    }, {
      id: "230183",
      pid: "230100",
      name: "尚志市"
    }, {
      id: "230184",
      pid: "230100",
      name: "五常市"
    }, {
      id: "230186",
      pid: "230100",
      name: "其它区"
    }]
  }, {
    id: "230200",
    pid: "230000",
    name: "齐齐哈尔市",
    children: [{
      id: "230202",
      pid: "230200",
      name: "龙沙区"
    }, {
      id: "230203",
      pid: "230200",
      name: "建华区"
    }, {
      id: "230204",
      pid: "230200",
      name: "铁锋区"
    }, {
      id: "230205",
      pid: "230200",
      name: "昂昂溪区"
    }, {
      id: "230206",
      pid: "230200",
      name: "富拉尔基区"
    }, {
      id: "230207",
      pid: "230200",
      name: "碾子山区"
    }, {
      id: "230208",
      pid: "230200",
      name: "梅里斯达斡尔族区"
    }, {
      id: "230221",
      pid: "230200",
      name: "龙江县"
    }, {
      id: "230223",
      pid: "230200",
      name: "依安县"
    }, {
      id: "230224",
      pid: "230200",
      name: "泰来县"
    }, {
      id: "230225",
      pid: "230200",
      name: "甘南县"
    }, {
      id: "230227",
      pid: "230200",
      name: "富裕县"
    }, {
      id: "230229",
      pid: "230200",
      name: "克山县"
    }, {
      id: "230230",
      pid: "230200",
      name: "克东县"
    }, {
      id: "230231",
      pid: "230200",
      name: "拜泉县"
    }, {
      id: "230281",
      pid: "230200",
      name: "讷河市"
    }, {
      id: "230282",
      pid: "230200",
      name: "其它区"
    }]
  }, {
    id: "230300",
    pid: "230000",
    name: "鸡西市",
    children: [{
      id: "230302",
      pid: "230300",
      name: "鸡冠区"
    }, {
      id: "230303",
      pid: "230300",
      name: "恒山区"
    }, {
      id: "230304",
      pid: "230300",
      name: "滴道区"
    }, {
      id: "230305",
      pid: "230300",
      name: "梨树区"
    }, {
      id: "230306",
      pid: "230300",
      name: "城子河区"
    }, {
      id: "230307",
      pid: "230300",
      name: "麻山区"
    }, {
      id: "230321",
      pid: "230300",
      name: "鸡东县"
    }, {
      id: "230381",
      pid: "230300",
      name: "虎林市"
    }, {
      id: "230382",
      pid: "230300",
      name: "密山市"
    }, {
      id: "230383",
      pid: "230300",
      name: "其它区"
    }]
  }, {
    id: "230400",
    pid: "230000",
    name: "鹤岗市",
    children: [{
      id: "230402",
      pid: "230400",
      name: "向阳区"
    }, {
      id: "230403",
      pid: "230400",
      name: "工农区"
    }, {
      id: "230404",
      pid: "230400",
      name: "南山区"
    }, {
      id: "230405",
      pid: "230400",
      name: "兴安区"
    }, {
      id: "230406",
      pid: "230400",
      name: "东山区"
    }, {
      id: "230407",
      pid: "230400",
      name: "兴山区"
    }, {
      id: "230421",
      pid: "230400",
      name: "萝北县"
    }, {
      id: "230422",
      pid: "230400",
      name: "绥滨县"
    }, {
      id: "230423",
      pid: "230400",
      name: "其它区"
    }]
  }, {
    id: "230500",
    pid: "230000",
    name: "双鸭山市",
    children: [{
      id: "230502",
      pid: "230500",
      name: "尖山区"
    }, {
      id: "230503",
      pid: "230500",
      name: "岭东区"
    }, {
      id: "230505",
      pid: "230500",
      name: "四方台区"
    }, {
      id: "230506",
      pid: "230500",
      name: "宝山区"
    }, {
      id: "230521",
      pid: "230500",
      name: "集贤县"
    }, {
      id: "230522",
      pid: "230500",
      name: "友谊县"
    }, {
      id: "230523",
      pid: "230500",
      name: "宝清县"
    }, {
      id: "230524",
      pid: "230500",
      name: "饶河县"
    }, {
      id: "230525",
      pid: "230500",
      name: "其它区"
    }]
  }, {
    id: "230600",
    pid: "230000",
    name: "大庆市",
    children: [{
      id: "230602",
      pid: "230600",
      name: "萨尔图区"
    }, {
      id: "230603",
      pid: "230600",
      name: "龙凤区"
    }, {
      id: "230604",
      pid: "230600",
      name: "让胡路区"
    }, {
      id: "230605",
      pid: "230600",
      name: "红岗区"
    }, {
      id: "230606",
      pid: "230600",
      name: "大同区"
    }, {
      id: "230621",
      pid: "230600",
      name: "肇州县"
    }, {
      id: "230622",
      pid: "230600",
      name: "肇源县"
    }, {
      id: "230623",
      pid: "230600",
      name: "林甸县"
    }, {
      id: "230624",
      pid: "230600",
      name: "杜尔伯特蒙古族自治县"
    }, {
      id: "230625",
      pid: "230600",
      name: "其它区"
    }]
  }, {
    id: "230700",
    pid: "230000",
    name: "伊春市",
    children: [{
      id: "230702",
      pid: "230700",
      name: "伊春区"
    }, {
      id: "230703",
      pid: "230700",
      name: "南岔区"
    }, {
      id: "230704",
      pid: "230700",
      name: "友好区"
    }, {
      id: "230705",
      pid: "230700",
      name: "西林区"
    }, {
      id: "230706",
      pid: "230700",
      name: "翠峦区"
    }, {
      id: "230707",
      pid: "230700",
      name: "新青区"
    }, {
      id: "230708",
      pid: "230700",
      name: "美溪区"
    }, {
      id: "230709",
      pid: "230700",
      name: "金山屯区"
    }, {
      id: "230710",
      pid: "230700",
      name: "五营区"
    }, {
      id: "230711",
      pid: "230700",
      name: "乌马河区"
    }, {
      id: "230712",
      pid: "230700",
      name: "汤旺河区"
    }, {
      id: "230713",
      pid: "230700",
      name: "带岭区"
    }, {
      id: "230714",
      pid: "230700",
      name: "乌伊岭区"
    }, {
      id: "230715",
      pid: "230700",
      name: "红星区"
    }, {
      id: "230716",
      pid: "230700",
      name: "上甘岭区"
    }, {
      id: "230722",
      pid: "230700",
      name: "嘉荫县"
    }, {
      id: "230781",
      pid: "230700",
      name: "铁力市"
    }, {
      id: "230782",
      pid: "230700",
      name: "其它区"
    }]
  }, {
    id: "230800",
    pid: "230000",
    name: "佳木斯市",
    children: [{
      id: "230803",
      pid: "230800",
      name: "向阳区"
    }, {
      id: "230804",
      pid: "230800",
      name: "前进区"
    }, {
      id: "230805",
      pid: "230800",
      name: "东风区"
    }, {
      id: "230811",
      pid: "230800",
      name: "郊区"
    }, {
      id: "230822",
      pid: "230800",
      name: "桦南县"
    }, {
      id: "230826",
      pid: "230800",
      name: "桦川县"
    }, {
      id: "230828",
      pid: "230800",
      name: "汤原县"
    }, {
      id: "230833",
      pid: "230800",
      name: "抚远县"
    }, {
      id: "230881",
      pid: "230800",
      name: "同江市"
    }, {
      id: "230882",
      pid: "230800",
      name: "富锦市"
    }, {
      id: "230883",
      pid: "230800",
      name: "其它区"
    }]
  }, {
    id: "230900",
    pid: "230000",
    name: "七台河市",
    children: [{
      id: "230902",
      pid: "230900",
      name: "新兴区"
    }, {
      id: "230903",
      pid: "230900",
      name: "桃山区"
    }, {
      id: "230904",
      pid: "230900",
      name: "茄子河区"
    }, {
      id: "230921",
      pid: "230900",
      name: "勃利县"
    }, {
      id: "230922",
      pid: "230900",
      name: "其它区"
    }]
  }, {
    id: "231000",
    pid: "230000",
    name: "牡丹江市",
    children: [{
      id: "231002",
      pid: "231000",
      name: "东安区"
    }, {
      id: "231003",
      pid: "231000",
      name: "阳明区"
    }, {
      id: "231004",
      pid: "231000",
      name: "爱民区"
    }, {
      id: "231005",
      pid: "231000",
      name: "西安区"
    }, {
      id: "231024",
      pid: "231000",
      name: "东宁县"
    }, {
      id: "231025",
      pid: "231000",
      name: "林口县"
    }, {
      id: "231081",
      pid: "231000",
      name: "绥芬河市"
    }, {
      id: "231083",
      pid: "231000",
      name: "海林市"
    }, {
      id: "231084",
      pid: "231000",
      name: "宁安市"
    }, {
      id: "231085",
      pid: "231000",
      name: "穆棱市"
    }, {
      id: "231086",
      pid: "231000",
      name: "其它区"
    }]
  }, {
    id: "231100",
    pid: "230000",
    name: "黑河市",
    children: [{
      id: "231102",
      pid: "231100",
      name: "爱辉区"
    }, {
      id: "231121",
      pid: "231100",
      name: "嫩江县"
    }, {
      id: "231123",
      pid: "231100",
      name: "逊克县"
    }, {
      id: "231124",
      pid: "231100",
      name: "孙吴县"
    }, {
      id: "231181",
      pid: "231100",
      name: "北安市"
    }, {
      id: "231182",
      pid: "231100",
      name: "五大连池市"
    }, {
      id: "231183",
      pid: "231100",
      name: "其它区"
    }]
  }, {
    id: "231200",
    pid: "230000",
    name: "绥化市",
    children: [{
      id: "231202",
      pid: "231200",
      name: "北林区"
    }, {
      id: "231221",
      pid: "231200",
      name: "望奎县"
    }, {
      id: "231222",
      pid: "231200",
      name: "兰西县"
    }, {
      id: "231223",
      pid: "231200",
      name: "青冈县"
    }, {
      id: "231224",
      pid: "231200",
      name: "庆安县"
    }, {
      id: "231225",
      pid: "231200",
      name: "明水县"
    }, {
      id: "231226",
      pid: "231200",
      name: "绥棱县"
    }, {
      id: "231281",
      pid: "231200",
      name: "安达市"
    }, {
      id: "231282",
      pid: "231200",
      name: "肇东市"
    }, {
      id: "231283",
      pid: "231200",
      name: "海伦市"
    }, {
      id: "231284",
      pid: "231200",
      name: "其它区"
    }]
  }, {
    id: "232700",
    pid: "230000",
    name: "大兴安岭地区",
    children: [{
      id: "232702",
      pid: "232700",
      name: "松岭区"
    }, {
      id: "232703",
      pid: "232700",
      name: "新林区"
    }, {
      id: "232704",
      pid: "232700",
      name: "呼中区"
    }, {
      id: "232721",
      pid: "232700",
      name: "呼玛县"
    }, {
      id: "232722",
      pid: "232700",
      name: "塔河县"
    }, {
      id: "232723",
      pid: "232700",
      name: "漠河县"
    }, {
      id: "232724",
      pid: "232700",
      name: "加格达奇区"
    }, {
      id: "232725",
      pid: "232700",
      name: "其它区"
    }]
  }]
}, {
  id: "310000",
  pid: "310000",
  name: "上海",
  children: [{
    id: "310100",
    pid: "310000",
    name: "上海市",
    children: [{
      id: "310101",
      pid: "310100",
      name: "黄浦区"
    }, {
      id: "310104",
      pid: "310100",
      name: "徐汇区"
    }, {
      id: "310105",
      pid: "310100",
      name: "长宁区"
    }, {
      id: "310106",
      pid: "310100",
      name: "静安区"
    }, {
      id: "310107",
      pid: "310100",
      name: "普陀区"
    }, {
      id: "310108",
      pid: "310100",
      name: "闸北区"
    }, {
      id: "310109",
      pid: "310100",
      name: "虹口区"
    }, {
      id: "310110",
      pid: "310100",
      name: "杨浦区"
    }, {
      id: "310112",
      pid: "310100",
      name: "闵行区"
    }, {
      id: "310113",
      pid: "310100",
      name: "宝山区"
    }, {
      id: "310114",
      pid: "310100",
      name: "嘉定区"
    }, {
      id: "310115",
      pid: "310100",
      name: "浦东新区"
    }, {
      id: "310116",
      pid: "310100",
      name: "金山区"
    }, {
      id: "310117",
      pid: "310100",
      name: "松江区"
    }, {
      id: "310118",
      pid: "310100",
      name: "青浦区"
    }, {
      id: "310120",
      pid: "310100",
      name: "奉贤区"
    }]
  }, {
    id: "310230",
    pid: "310000",
    name: "崇明县"
  }, {
    id: "310231",
    pid: "310000",
    name: "其它区"
  }]
}, {
  id: "320000",
  pid: "320000",
  name: "江苏省",
  children: [{
    id: "320100",
    pid: "320000",
    name: "南京市",
    children: [{
      id: "320102",
      pid: "320100",
      name: "玄武区"
    }, {
      id: "320104",
      pid: "320100",
      name: "秦淮区"
    }, {
      id: "320105",
      pid: "320100",
      name: "建邺区"
    }, {
      id: "320106",
      pid: "320100",
      name: "鼓楼区"
    }, {
      id: "320111",
      pid: "320100",
      name: "浦口区"
    }, {
      id: "320113",
      pid: "320100",
      name: "栖霞区"
    }, {
      id: "320114",
      pid: "320100",
      name: "雨花台区"
    }, {
      id: "320115",
      pid: "320100",
      name: "江宁区"
    }, {
      id: "320116",
      pid: "320100",
      name: "六合区"
    }, {
      id: "320124",
      pid: "320100",
      name: "溧水区"
    }, {
      id: "320125",
      pid: "320100",
      name: "高淳区"
    }, {
      id: "320126",
      pid: "320100",
      name: "其它区"
    }]
  }, {
    id: "320200",
    pid: "320000",
    name: "无锡市",
    children: [{
      id: "320202",
      pid: "320200",
      name: "崇安区"
    }, {
      id: "320203",
      pid: "320200",
      name: "南长区"
    }, {
      id: "320204",
      pid: "320200",
      name: "北塘区"
    }, {
      id: "320205",
      pid: "320200",
      name: "锡山区"
    }, {
      id: "320206",
      pid: "320200",
      name: "惠山区"
    }, {
      id: "320211",
      pid: "320200",
      name: "滨湖区"
    }, {
      id: "320281",
      pid: "320200",
      name: "江阴市"
    }, {
      id: "320282",
      pid: "320200",
      name: "宜兴市"
    }, {
      id: "320297",
      pid: "320200",
      name: "其它区"
    }]
  }, {
    id: "320300",
    pid: "320000",
    name: "徐州市",
    children: [{
      id: "320302",
      pid: "320300",
      name: "鼓楼区"
    }, {
      id: "320303",
      pid: "320300",
      name: "云龙区"
    }, {
      id: "320305",
      pid: "320300",
      name: "贾汪区"
    }, {
      id: "320311",
      pid: "320300",
      name: "泉山区"
    }, {
      id: "320321",
      pid: "320300",
      name: "丰县"
    }, {
      id: "320322",
      pid: "320300",
      name: "沛县"
    }, {
      id: "320323",
      pid: "320300",
      name: "铜山区"
    }, {
      id: "320324",
      pid: "320300",
      name: "睢宁县"
    }, {
      id: "320381",
      pid: "320300",
      name: "新沂市"
    }, {
      id: "320382",
      pid: "320300",
      name: "邳州市"
    }, {
      id: "320383",
      pid: "320300",
      name: "其它区"
    }]
  }, {
    id: "320400",
    pid: "320000",
    name: "常州市",
    children: [{
      id: "320402",
      pid: "320400",
      name: "天宁区"
    }, {
      id: "320404",
      pid: "320400",
      name: "钟楼区"
    }, {
      id: "320405",
      pid: "320400",
      name: "戚墅堰区"
    }, {
      id: "320411",
      pid: "320400",
      name: "新北区"
    }, {
      id: "320412",
      pid: "320400",
      name: "武进区"
    }, {
      id: "320481",
      pid: "320400",
      name: "溧阳市"
    }, {
      id: "320482",
      pid: "320400",
      name: "金坛市"
    }, {
      id: "320483",
      pid: "320400",
      name: "其它区"
    }]
  }, {
    id: "320500",
    pid: "320000",
    name: "苏州市",
    children: [{
      id: "320505",
      pid: "320500",
      name: "虎丘区"
    }, {
      id: "320506",
      pid: "320500",
      name: "吴中区"
    }, {
      id: "320507",
      pid: "320500",
      name: "相城区"
    }, {
      id: "320508",
      pid: "320500",
      name: "姑苏区"
    }, {
      id: "320581",
      pid: "320500",
      name: "常熟市"
    }, {
      id: "320582",
      pid: "320500",
      name: "张家港市"
    }, {
      id: "320583",
      pid: "320500",
      name: "昆山市"
    }, {
      id: "320584",
      pid: "320500",
      name: "吴江区"
    }, {
      id: "320585",
      pid: "320500",
      name: "太仓市"
    }, {
      id: "320596",
      pid: "320500",
      name: "其它区"
    }]
  }, {
    id: "320600",
    pid: "320000",
    name: "南通市",
    children: [{
      id: "320602",
      pid: "320600",
      name: "崇川区"
    }, {
      id: "320611",
      pid: "320600",
      name: "港闸区"
    }, {
      id: "320612",
      pid: "320600",
      name: "通州区"
    }, {
      id: "320621",
      pid: "320600",
      name: "海安县"
    }, {
      id: "320623",
      pid: "320600",
      name: "如东县"
    }, {
      id: "320681",
      pid: "320600",
      name: "启东市"
    }, {
      id: "320682",
      pid: "320600",
      name: "如皋市"
    }, {
      id: "320684",
      pid: "320600",
      name: "海门市"
    }, {
      id: "320694",
      pid: "320600",
      name: "其它区"
    }]
  }, {
    id: "320700",
    pid: "320000",
    name: "连云港市",
    children: [{
      id: "320703",
      pid: "320700",
      name: "连云区"
    }, {
      id: "320705",
      pid: "320700",
      name: "新浦区"
    }, {
      id: "320706",
      pid: "320700",
      name: "海州区"
    }, {
      id: "320721",
      pid: "320700",
      name: "赣榆县"
    }, {
      id: "320722",
      pid: "320700",
      name: "东海县"
    }, {
      id: "320723",
      pid: "320700",
      name: "灌云县"
    }, {
      id: "320724",
      pid: "320700",
      name: "灌南县"
    }, {
      id: "320725",
      pid: "320700",
      name: "其它区"
    }]
  }, {
    id: "320800",
    pid: "320000",
    name: "淮安市",
    children: [{
      id: "320802",
      pid: "320800",
      name: "清河区"
    }, {
      id: "320803",
      pid: "320800",
      name: "淮安区"
    }, {
      id: "320804",
      pid: "320800",
      name: "淮阴区"
    }, {
      id: "320811",
      pid: "320800",
      name: "清浦区"
    }, {
      id: "320826",
      pid: "320800",
      name: "涟水县"
    }, {
      id: "320829",
      pid: "320800",
      name: "洪泽县"
    }, {
      id: "320830",
      pid: "320800",
      name: "盱眙县"
    }, {
      id: "320831",
      pid: "320800",
      name: "金湖县"
    }, {
      id: "320832",
      pid: "320800",
      name: "其它区"
    }]
  }, {
    id: "320900",
    pid: "320000",
    name: "盐城市",
    children: [{
      id: "320902",
      pid: "320900",
      name: "亭湖区"
    }, {
      id: "320903",
      pid: "320900",
      name: "盐都区"
    }, {
      id: "320921",
      pid: "320900",
      name: "响水县"
    }, {
      id: "320922",
      pid: "320900",
      name: "滨海县"
    }, {
      id: "320923",
      pid: "320900",
      name: "阜宁县"
    }, {
      id: "320924",
      pid: "320900",
      name: "射阳县"
    }, {
      id: "320925",
      pid: "320900",
      name: "建湖县"
    }, {
      id: "320981",
      pid: "320900",
      name: "东台市"
    }, {
      id: "320982",
      pid: "320900",
      name: "大丰市"
    }, {
      id: "320983",
      pid: "320900",
      name: "其它区"
    }]
  }, {
    id: "321000",
    pid: "320000",
    name: "扬州市",
    children: [{
      id: "321002",
      pid: "321000",
      name: "广陵区"
    }, {
      id: "321003",
      pid: "321000",
      name: "邗江区"
    }, {
      id: "321023",
      pid: "321000",
      name: "宝应县"
    }, {
      id: "321081",
      pid: "321000",
      name: "仪征市"
    }, {
      id: "321084",
      pid: "321000",
      name: "高邮市"
    }, {
      id: "321088",
      pid: "321000",
      name: "江都区"
    }, {
      id: "321093",
      pid: "321000",
      name: "其它区"
    }]
  }, {
    id: "321100",
    pid: "320000",
    name: "镇江市",
    children: [{
      id: "321102",
      pid: "321100",
      name: "京口区"
    }, {
      id: "321111",
      pid: "321100",
      name: "润州区"
    }, {
      id: "321112",
      pid: "321100",
      name: "丹徒区"
    }, {
      id: "321181",
      pid: "321100",
      name: "丹阳市"
    }, {
      id: "321182",
      pid: "321100",
      name: "扬中市"
    }, {
      id: "321183",
      pid: "321100",
      name: "句容市"
    }, {
      id: "321184",
      pid: "321100",
      name: "其它区"
    }]
  }, {
    id: "321200",
    pid: "320000",
    name: "泰州市",
    children: [{
      id: "321202",
      pid: "321200",
      name: "海陵区"
    }, {
      id: "321203",
      pid: "321200",
      name: "高港区"
    }, {
      id: "321281",
      pid: "321200",
      name: "兴化市"
    }, {
      id: "321282",
      pid: "321200",
      name: "靖江市"
    }, {
      id: "321283",
      pid: "321200",
      name: "泰兴市"
    }, {
      id: "321284",
      pid: "321200",
      name: "姜堰区"
    }, {
      id: "321285",
      pid: "321200",
      name: "其它区"
    }]
  }, {
    id: "321300",
    pid: "320000",
    name: "宿迁市",
    children: [{
      id: "321302",
      pid: "321300",
      name: "宿城区"
    }, {
      id: "321311",
      pid: "321300",
      name: "宿豫区"
    }, {
      id: "321322",
      pid: "321300",
      name: "沭阳县"
    }, {
      id: "321323",
      pid: "321300",
      name: "泗阳县"
    }, {
      id: "321324",
      pid: "321300",
      name: "泗洪县"
    }, {
      id: "321325",
      pid: "321300",
      name: "其它区"
    }]
  }]
}, {
  id: "330000",
  pid: "330000",
  name: "浙江省",
  children: [{
    id: "330100",
    pid: "330000",
    name: "杭州市",
    children: [{
      id: "330102",
      pid: "330100",
      name: "上城区"
    }, {
      id: "330103",
      pid: "330100",
      name: "下城区"
    }, {
      id: "330104",
      pid: "330100",
      name: "江干区"
    }, {
      id: "330105",
      pid: "330100",
      name: "拱墅区"
    }, {
      id: "330106",
      pid: "330100",
      name: "西湖区"
    }, {
      id: "330108",
      pid: "330100",
      name: "滨江区"
    }, {
      id: "330109",
      pid: "330100",
      name: "萧山区"
    }, {
      id: "330110",
      pid: "330100",
      name: "余杭区"
    }, {
      id: "330122",
      pid: "330100",
      name: "桐庐县"
    }, {
      id: "330127",
      pid: "330100",
      name: "淳安县"
    }, {
      id: "330182",
      pid: "330100",
      name: "建德市"
    }, {
      id: "330183",
      pid: "330100",
      name: "富阳市"
    }, {
      id: "330185",
      pid: "330100",
      name: "临安市"
    }, {
      id: "330186",
      pid: "330100",
      name: "其它区"
    }]
  }, {
    id: "330200",
    pid: "330000",
    name: "宁波市",
    children: [{
      id: "330203",
      pid: "330200",
      name: "海曙区"
    }, {
      id: "330204",
      pid: "330200",
      name: "江东区"
    }, {
      id: "330205",
      pid: "330200",
      name: "江北区"
    }, {
      id: "330206",
      pid: "330200",
      name: "北仑区"
    }, {
      id: "330211",
      pid: "330200",
      name: "镇海区"
    }, {
      id: "330212",
      pid: "330200",
      name: "鄞州区"
    }, {
      id: "330225",
      pid: "330200",
      name: "象山县"
    }, {
      id: "330226",
      pid: "330200",
      name: "宁海县"
    }, {
      id: "330281",
      pid: "330200",
      name: "余姚市"
    }, {
      id: "330282",
      pid: "330200",
      name: "慈溪市"
    }, {
      id: "330283",
      pid: "330200",
      name: "奉化市"
    }, {
      id: "330284",
      pid: "330200",
      name: "其它区"
    }]
  }, {
    id: "330300",
    pid: "330000",
    name: "温州市",
    children: [{
      id: "330302",
      pid: "330300",
      name: "鹿城区"
    }, {
      id: "330303",
      pid: "330300",
      name: "龙湾区"
    }, {
      id: "330304",
      pid: "330300",
      name: "瓯海区"
    }, {
      id: "330322",
      pid: "330300",
      name: "洞头县"
    }, {
      id: "330324",
      pid: "330300",
      name: "永嘉县"
    }, {
      id: "330326",
      pid: "330300",
      name: "平阳县"
    }, {
      id: "330327",
      pid: "330300",
      name: "苍南县"
    }, {
      id: "330328",
      pid: "330300",
      name: "文成县"
    }, {
      id: "330329",
      pid: "330300",
      name: "泰顺县"
    }, {
      id: "330381",
      pid: "330300",
      name: "瑞安市"
    }, {
      id: "330382",
      pid: "330300",
      name: "乐清市"
    }, {
      id: "330383",
      pid: "330300",
      name: "其它区"
    }]
  }, {
    id: "330400",
    pid: "330000",
    name: "嘉兴市",
    children: [{
      id: "330402",
      pid: "330400",
      name: "南湖区"
    }, {
      id: "330411",
      pid: "330400",
      name: "秀洲区"
    }, {
      id: "330421",
      pid: "330400",
      name: "嘉善县"
    }, {
      id: "330424",
      pid: "330400",
      name: "海盐县"
    }, {
      id: "330481",
      pid: "330400",
      name: "海宁市"
    }, {
      id: "330482",
      pid: "330400",
      name: "平湖市"
    }, {
      id: "330483",
      pid: "330400",
      name: "桐乡市"
    }, {
      id: "330484",
      pid: "330400",
      name: "其它区"
    }]
  }, {
    id: "330500",
    pid: "330000",
    name: "湖州市",
    children: [{
      id: "330502",
      pid: "330500",
      name: "吴兴区"
    }, {
      id: "330503",
      pid: "330500",
      name: "南浔区"
    }, {
      id: "330521",
      pid: "330500",
      name: "德清县"
    }, {
      id: "330522",
      pid: "330500",
      name: "长兴县"
    }, {
      id: "330523",
      pid: "330500",
      name: "安吉县"
    }, {
      id: "330524",
      pid: "330500",
      name: "其它区"
    }]
  }, {
    id: "330600",
    pid: "330000",
    name: "绍兴市",
    children: [{
      id: "330602",
      pid: "330600",
      name: "越城区"
    }, {
      id: "330621",
      pid: "330600",
      name: "绍兴县"
    }, {
      id: "330624",
      pid: "330600",
      name: "新昌县"
    }, {
      id: "330681",
      pid: "330600",
      name: "诸暨市"
    }, {
      id: "330682",
      pid: "330600",
      name: "上虞市"
    }, {
      id: "330683",
      pid: "330600",
      name: "嵊州市"
    }, {
      id: "330684",
      pid: "330600",
      name: "其它区"
    }]
  }, {
    id: "330700",
    pid: "330000",
    name: "金华市",
    children: [{
      id: "330702",
      pid: "330700",
      name: "婺城区"
    }, {
      id: "330703",
      pid: "330700",
      name: "金东区"
    }, {
      id: "330723",
      pid: "330700",
      name: "武义县"
    }, {
      id: "330726",
      pid: "330700",
      name: "浦江县"
    }, {
      id: "330727",
      pid: "330700",
      name: "磐安县"
    }, {
      id: "330781",
      pid: "330700",
      name: "兰溪市"
    }, {
      id: "330782",
      pid: "330700",
      name: "义乌市"
    }, {
      id: "330783",
      pid: "330700",
      name: "东阳市"
    }, {
      id: "330784",
      pid: "330700",
      name: "永康市"
    }, {
      id: "330785",
      pid: "330700",
      name: "其它区"
    }]
  }, {
    id: "330800",
    pid: "330000",
    name: "衢州市",
    children: [{
      id: "330802",
      pid: "330800",
      name: "柯城区"
    }, {
      id: "330803",
      pid: "330800",
      name: "衢江区"
    }, {
      id: "330822",
      pid: "330800",
      name: "常山县"
    }, {
      id: "330824",
      pid: "330800",
      name: "开化县"
    }, {
      id: "330825",
      pid: "330800",
      name: "龙游县"
    }, {
      id: "330881",
      pid: "330800",
      name: "江山市"
    }, {
      id: "330882",
      pid: "330800",
      name: "其它区"
    }]
  }, {
    id: "330900",
    pid: "330000",
    name: "舟山市",
    children: [{
      id: "330902",
      pid: "330900",
      name: "定海区"
    }, {
      id: "330903",
      pid: "330900",
      name: "普陀区"
    }, {
      id: "330921",
      pid: "330900",
      name: "岱山县"
    }, {
      id: "330922",
      pid: "330900",
      name: "嵊泗县"
    }, {
      id: "330923",
      pid: "330900",
      name: "其它区"
    }]
  }, {
    id: "331000",
    pid: "330000",
    name: "台州市",
    children: [{
      id: "331002",
      pid: "331000",
      name: "椒江区"
    }, {
      id: "331003",
      pid: "331000",
      name: "黄岩区"
    }, {
      id: "331004",
      pid: "331000",
      name: "路桥区"
    }, {
      id: "331021",
      pid: "331000",
      name: "玉环县"
    }, {
      id: "331022",
      pid: "331000",
      name: "三门县"
    }, {
      id: "331023",
      pid: "331000",
      name: "天台县"
    }, {
      id: "331024",
      pid: "331000",
      name: "仙居县"
    }, {
      id: "331081",
      pid: "331000",
      name: "温岭市"
    }, {
      id: "331082",
      pid: "331000",
      name: "临海市"
    }, {
      id: "331083",
      pid: "331000",
      name: "其它区"
    }]
  }, {
    id: "331100",
    pid: "330000",
    name: "丽水市",
    children: [{
      id: "331102",
      pid: "331100",
      name: "莲都区"
    }, {
      id: "331121",
      pid: "331100",
      name: "青田县"
    }, {
      id: "331122",
      pid: "331100",
      name: "缙云县"
    }, {
      id: "331123",
      pid: "331100",
      name: "遂昌县"
    }, {
      id: "331124",
      pid: "331100",
      name: "松阳县"
    }, {
      id: "331125",
      pid: "331100",
      name: "云和县"
    }, {
      id: "331126",
      pid: "331100",
      name: "庆元县"
    }, {
      id: "331127",
      pid: "331100",
      name: "景宁畲族自治县"
    }, {
      id: "331181",
      pid: "331100",
      name: "龙泉市"
    }, {
      id: "331182",
      pid: "331100",
      name: "其它区"
    }]
  }]
}, {
  id: "340000",
  pid: "340000",
  name: "安徽省",
  children: [{
    id: "340100",
    pid: "340000",
    name: "合肥市",
    children: [{
      id: "340102",
      pid: "340100",
      name: "瑶海区"
    }, {
      id: "340103",
      pid: "340100",
      name: "庐阳区"
    }, {
      id: "340104",
      pid: "340100",
      name: "蜀山区"
    }, {
      id: "340111",
      pid: "340100",
      name: "包河区"
    }, {
      id: "340121",
      pid: "340100",
      name: "长丰县"
    }, {
      id: "340122",
      pid: "340100",
      name: "肥东县"
    }, {
      id: "340123",
      pid: "340100",
      name: "肥西县"
    }, {
      id: "340192",
      pid: "340100",
      name: "其它区"
    }]
  }, {
    id: "340200",
    pid: "340000",
    name: "芜湖市",
    children: [{
      id: "340202",
      pid: "340200",
      name: "镜湖区"
    }, {
      id: "340203",
      pid: "340200",
      name: "弋江区"
    }, {
      id: "340207",
      pid: "340200",
      name: "鸠江区"
    }, {
      id: "340208",
      pid: "340200",
      name: "三山区"
    }, {
      id: "340221",
      pid: "340200",
      name: "芜湖县"
    }, {
      id: "340222",
      pid: "340200",
      name: "繁昌县"
    }, {
      id: "340223",
      pid: "340200",
      name: "南陵县"
    }, {
      id: "340224",
      pid: "340200",
      name: "其它区"
    }]
  }, {
    id: "340300",
    pid: "340000",
    name: "蚌埠市",
    children: [{
      id: "340302",
      pid: "340300",
      name: "龙子湖区"
    }, {
      id: "340303",
      pid: "340300",
      name: "蚌山区"
    }, {
      id: "340304",
      pid: "340300",
      name: "禹会区"
    }, {
      id: "340311",
      pid: "340300",
      name: "淮上区"
    }, {
      id: "340321",
      pid: "340300",
      name: "怀远县"
    }, {
      id: "340322",
      pid: "340300",
      name: "五河县"
    }, {
      id: "340323",
      pid: "340300",
      name: "固镇县"
    }, {
      id: "340324",
      pid: "340300",
      name: "其它区"
    }]
  }, {
    id: "340400",
    pid: "340000",
    name: "淮南市",
    children: [{
      id: "340402",
      pid: "340400",
      name: "大通区"
    }, {
      id: "340403",
      pid: "340400",
      name: "田家庵区"
    }, {
      id: "340404",
      pid: "340400",
      name: "谢家集区"
    }, {
      id: "340405",
      pid: "340400",
      name: "八公山区"
    }, {
      id: "340406",
      pid: "340400",
      name: "潘集区"
    }, {
      id: "340421",
      pid: "340400",
      name: "凤台县"
    }, {
      id: "340422",
      pid: "340400",
      name: "其它区"
    }]
  }, {
    id: "340500",
    pid: "340000",
    name: "马鞍山市",
    children: [{
      id: "340503",
      pid: "340500",
      name: "花山区"
    }, {
      id: "340504",
      pid: "340500",
      name: "雨山区"
    }, {
      id: "340506",
      pid: "340500",
      name: "博望区"
    }, {
      id: "340521",
      pid: "340500",
      name: "当涂县"
    }, {
      id: "340522",
      pid: "340500",
      name: "其它区"
    }]
  }, {
    id: "340600",
    pid: "340000",
    name: "淮北市",
    children: [{
      id: "340602",
      pid: "340600",
      name: "杜集区"
    }, {
      id: "340603",
      pid: "340600",
      name: "相山区"
    }, {
      id: "340604",
      pid: "340600",
      name: "烈山区"
    }, {
      id: "340621",
      pid: "340600",
      name: "濉溪县"
    }, {
      id: "340622",
      pid: "340600",
      name: "其它区"
    }]
  }, {
    id: "340700",
    pid: "340000",
    name: "铜陵市",
    children: [{
      id: "340702",
      pid: "340700",
      name: "铜官山区"
    }, {
      id: "340703",
      pid: "340700",
      name: "狮子山区"
    }, {
      id: "340711",
      pid: "340700",
      name: "郊区"
    }, {
      id: "340721",
      pid: "340700",
      name: "铜陵县"
    }, {
      id: "340722",
      pid: "340700",
      name: "其它区"
    }]
  }, {
    id: "340800",
    pid: "340000",
    name: "安庆市",
    children: [{
      id: "340802",
      pid: "340800",
      name: "迎江区"
    }, {
      id: "340803",
      pid: "340800",
      name: "大观区"
    }, {
      id: "340811",
      pid: "340800",
      name: "宜秀区"
    }, {
      id: "340822",
      pid: "340800",
      name: "怀宁县"
    }, {
      id: "340823",
      pid: "340800",
      name: "枞阳县"
    }, {
      id: "340824",
      pid: "340800",
      name: "潜山县"
    }, {
      id: "340825",
      pid: "340800",
      name: "太湖县"
    }, {
      id: "340826",
      pid: "340800",
      name: "宿松县"
    }, {
      id: "340827",
      pid: "340800",
      name: "望江县"
    }, {
      id: "340828",
      pid: "340800",
      name: "岳西县"
    }, {
      id: "340881",
      pid: "340800",
      name: "桐城市"
    }, {
      id: "340882",
      pid: "340800",
      name: "其它区"
    }]
  }, {
    id: "341000",
    pid: "340000",
    name: "黄山市",
    children: [{
      id: "341002",
      pid: "341000",
      name: "屯溪区"
    }, {
      id: "341003",
      pid: "341000",
      name: "黄山区"
    }, {
      id: "341004",
      pid: "341000",
      name: "徽州区"
    }, {
      id: "341021",
      pid: "341000",
      name: "歙县"
    }, {
      id: "341022",
      pid: "341000",
      name: "休宁县"
    }, {
      id: "341023",
      pid: "341000",
      name: "黟县"
    }, {
      id: "341024",
      pid: "341000",
      name: "祁门县"
    }, {
      id: "341025",
      pid: "341000",
      name: "其它区"
    }]
  }, {
    id: "341100",
    pid: "340000",
    name: "滁州市",
    children: [{
      id: "341102",
      pid: "341100",
      name: "琅琊区"
    }, {
      id: "341103",
      pid: "341100",
      name: "南谯区"
    }, {
      id: "341122",
      pid: "341100",
      name: "来安县"
    }, {
      id: "341124",
      pid: "341100",
      name: "全椒县"
    }, {
      id: "341125",
      pid: "341100",
      name: "定远县"
    }, {
      id: "341126",
      pid: "341100",
      name: "凤阳县"
    }, {
      id: "341181",
      pid: "341100",
      name: "天长市"
    }, {
      id: "341182",
      pid: "341100",
      name: "明光市"
    }, {
      id: "341183",
      pid: "341100",
      name: "其它区"
    }]
  }, {
    id: "341200",
    pid: "340000",
    name: "阜阳市",
    children: [{
      id: "341202",
      pid: "341200",
      name: "颍州区"
    }, {
      id: "341203",
      pid: "341200",
      name: "颍东区"
    }, {
      id: "341204",
      pid: "341200",
      name: "颍泉区"
    }, {
      id: "341221",
      pid: "341200",
      name: "临泉县"
    }, {
      id: "341222",
      pid: "341200",
      name: "太和县"
    }, {
      id: "341225",
      pid: "341200",
      name: "阜南县"
    }, {
      id: "341226",
      pid: "341200",
      name: "颍上县"
    }, {
      id: "341282",
      pid: "341200",
      name: "界首市"
    }, {
      id: "341283",
      pid: "341200",
      name: "其它区"
    }]
  }, {
    id: "341300",
    pid: "340000",
    name: "宿州市",
    children: [{
      id: "341302",
      pid: "341300",
      name: "埇桥区"
    }, {
      id: "341321",
      pid: "341300",
      name: "砀山县"
    }, {
      id: "341322",
      pid: "341300",
      name: "萧县"
    }, {
      id: "341323",
      pid: "341300",
      name: "灵璧县"
    }, {
      id: "341324",
      pid: "341300",
      name: "泗县"
    }, {
      id: "341325",
      pid: "341300",
      name: "其它区"
    }]
  }, {
    id: "341400",
    pid: "340000",
    name: "巢湖市",
    children: [{
      id: "341421",
      pid: "341400",
      name: "庐江县"
    }, {
      id: "341422",
      pid: "341400",
      name: "无为县"
    }, {
      id: "341423",
      pid: "341400",
      name: "含山县"
    }, {
      id: "341424",
      pid: "341400",
      name: "和县"
    }]
  }, {
    id: "341500",
    pid: "340000",
    name: "六安市",
    children: [{
      id: "341502",
      pid: "341500",
      name: "金安区"
    }, {
      id: "341503",
      pid: "341500",
      name: "裕安区"
    }, {
      id: "341521",
      pid: "341500",
      name: "寿县"
    }, {
      id: "341522",
      pid: "341500",
      name: "霍邱县"
    }, {
      id: "341523",
      pid: "341500",
      name: "舒城县"
    }, {
      id: "341524",
      pid: "341500",
      name: "金寨县"
    }, {
      id: "341525",
      pid: "341500",
      name: "霍山县"
    }, {
      id: "341526",
      pid: "341500",
      name: "其它区"
    }]
  }, {
    id: "341600",
    pid: "340000",
    name: "亳州市",
    children: [{
      id: "341602",
      pid: "341600",
      name: "谯城区"
    }, {
      id: "341621",
      pid: "341600",
      name: "涡阳县"
    }, {
      id: "341622",
      pid: "341600",
      name: "蒙城县"
    }, {
      id: "341623",
      pid: "341600",
      name: "利辛县"
    }, {
      id: "341624",
      pid: "341600",
      name: "其它区"
    }]
  }, {
    id: "341700",
    pid: "340000",
    name: "池州市",
    children: [{
      id: "341702",
      pid: "341700",
      name: "贵池区"
    }, {
      id: "341721",
      pid: "341700",
      name: "东至县"
    }, {
      id: "341722",
      pid: "341700",
      name: "石台县"
    }, {
      id: "341723",
      pid: "341700",
      name: "青阳县"
    }, {
      id: "341724",
      pid: "341700",
      name: "其它区"
    }]
  }, {
    id: "341800",
    pid: "340000",
    name: "宣城市",
    children: [{
      id: "341802",
      pid: "341800",
      name: "宣州区"
    }, {
      id: "341821",
      pid: "341800",
      name: "郎溪县"
    }, {
      id: "341822",
      pid: "341800",
      name: "广德县"
    }, {
      id: "341823",
      pid: "341800",
      name: "泾县"
    }, {
      id: "341824",
      pid: "341800",
      name: "绩溪县"
    }, {
      id: "341825",
      pid: "341800",
      name: "旌德县"
    }, {
      id: "341881",
      pid: "341800",
      name: "宁国市"
    }, {
      id: "341882",
      pid: "341800",
      name: "其它区"
    }]
  }]
}, {
  id: "350000",
  pid: "350000",
  name: "福建省",
  children: [{
    id: "350100",
    pid: "350000",
    name: "福州市",
    children: [{
      id: "350102",
      pid: "350100",
      name: "鼓楼区"
    }, {
      id: "350103",
      pid: "350100",
      name: "台江区"
    }, {
      id: "350104",
      pid: "350100",
      name: "仓山区"
    }, {
      id: "350105",
      pid: "350100",
      name: "马尾区"
    }, {
      id: "350111",
      pid: "350100",
      name: "晋安区"
    }, {
      id: "350121",
      pid: "350100",
      name: "闽侯县"
    }, {
      id: "350122",
      pid: "350100",
      name: "连江县"
    }, {
      id: "350123",
      pid: "350100",
      name: "罗源县"
    }, {
      id: "350124",
      pid: "350100",
      name: "闽清县"
    }, {
      id: "350125",
      pid: "350100",
      name: "永泰县"
    }, {
      id: "350128",
      pid: "350100",
      name: "平潭县"
    }, {
      id: "350181",
      pid: "350100",
      name: "福清市"
    }, {
      id: "350182",
      pid: "350100",
      name: "长乐市"
    }, {
      id: "350183",
      pid: "350100",
      name: "其它区"
    }]
  }, {
    id: "350200",
    pid: "350000",
    name: "厦门市",
    children: [{
      id: "350203",
      pid: "350200",
      name: "思明区"
    }, {
      id: "350205",
      pid: "350200",
      name: "海沧区"
    }, {
      id: "350206",
      pid: "350200",
      name: "湖里区"
    }, {
      id: "350211",
      pid: "350200",
      name: "集美区"
    }, {
      id: "350212",
      pid: "350200",
      name: "同安区"
    }, {
      id: "350213",
      pid: "350200",
      name: "翔安区"
    }, {
      id: "350214",
      pid: "350200",
      name: "其它区"
    }]
  }, {
    id: "350300",
    pid: "350000",
    name: "莆田市",
    children: [{
      id: "350302",
      pid: "350300",
      name: "城厢区"
    }, {
      id: "350303",
      pid: "350300",
      name: "涵江区"
    }, {
      id: "350304",
      pid: "350300",
      name: "荔城区"
    }, {
      id: "350305",
      pid: "350300",
      name: "秀屿区"
    }, {
      id: "350322",
      pid: "350300",
      name: "仙游县"
    }, {
      id: "350323",
      pid: "350300",
      name: "其它区"
    }]
  }, {
    id: "350400",
    pid: "350000",
    name: "三明市",
    children: [{
      id: "350402",
      pid: "350400",
      name: "梅列区"
    }, {
      id: "350403",
      pid: "350400",
      name: "三元区"
    }, {
      id: "350421",
      pid: "350400",
      name: "明溪县"
    }, {
      id: "350423",
      pid: "350400",
      name: "清流县"
    }, {
      id: "350424",
      pid: "350400",
      name: "宁化县"
    }, {
      id: "350425",
      pid: "350400",
      name: "大田县"
    }, {
      id: "350426",
      pid: "350400",
      name: "尤溪县"
    }, {
      id: "350427",
      pid: "350400",
      name: "沙县"
    }, {
      id: "350428",
      pid: "350400",
      name: "将乐县"
    }, {
      id: "350429",
      pid: "350400",
      name: "泰宁县"
    }, {
      id: "350430",
      pid: "350400",
      name: "建宁县"
    }, {
      id: "350481",
      pid: "350400",
      name: "永安市"
    }, {
      id: "350482",
      pid: "350400",
      name: "其它区"
    }]
  }, {
    id: "350500",
    pid: "350000",
    name: "泉州市",
    children: [{
      id: "350502",
      pid: "350500",
      name: "鲤城区"
    }, {
      id: "350503",
      pid: "350500",
      name: "丰泽区"
    }, {
      id: "350504",
      pid: "350500",
      name: "洛江区"
    }, {
      id: "350505",
      pid: "350500",
      name: "泉港区"
    }, {
      id: "350521",
      pid: "350500",
      name: "惠安县"
    }, {
      id: "350524",
      pid: "350500",
      name: "安溪县"
    }, {
      id: "350525",
      pid: "350500",
      name: "永春县"
    }, {
      id: "350526",
      pid: "350500",
      name: "德化县"
    }, {
      id: "350527",
      pid: "350500",
      name: "金门县"
    }, {
      id: "350581",
      pid: "350500",
      name: "石狮市"
    }, {
      id: "350582",
      pid: "350500",
      name: "晋江市"
    }, {
      id: "350583",
      pid: "350500",
      name: "南安市"
    }, {
      id: "350584",
      pid: "350500",
      name: "其它区"
    }]
  }, {
    id: "350600",
    pid: "350000",
    name: "漳州市",
    children: [{
      id: "350602",
      pid: "350600",
      name: "芗城区"
    }, {
      id: "350603",
      pid: "350600",
      name: "龙文区"
    }, {
      id: "350622",
      pid: "350600",
      name: "云霄县"
    }, {
      id: "350623",
      pid: "350600",
      name: "漳浦县"
    }, {
      id: "350624",
      pid: "350600",
      name: "诏安县"
    }, {
      id: "350625",
      pid: "350600",
      name: "长泰县"
    }, {
      id: "350626",
      pid: "350600",
      name: "东山县"
    }, {
      id: "350627",
      pid: "350600",
      name: "南靖县"
    }, {
      id: "350628",
      pid: "350600",
      name: "平和县"
    }, {
      id: "350629",
      pid: "350600",
      name: "华安县"
    }, {
      id: "350681",
      pid: "350600",
      name: "龙海市"
    }, {
      id: "350682",
      pid: "350600",
      name: "其它区"
    }]
  }, {
    id: "350700",
    pid: "350000",
    name: "南平市",
    children: [{
      id: "350702",
      pid: "350700",
      name: "延平区"
    }, {
      id: "350721",
      pid: "350700",
      name: "顺昌县"
    }, {
      id: "350722",
      pid: "350700",
      name: "浦城县"
    }, {
      id: "350723",
      pid: "350700",
      name: "光泽县"
    }, {
      id: "350724",
      pid: "350700",
      name: "松溪县"
    }, {
      id: "350725",
      pid: "350700",
      name: "政和县"
    }, {
      id: "350781",
      pid: "350700",
      name: "邵武市"
    }, {
      id: "350782",
      pid: "350700",
      name: "武夷山市"
    }, {
      id: "350783",
      pid: "350700",
      name: "建瓯市"
    }, {
      id: "350784",
      pid: "350700",
      name: "建阳市"
    }, {
      id: "350785",
      pid: "350700",
      name: "其它区"
    }]
  }, {
    id: "350800",
    pid: "350000",
    name: "龙岩市",
    children: [{
      id: "350802",
      pid: "350800",
      name: "新罗区"
    }, {
      id: "350821",
      pid: "350800",
      name: "长汀县"
    }, {
      id: "350822",
      pid: "350800",
      name: "永定县"
    }, {
      id: "350823",
      pid: "350800",
      name: "上杭县"
    }, {
      id: "350824",
      pid: "350800",
      name: "武平县"
    }, {
      id: "350825",
      pid: "350800",
      name: "连城县"
    }, {
      id: "350881",
      pid: "350800",
      name: "漳平市"
    }, {
      id: "350882",
      pid: "350800",
      name: "其它区"
    }]
  }, {
    id: "350900",
    pid: "350000",
    name: "宁德市",
    children: [{
      id: "350902",
      pid: "350900",
      name: "蕉城区"
    }, {
      id: "350921",
      pid: "350900",
      name: "霞浦县"
    }, {
      id: "350922",
      pid: "350900",
      name: "古田县"
    }, {
      id: "350923",
      pid: "350900",
      name: "屏南县"
    }, {
      id: "350924",
      pid: "350900",
      name: "寿宁县"
    }, {
      id: "350925",
      pid: "350900",
      name: "周宁县"
    }, {
      id: "350926",
      pid: "350900",
      name: "柘荣县"
    }, {
      id: "350981",
      pid: "350900",
      name: "福安市"
    }, {
      id: "350982",
      pid: "350900",
      name: "福鼎市"
    }, {
      id: "350983",
      pid: "350900",
      name: "其它区"
    }]
  }]
}, {
  id: "360000",
  pid: "360000",
  name: "江西省",
  children: [{
    id: "360100",
    pid: "360000",
    name: "南昌市",
    children: [{
      id: "360102",
      pid: "360100",
      name: "东湖区"
    }, {
      id: "360103",
      pid: "360100",
      name: "西湖区"
    }, {
      id: "360104",
      pid: "360100",
      name: "青云谱区"
    }, {
      id: "360105",
      pid: "360100",
      name: "湾里区"
    }, {
      id: "360111",
      pid: "360100",
      name: "青山湖区"
    }, {
      id: "360121",
      pid: "360100",
      name: "南昌县"
    }, {
      id: "360122",
      pid: "360100",
      name: "新建县"
    }, {
      id: "360123",
      pid: "360100",
      name: "安义县"
    }, {
      id: "360124",
      pid: "360100",
      name: "进贤县"
    }, {
      id: "360128",
      pid: "360100",
      name: "其它区"
    }]
  }, {
    id: "360200",
    pid: "360000",
    name: "景德镇市",
    children: [{
      id: "360202",
      pid: "360200",
      name: "昌江区"
    }, {
      id: "360203",
      pid: "360200",
      name: "珠山区"
    }, {
      id: "360222",
      pid: "360200",
      name: "浮梁县"
    }, {
      id: "360281",
      pid: "360200",
      name: "乐平市"
    }, {
      id: "360282",
      pid: "360200",
      name: "其它区"
    }]
  }, {
    id: "360300",
    pid: "360000",
    name: "萍乡市",
    children: [{
      id: "360302",
      pid: "360300",
      name: "安源区"
    }, {
      id: "360313",
      pid: "360300",
      name: "湘东区"
    }, {
      id: "360321",
      pid: "360300",
      name: "莲花县"
    }, {
      id: "360322",
      pid: "360300",
      name: "上栗县"
    }, {
      id: "360323",
      pid: "360300",
      name: "芦溪县"
    }, {
      id: "360324",
      pid: "360300",
      name: "其它区"
    }]
  }, {
    id: "360400",
    pid: "360000",
    name: "九江市",
    children: [{
      id: "360402",
      pid: "360400",
      name: "庐山区"
    }, {
      id: "360403",
      pid: "360400",
      name: "浔阳区"
    }, {
      id: "360421",
      pid: "360400",
      name: "九江县"
    }, {
      id: "360423",
      pid: "360400",
      name: "武宁县"
    }, {
      id: "360424",
      pid: "360400",
      name: "修水县"
    }, {
      id: "360425",
      pid: "360400",
      name: "永修县"
    }, {
      id: "360426",
      pid: "360400",
      name: "德安县"
    }, {
      id: "360427",
      pid: "360400",
      name: "星子县"
    }, {
      id: "360428",
      pid: "360400",
      name: "都昌县"
    }, {
      id: "360429",
      pid: "360400",
      name: "湖口县"
    }, {
      id: "360430",
      pid: "360400",
      name: "彭泽县"
    }, {
      id: "360481",
      pid: "360400",
      name: "瑞昌市"
    }, {
      id: "360482",
      pid: "360400",
      name: "其它区"
    }, {
      id: "360483",
      pid: "360400",
      name: "共青城市"
    }]
  }, {
    id: "360500",
    pid: "360000",
    name: "新余市",
    children: [{
      id: "360502",
      pid: "360500",
      name: "渝水区"
    }, {
      id: "360521",
      pid: "360500",
      name: "分宜县"
    }, {
      id: "360522",
      pid: "360500",
      name: "其它区"
    }]
  }, {
    id: "360600",
    pid: "360000",
    name: "鹰潭市",
    children: [{
      id: "360602",
      pid: "360600",
      name: "月湖区"
    }, {
      id: "360622",
      pid: "360600",
      name: "余江县"
    }, {
      id: "360681",
      pid: "360600",
      name: "贵溪市"
    }, {
      id: "360682",
      pid: "360600",
      name: "其它区"
    }]
  }, {
    id: "360700",
    pid: "360000",
    name: "赣州市",
    children: [{
      id: "360702",
      pid: "360700",
      name: "章贡区"
    }, {
      id: "360721",
      pid: "360700",
      name: "赣县"
    }, {
      id: "360722",
      pid: "360700",
      name: "信丰县"
    }, {
      id: "360723",
      pid: "360700",
      name: "大余县"
    }, {
      id: "360724",
      pid: "360700",
      name: "上犹县"
    }, {
      id: "360725",
      pid: "360700",
      name: "崇义县"
    }, {
      id: "360726",
      pid: "360700",
      name: "安远县"
    }, {
      id: "360727",
      pid: "360700",
      name: "龙南县"
    }, {
      id: "360728",
      pid: "360700",
      name: "定南县"
    }, {
      id: "360729",
      pid: "360700",
      name: "全南县"
    }, {
      id: "360730",
      pid: "360700",
      name: "宁都县"
    }, {
      id: "360731",
      pid: "360700",
      name: "于都县"
    }, {
      id: "360732",
      pid: "360700",
      name: "兴国县"
    }, {
      id: "360733",
      pid: "360700",
      name: "会昌县"
    }, {
      id: "360734",
      pid: "360700",
      name: "寻乌县"
    }, {
      id: "360735",
      pid: "360700",
      name: "石城县"
    }, {
      id: "360781",
      pid: "360700",
      name: "瑞金市"
    }, {
      id: "360782",
      pid: "360700",
      name: "南康市"
    }, {
      id: "360783",
      pid: "360700",
      name: "其它区"
    }]
  }, {
    id: "360800",
    pid: "360000",
    name: "吉安市",
    children: [{
      id: "360802",
      pid: "360800",
      name: "吉州区"
    }, {
      id: "360803",
      pid: "360800",
      name: "青原区"
    }, {
      id: "360821",
      pid: "360800",
      name: "吉安县"
    }, {
      id: "360822",
      pid: "360800",
      name: "吉水县"
    }, {
      id: "360823",
      pid: "360800",
      name: "峡江县"
    }, {
      id: "360824",
      pid: "360800",
      name: "新干县"
    }, {
      id: "360825",
      pid: "360800",
      name: "永丰县"
    }, {
      id: "360826",
      pid: "360800",
      name: "泰和县"
    }, {
      id: "360827",
      pid: "360800",
      name: "遂川县"
    }, {
      id: "360828",
      pid: "360800",
      name: "万安县"
    }, {
      id: "360829",
      pid: "360800",
      name: "安福县"
    }, {
      id: "360830",
      pid: "360800",
      name: "永新县"
    }, {
      id: "360881",
      pid: "360800",
      name: "井冈山市"
    }, {
      id: "360882",
      pid: "360800",
      name: "其它区"
    }]
  }, {
    id: "360900",
    pid: "360000",
    name: "宜春市",
    children: [{
      id: "360902",
      pid: "360900",
      name: "袁州区"
    }, {
      id: "360921",
      pid: "360900",
      name: "奉新县"
    }, {
      id: "360922",
      pid: "360900",
      name: "万载县"
    }, {
      id: "360923",
      pid: "360900",
      name: "上高县"
    }, {
      id: "360924",
      pid: "360900",
      name: "宜丰县"
    }, {
      id: "360925",
      pid: "360900",
      name: "靖安县"
    }, {
      id: "360926",
      pid: "360900",
      name: "铜鼓县"
    }, {
      id: "360981",
      pid: "360900",
      name: "丰城市"
    }, {
      id: "360982",
      pid: "360900",
      name: "樟树市"
    }, {
      id: "360983",
      pid: "360900",
      name: "高安市"
    }, {
      id: "360984",
      pid: "360900",
      name: "其它区"
    }]
  }, {
    id: "361000",
    pid: "360000",
    name: "抚州市",
    children: [{
      id: "361002",
      pid: "361000",
      name: "临川区"
    }, {
      id: "361021",
      pid: "361000",
      name: "南城县"
    }, {
      id: "361022",
      pid: "361000",
      name: "黎川县"
    }, {
      id: "361023",
      pid: "361000",
      name: "南丰县"
    }, {
      id: "361024",
      pid: "361000",
      name: "崇仁县"
    }, {
      id: "361025",
      pid: "361000",
      name: "乐安县"
    }, {
      id: "361026",
      pid: "361000",
      name: "宜黄县"
    }, {
      id: "361027",
      pid: "361000",
      name: "金溪县"
    }, {
      id: "361028",
      pid: "361000",
      name: "资溪县"
    }, {
      id: "361029",
      pid: "361000",
      name: "东乡县"
    }, {
      id: "361030",
      pid: "361000",
      name: "广昌县"
    }, {
      id: "361031",
      pid: "361000",
      name: "其它区"
    }]
  }, {
    id: "361100",
    pid: "360000",
    name: "上饶市",
    children: [{
      id: "361102",
      pid: "361100",
      name: "信州区"
    }, {
      id: "361121",
      pid: "361100",
      name: "上饶县"
    }, {
      id: "361122",
      pid: "361100",
      name: "广丰县"
    }, {
      id: "361123",
      pid: "361100",
      name: "玉山县"
    }, {
      id: "361124",
      pid: "361100",
      name: "铅山县"
    }, {
      id: "361125",
      pid: "361100",
      name: "横峰县"
    }, {
      id: "361126",
      pid: "361100",
      name: "弋阳县"
    }, {
      id: "361127",
      pid: "361100",
      name: "余干县"
    }, {
      id: "361128",
      pid: "361100",
      name: "鄱阳县"
    }, {
      id: "361129",
      pid: "361100",
      name: "万年县"
    }, {
      id: "361130",
      pid: "361100",
      name: "婺源县"
    }, {
      id: "361181",
      pid: "361100",
      name: "德兴市"
    }, {
      id: "361182",
      pid: "361100",
      name: "其它区"
    }]
  }]
}, {
  id: "370000",
  pid: "370000",
  name: "山东省",
  children: [{
    id: "370100",
    pid: "370000",
    name: "济南市",
    children: [{
      id: "370102",
      pid: "370100",
      name: "历下区"
    }, {
      id: "370103",
      pid: "370100",
      name: "市中区"
    }, {
      id: "370104",
      pid: "370100",
      name: "槐荫区"
    }, {
      id: "370105",
      pid: "370100",
      name: "天桥区"
    }, {
      id: "370112",
      pid: "370100",
      name: "历城区"
    }, {
      id: "370113",
      pid: "370100",
      name: "长清区"
    }, {
      id: "370124",
      pid: "370100",
      name: "平阴县"
    }, {
      id: "370125",
      pid: "370100",
      name: "济阳县"
    }, {
      id: "370126",
      pid: "370100",
      name: "商河县"
    }, {
      id: "370181",
      pid: "370100",
      name: "章丘市"
    }, {
      id: "370182",
      pid: "370100",
      name: "其它区"
    }]
  }, {
    id: "370200",
    pid: "370000",
    name: "青岛市",
    children: [{
      id: "370202",
      pid: "370200",
      name: "市南区"
    }, {
      id: "370203",
      pid: "370200",
      name: "市北区"
    }, {
      id: "370211",
      pid: "370200",
      name: "黄岛区"
    }, {
      id: "370212",
      pid: "370200",
      name: "崂山区"
    }, {
      id: "370213",
      pid: "370200",
      name: "李沧区"
    }, {
      id: "370214",
      pid: "370200",
      name: "城阳区"
    }, {
      id: "370281",
      pid: "370200",
      name: "胶州市"
    }, {
      id: "370282",
      pid: "370200",
      name: "即墨市"
    }, {
      id: "370283",
      pid: "370200",
      name: "平度市"
    }, {
      id: "370285",
      pid: "370200",
      name: "莱西市"
    }, {
      id: "370286",
      pid: "370200",
      name: "其它区"
    }]
  }, {
    id: "370300",
    pid: "370000",
    name: "淄博市",
    children: [{
      id: "370302",
      pid: "370300",
      name: "淄川区"
    }, {
      id: "370303",
      pid: "370300",
      name: "张店区"
    }, {
      id: "370304",
      pid: "370300",
      name: "博山区"
    }, {
      id: "370305",
      pid: "370300",
      name: "临淄区"
    }, {
      id: "370306",
      pid: "370300",
      name: "周村区"
    }, {
      id: "370321",
      pid: "370300",
      name: "桓台县"
    }, {
      id: "370322",
      pid: "370300",
      name: "高青县"
    }, {
      id: "370323",
      pid: "370300",
      name: "沂源县"
    }, {
      id: "370324",
      pid: "370300",
      name: "其它区"
    }]
  }, {
    id: "370400",
    pid: "370000",
    name: "枣庄市",
    children: [{
      id: "370402",
      pid: "370400",
      name: "市中区"
    }, {
      id: "370403",
      pid: "370400",
      name: "薛城区"
    }, {
      id: "370404",
      pid: "370400",
      name: "峄城区"
    }, {
      id: "370405",
      pid: "370400",
      name: "台儿庄区"
    }, {
      id: "370406",
      pid: "370400",
      name: "山亭区"
    }, {
      id: "370481",
      pid: "370400",
      name: "滕州市"
    }, {
      id: "370482",
      pid: "370400",
      name: "其它区"
    }]
  }, {
    id: "370500",
    pid: "370000",
    name: "东营市",
    children: [{
      id: "370502",
      pid: "370500",
      name: "东营区"
    }, {
      id: "370503",
      pid: "370500",
      name: "河口区"
    }, {
      id: "370521",
      pid: "370500",
      name: "垦利县"
    }, {
      id: "370522",
      pid: "370500",
      name: "利津县"
    }, {
      id: "370523",
      pid: "370500",
      name: "广饶县"
    }, {
      id: "370591",
      pid: "370500",
      name: "其它区"
    }]
  }, {
    id: "370600",
    pid: "370000",
    name: "烟台市",
    children: [{
      id: "370602",
      pid: "370600",
      name: "芝罘区"
    }, {
      id: "370611",
      pid: "370600",
      name: "福山区"
    }, {
      id: "370612",
      pid: "370600",
      name: "牟平区"
    }, {
      id: "370613",
      pid: "370600",
      name: "莱山区"
    }, {
      id: "370634",
      pid: "370600",
      name: "长岛县"
    }, {
      id: "370681",
      pid: "370600",
      name: "龙口市"
    }, {
      id: "370682",
      pid: "370600",
      name: "莱阳市"
    }, {
      id: "370683",
      pid: "370600",
      name: "莱州市"
    }, {
      id: "370684",
      pid: "370600",
      name: "蓬莱市"
    }, {
      id: "370685",
      pid: "370600",
      name: "招远市"
    }, {
      id: "370686",
      pid: "370600",
      name: "栖霞市"
    }, {
      id: "370687",
      pid: "370600",
      name: "海阳市"
    }, {
      id: "370688",
      pid: "370600",
      name: "其它区"
    }]
  }, {
    id: "370700",
    pid: "370000",
    name: "潍坊市",
    children: [{
      id: "370702",
      pid: "370700",
      name: "潍城区"
    }, {
      id: "370703",
      pid: "370700",
      name: "寒亭区"
    }, {
      id: "370704",
      pid: "370700",
      name: "坊子区"
    }, {
      id: "370705",
      pid: "370700",
      name: "奎文区"
    }, {
      id: "370724",
      pid: "370700",
      name: "临朐县"
    }, {
      id: "370725",
      pid: "370700",
      name: "昌乐县"
    }, {
      id: "370781",
      pid: "370700",
      name: "青州市"
    }, {
      id: "370782",
      pid: "370700",
      name: "诸城市"
    }, {
      id: "370783",
      pid: "370700",
      name: "寿光市"
    }, {
      id: "370784",
      pid: "370700",
      name: "安丘市"
    }, {
      id: "370785",
      pid: "370700",
      name: "高密市"
    }, {
      id: "370786",
      pid: "370700",
      name: "昌邑市"
    }, {
      id: "370787",
      pid: "370700",
      name: "其它区"
    }]
  }, {
    id: "370800",
    pid: "370000",
    name: "济宁市",
    children: [{
      id: "370802",
      pid: "370800",
      name: "市中区"
    }, {
      id: "370811",
      pid: "370800",
      name: "任城区"
    }, {
      id: "370826",
      pid: "370800",
      name: "微山县"
    }, {
      id: "370827",
      pid: "370800",
      name: "鱼台县"
    }, {
      id: "370828",
      pid: "370800",
      name: "金乡县"
    }, {
      id: "370829",
      pid: "370800",
      name: "嘉祥县"
    }, {
      id: "370830",
      pid: "370800",
      name: "汶上县"
    }, {
      id: "370831",
      pid: "370800",
      name: "泗水县"
    }, {
      id: "370832",
      pid: "370800",
      name: "梁山县"
    }, {
      id: "370881",
      pid: "370800",
      name: "曲阜市"
    }, {
      id: "370882",
      pid: "370800",
      name: "兖州市"
    }, {
      id: "370883",
      pid: "370800",
      name: "邹城市"
    }, {
      id: "370884",
      pid: "370800",
      name: "其它区"
    }]
  }, {
    id: "370900",
    pid: "370000",
    name: "泰安市",
    children: [{
      id: "370902",
      pid: "370900",
      name: "泰山区"
    }, {
      id: "370903",
      pid: "370900",
      name: "岱岳区"
    }, {
      id: "370921",
      pid: "370900",
      name: "宁阳县"
    }, {
      id: "370923",
      pid: "370900",
      name: "东平县"
    }, {
      id: "370982",
      pid: "370900",
      name: "新泰市"
    }, {
      id: "370983",
      pid: "370900",
      name: "肥城市"
    }, {
      id: "370984",
      pid: "370900",
      name: "其它区"
    }]
  }, {
    id: "371000",
    pid: "370000",
    name: "威海市",
    children: [{
      id: "371002",
      pid: "371000",
      name: "环翠区"
    }, {
      id: "371081",
      pid: "371000",
      name: "文登市"
    }, {
      id: "371082",
      pid: "371000",
      name: "荣成市"
    }, {
      id: "371083",
      pid: "371000",
      name: "乳山市"
    }, {
      id: "371084",
      pid: "371000",
      name: "其它区"
    }]
  }, {
    id: "371100",
    pid: "370000",
    name: "日照市",
    children: [{
      id: "371102",
      pid: "371100",
      name: "东港区"
    }, {
      id: "371103",
      pid: "371100",
      name: "岚山区"
    }, {
      id: "371121",
      pid: "371100",
      name: "五莲县"
    }, {
      id: "371122",
      pid: "371100",
      name: "莒县"
    }, {
      id: "371123",
      pid: "371100",
      name: "其它区"
    }]
  }, {
    id: "371200",
    pid: "370000",
    name: "莱芜市",
    children: [{
      id: "371202",
      pid: "371200",
      name: "莱城区"
    }, {
      id: "371203",
      pid: "371200",
      name: "钢城区"
    }, {
      id: "371204",
      pid: "371200",
      name: "其它区"
    }]
  }, {
    id: "371300",
    pid: "370000",
    name: "临沂市",
    children: [{
      id: "371302",
      pid: "371300",
      name: "兰山区"
    }, {
      id: "371311",
      pid: "371300",
      name: "罗庄区"
    }, {
      id: "371312",
      pid: "371300",
      name: "河东区"
    }, {
      id: "371321",
      pid: "371300",
      name: "沂南县"
    }, {
      id: "371322",
      pid: "371300",
      name: "郯城县"
    }, {
      id: "371323",
      pid: "371300",
      name: "沂水县"
    }, {
      id: "371324",
      pid: "371300",
      name: "苍山县"
    }, {
      id: "371325",
      pid: "371300",
      name: "费县"
    }, {
      id: "371326",
      pid: "371300",
      name: "平邑县"
    }, {
      id: "371327",
      pid: "371300",
      name: "莒南县"
    }, {
      id: "371328",
      pid: "371300",
      name: "蒙阴县"
    }, {
      id: "371329",
      pid: "371300",
      name: "临沭县"
    }, {
      id: "371330",
      pid: "371300",
      name: "其它区"
    }]
  }, {
    id: "371400",
    pid: "370000",
    name: "德州市",
    children: [{
      id: "371402",
      pid: "371400",
      name: "德城区"
    }, {
      id: "371421",
      pid: "371400",
      name: "陵县"
    }, {
      id: "371422",
      pid: "371400",
      name: "宁津县"
    }, {
      id: "371423",
      pid: "371400",
      name: "庆云县"
    }, {
      id: "371424",
      pid: "371400",
      name: "临邑县"
    }, {
      id: "371425",
      pid: "371400",
      name: "齐河县"
    }, {
      id: "371426",
      pid: "371400",
      name: "平原县"
    }, {
      id: "371427",
      pid: "371400",
      name: "夏津县"
    }, {
      id: "371428",
      pid: "371400",
      name: "武城县"
    }, {
      id: "371481",
      pid: "371400",
      name: "乐陵市"
    }, {
      id: "371482",
      pid: "371400",
      name: "禹城市"
    }, {
      id: "371483",
      pid: "371400",
      name: "其它区"
    }]
  }, {
    id: "371500",
    pid: "370000",
    name: "聊城市",
    children: [{
      id: "371502",
      pid: "371500",
      name: "东昌府区"
    }, {
      id: "371521",
      pid: "371500",
      name: "阳谷县"
    }, {
      id: "371522",
      pid: "371500",
      name: "莘县"
    }, {
      id: "371523",
      pid: "371500",
      name: "茌平县"
    }, {
      id: "371524",
      pid: "371500",
      name: "东阿县"
    }, {
      id: "371525",
      pid: "371500",
      name: "冠县"
    }, {
      id: "371526",
      pid: "371500",
      name: "高唐县"
    }, {
      id: "371581",
      pid: "371500",
      name: "临清市"
    }, {
      id: "371582",
      pid: "371500",
      name: "其它区"
    }]
  }, {
    id: "371600",
    pid: "370000",
    name: "滨州市",
    children: [{
      id: "371602",
      pid: "371600",
      name: "滨城区"
    }, {
      id: "371621",
      pid: "371600",
      name: "惠民县"
    }, {
      id: "371622",
      pid: "371600",
      name: "阳信县"
    }, {
      id: "371623",
      pid: "371600",
      name: "无棣县"
    }, {
      id: "371624",
      pid: "371600",
      name: "沾化县"
    }, {
      id: "371625",
      pid: "371600",
      name: "博兴县"
    }, {
      id: "371626",
      pid: "371600",
      name: "邹平县"
    }, {
      id: "371627",
      pid: "371600",
      name: "其它区"
    }]
  }, {
    id: "371700",
    pid: "370000",
    name: "菏泽市",
    children: [{
      id: "371702",
      pid: "371700",
      name: "牡丹区"
    }, {
      id: "371721",
      pid: "371700",
      name: "曹县"
    }, {
      id: "371722",
      pid: "371700",
      name: "单县"
    }, {
      id: "371723",
      pid: "371700",
      name: "成武县"
    }, {
      id: "371724",
      pid: "371700",
      name: "巨野县"
    }, {
      id: "371725",
      pid: "371700",
      name: "郓城县"
    }, {
      id: "371726",
      pid: "371700",
      name: "鄄城县"
    }, {
      id: "371727",
      pid: "371700",
      name: "定陶县"
    }, {
      id: "371728",
      pid: "371700",
      name: "东明县"
    }, {
      id: "371729",
      pid: "371700",
      name: "其它区"
    }]
  }]
}, {
  id: "410000",
  pid: "410000",
  name: "河南省",
  children: [{
    id: "410100",
    pid: "410000",
    name: "郑州市",
    children: [{
      id: "410102",
      pid: "410100",
      name: "中原区"
    }, {
      id: "410103",
      pid: "410100",
      name: "二七区"
    }, {
      id: "410104",
      pid: "410100",
      name: "管城回族区"
    }, {
      id: "410105",
      pid: "410100",
      name: "金水区"
    }, {
      id: "410106",
      pid: "410100",
      name: "上街区"
    }, {
      id: "410108",
      pid: "410100",
      name: "惠济区"
    }, {
      id: "410122",
      pid: "410100",
      name: "中牟县"
    }, {
      id: "410181",
      pid: "410100",
      name: "巩义市"
    }, {
      id: "410182",
      pid: "410100",
      name: "荥阳市"
    }, {
      id: "410183",
      pid: "410100",
      name: "新密市"
    }, {
      id: "410184",
      pid: "410100",
      name: "新郑市"
    }, {
      id: "410185",
      pid: "410100",
      name: "登封市"
    }, {
      id: "410188",
      pid: "410100",
      name: "其它区"
    }]
  }, {
    id: "410200",
    pid: "410000",
    name: "开封市",
    children: [{
      id: "410202",
      pid: "410200",
      name: "龙亭区"
    }, {
      id: "410203",
      pid: "410200",
      name: "顺河回族区"
    }, {
      id: "410204",
      pid: "410200",
      name: "鼓楼区"
    }, {
      id: "410205",
      pid: "410200",
      name: "禹王台区"
    }, {
      id: "410211",
      pid: "410200",
      name: "金明区"
    }, {
      id: "410221",
      pid: "410200",
      name: "杞县"
    }, {
      id: "410222",
      pid: "410200",
      name: "通许县"
    }, {
      id: "410223",
      pid: "410200",
      name: "尉氏县"
    }, {
      id: "410224",
      pid: "410200",
      name: "开封县"
    }, {
      id: "410225",
      pid: "410200",
      name: "兰考县"
    }, {
      id: "410226",
      pid: "410200",
      name: "其它区"
    }]
  }, {
    id: "410300",
    pid: "410000",
    name: "洛阳市",
    children: [{
      id: "410302",
      pid: "410300",
      name: "老城区"
    }, {
      id: "410303",
      pid: "410300",
      name: "西工区"
    }, {
      id: "410304",
      pid: "410300",
      name: "瀍河回族区"
    }, {
      id: "410305",
      pid: "410300",
      name: "涧西区"
    }, {
      id: "410306",
      pid: "410300",
      name: "吉利区"
    }, {
      id: "410307",
      pid: "410300",
      name: "洛龙区"
    }, {
      id: "410322",
      pid: "410300",
      name: "孟津县"
    }, {
      id: "410323",
      pid: "410300",
      name: "新安县"
    }, {
      id: "410324",
      pid: "410300",
      name: "栾川县"
    }, {
      id: "410325",
      pid: "410300",
      name: "嵩县"
    }, {
      id: "410326",
      pid: "410300",
      name: "汝阳县"
    }, {
      id: "410327",
      pid: "410300",
      name: "宜阳县"
    }, {
      id: "410328",
      pid: "410300",
      name: "洛宁县"
    }, {
      id: "410329",
      pid: "410300",
      name: "伊川县"
    }, {
      id: "410381",
      pid: "410300",
      name: "偃师市"
    }]
  }, {
    id: "410400",
    pid: "410000",
    name: "平顶山市",
    children: [{
      id: "410402",
      pid: "410400",
      name: "新华区"
    }, {
      id: "410403",
      pid: "410400",
      name: "卫东区"
    }, {
      id: "410404",
      pid: "410400",
      name: "石龙区"
    }, {
      id: "410411",
      pid: "410400",
      name: "湛河区"
    }, {
      id: "410421",
      pid: "410400",
      name: "宝丰县"
    }, {
      id: "410422",
      pid: "410400",
      name: "叶县"
    }, {
      id: "410423",
      pid: "410400",
      name: "鲁山县"
    }, {
      id: "410425",
      pid: "410400",
      name: "郏县"
    }, {
      id: "410481",
      pid: "410400",
      name: "舞钢市"
    }, {
      id: "410482",
      pid: "410400",
      name: "汝州市"
    }, {
      id: "410483",
      pid: "410400",
      name: "其它区"
    }]
  }, {
    id: "410500",
    pid: "410000",
    name: "安阳市",
    children: [{
      id: "410502",
      pid: "410500",
      name: "文峰区"
    }, {
      id: "410503",
      pid: "410500",
      name: "北关区"
    }, {
      id: "410505",
      pid: "410500",
      name: "殷都区"
    }, {
      id: "410506",
      pid: "410500",
      name: "龙安区"
    }, {
      id: "410522",
      pid: "410500",
      name: "安阳县"
    }, {
      id: "410523",
      pid: "410500",
      name: "汤阴县"
    }, {
      id: "410526",
      pid: "410500",
      name: "滑县"
    }, {
      id: "410527",
      pid: "410500",
      name: "内黄县"
    }, {
      id: "410581",
      pid: "410500",
      name: "林州市"
    }, {
      id: "410582",
      pid: "410500",
      name: "其它区"
    }]
  }, {
    id: "410600",
    pid: "410000",
    name: "鹤壁市",
    children: [{
      id: "410602",
      pid: "410600",
      name: "鹤山区"
    }, {
      id: "410603",
      pid: "410600",
      name: "山城区"
    }, {
      id: "410611",
      pid: "410600",
      name: "淇滨区"
    }, {
      id: "410621",
      pid: "410600",
      name: "浚县"
    }, {
      id: "410622",
      pid: "410600",
      name: "淇县"
    }, {
      id: "410623",
      pid: "410600",
      name: "其它区"
    }]
  }, {
    id: "410700",
    pid: "410000",
    name: "新乡市",
    children: [{
      id: "410702",
      pid: "410700",
      name: "红旗区"
    }, {
      id: "410703",
      pid: "410700",
      name: "卫滨区"
    }, {
      id: "410704",
      pid: "410700",
      name: "凤泉区"
    }, {
      id: "410711",
      pid: "410700",
      name: "牧野区"
    }, {
      id: "410721",
      pid: "410700",
      name: "新乡县"
    }, {
      id: "410724",
      pid: "410700",
      name: "获嘉县"
    }, {
      id: "410725",
      pid: "410700",
      name: "原阳县"
    }, {
      id: "410726",
      pid: "410700",
      name: "延津县"
    }, {
      id: "410727",
      pid: "410700",
      name: "封丘县"
    }, {
      id: "410728",
      pid: "410700",
      name: "长垣县"
    }, {
      id: "410781",
      pid: "410700",
      name: "卫辉市"
    }, {
      id: "410782",
      pid: "410700",
      name: "辉县市"
    }, {
      id: "410783",
      pid: "410700",
      name: "其它区"
    }]
  }, {
    id: "410800",
    pid: "410000",
    name: "焦作市",
    children: [{
      id: "410802",
      pid: "410800",
      name: "解放区"
    }, {
      id: "410803",
      pid: "410800",
      name: "中站区"
    }, {
      id: "410804",
      pid: "410800",
      name: "马村区"
    }, {
      id: "410811",
      pid: "410800",
      name: "山阳区"
    }, {
      id: "410821",
      pid: "410800",
      name: "修武县"
    }, {
      id: "410822",
      pid: "410800",
      name: "博爱县"
    }, {
      id: "410823",
      pid: "410800",
      name: "武陟县"
    }, {
      id: "410825",
      pid: "410800",
      name: "温县"
    }, {
      id: "410881",
      pid: "410800",
      name: "济源市"
    }, {
      id: "410882",
      pid: "410800",
      name: "沁阳市"
    }, {
      id: "410883",
      pid: "410800",
      name: "孟州市"
    }, {
      id: "410884",
      pid: "410800",
      name: "其它区"
    }]
  }, {
    id: "410900",
    pid: "410000",
    name: "濮阳市",
    children: [{
      id: "410902",
      pid: "410900",
      name: "华龙区"
    }, {
      id: "410922",
      pid: "410900",
      name: "清丰县"
    }, {
      id: "410923",
      pid: "410900",
      name: "南乐县"
    }, {
      id: "410926",
      pid: "410900",
      name: "范县"
    }, {
      id: "410927",
      pid: "410900",
      name: "台前县"
    }, {
      id: "410928",
      pid: "410900",
      name: "濮阳县"
    }, {
      id: "410929",
      pid: "410900",
      name: "其它区"
    }]
  }, {
    id: "411000",
    pid: "410000",
    name: "许昌市",
    children: [{
      id: "411002",
      pid: "411000",
      name: "魏都区"
    }, {
      id: "411023",
      pid: "411000",
      name: "许昌县"
    }, {
      id: "411024",
      pid: "411000",
      name: "鄢陵县"
    }, {
      id: "411025",
      pid: "411000",
      name: "襄城县"
    }, {
      id: "411081",
      pid: "411000",
      name: "禹州市"
    }, {
      id: "411082",
      pid: "411000",
      name: "长葛市"
    }, {
      id: "411083",
      pid: "411000",
      name: "其它区"
    }]
  }, {
    id: "411100",
    pid: "410000",
    name: "漯河市",
    children: [{
      id: "411102",
      pid: "411100",
      name: "源汇区"
    }, {
      id: "411103",
      pid: "411100",
      name: "郾城区"
    }, {
      id: "411104",
      pid: "411100",
      name: "召陵区"
    }, {
      id: "411121",
      pid: "411100",
      name: "舞阳县"
    }, {
      id: "411122",
      pid: "411100",
      name: "临颍县"
    }, {
      id: "411123",
      pid: "411100",
      name: "其它区"
    }]
  }, {
    id: "411200",
    pid: "410000",
    name: "三门峡市",
    children: [{
      id: "411202",
      pid: "411200",
      name: "湖滨区"
    }, {
      id: "411221",
      pid: "411200",
      name: "渑池县"
    }, {
      id: "411222",
      pid: "411200",
      name: "陕县"
    }, {
      id: "411224",
      pid: "411200",
      name: "卢氏县"
    }, {
      id: "411281",
      pid: "411200",
      name: "义马市"
    }, {
      id: "411282",
      pid: "411200",
      name: "灵宝市"
    }, {
      id: "411283",
      pid: "411200",
      name: "其它区"
    }]
  }, {
    id: "411300",
    pid: "410000",
    name: "南阳市",
    children: [{
      id: "411302",
      pid: "411300",
      name: "宛城区"
    }, {
      id: "411303",
      pid: "411300",
      name: "卧龙区"
    }, {
      id: "411321",
      pid: "411300",
      name: "南召县"
    }, {
      id: "411322",
      pid: "411300",
      name: "方城县"
    }, {
      id: "411323",
      pid: "411300",
      name: "西峡县"
    }, {
      id: "411324",
      pid: "411300",
      name: "镇平县"
    }, {
      id: "411325",
      pid: "411300",
      name: "内乡县"
    }, {
      id: "411326",
      pid: "411300",
      name: "淅川县"
    }, {
      id: "411327",
      pid: "411300",
      name: "社旗县"
    }, {
      id: "411328",
      pid: "411300",
      name: "唐河县"
    }, {
      id: "411329",
      pid: "411300",
      name: "新野县"
    }, {
      id: "411330",
      pid: "411300",
      name: "桐柏县"
    }, {
      id: "411381",
      pid: "411300",
      name: "邓州市"
    }, {
      id: "411382",
      pid: "411300",
      name: "其它区"
    }]
  }, {
    id: "411400",
    pid: "410000",
    name: "商丘市",
    children: [{
      id: "411402",
      pid: "411400",
      name: "梁园区"
    }, {
      id: "411403",
      pid: "411400",
      name: "睢阳区"
    }, {
      id: "411421",
      pid: "411400",
      name: "民权县"
    }, {
      id: "411422",
      pid: "411400",
      name: "睢县"
    }, {
      id: "411423",
      pid: "411400",
      name: "宁陵县"
    }, {
      id: "411424",
      pid: "411400",
      name: "柘城县"
    }, {
      id: "411425",
      pid: "411400",
      name: "虞城县"
    }, {
      id: "411426",
      pid: "411400",
      name: "夏邑县"
    }, {
      id: "411481",
      pid: "411400",
      name: "永城市"
    }, {
      id: "411482",
      pid: "411400",
      name: "其它区"
    }]
  }, {
    id: "411500",
    pid: "410000",
    name: "信阳市",
    children: [{
      id: "411502",
      pid: "411500",
      name: "浉河区"
    }, {
      id: "411503",
      pid: "411500",
      name: "平桥区"
    }, {
      id: "411521",
      pid: "411500",
      name: "罗山县"
    }, {
      id: "411522",
      pid: "411500",
      name: "光山县"
    }, {
      id: "411523",
      pid: "411500",
      name: "新县"
    }, {
      id: "411524",
      pid: "411500",
      name: "商城县"
    }, {
      id: "411525",
      pid: "411500",
      name: "固始县"
    }, {
      id: "411526",
      pid: "411500",
      name: "潢川县"
    }, {
      id: "411527",
      pid: "411500",
      name: "淮滨县"
    }, {
      id: "411528",
      pid: "411500",
      name: "息县"
    }, {
      id: "411529",
      pid: "411500",
      name: "其它区"
    }]
  }, {
    id: "411600",
    pid: "410000",
    name: "周口市",
    children: [{
      id: "411602",
      pid: "411600",
      name: "川汇区"
    }, {
      id: "411621",
      pid: "411600",
      name: "扶沟县"
    }, {
      id: "411622",
      pid: "411600",
      name: "西华县"
    }, {
      id: "411623",
      pid: "411600",
      name: "商水县"
    }, {
      id: "411624",
      pid: "411600",
      name: "沈丘县"
    }, {
      id: "411625",
      pid: "411600",
      name: "郸城县"
    }, {
      id: "411626",
      pid: "411600",
      name: "淮阳县"
    }, {
      id: "411627",
      pid: "411600",
      name: "太康县"
    }, {
      id: "411628",
      pid: "411600",
      name: "鹿邑县"
    }, {
      id: "411681",
      pid: "411600",
      name: "项城市"
    }, {
      id: "411682",
      pid: "411600",
      name: "其它区"
    }]
  }, {
    id: "411700",
    pid: "410000",
    name: "驻马店市",
    children: [{
      id: "411702",
      pid: "411700",
      name: "驿城区"
    }, {
      id: "411721",
      pid: "411700",
      name: "西平县"
    }, {
      id: "411722",
      pid: "411700",
      name: "上蔡县"
    }, {
      id: "411723",
      pid: "411700",
      name: "平舆县"
    }, {
      id: "411724",
      pid: "411700",
      name: "正阳县"
    }, {
      id: "411725",
      pid: "411700",
      name: "确山县"
    }, {
      id: "411726",
      pid: "411700",
      name: "泌阳县"
    }, {
      id: "411727",
      pid: "411700",
      name: "汝南县"
    }, {
      id: "411728",
      pid: "411700",
      name: "遂平县"
    }, {
      id: "411729",
      pid: "411700",
      name: "新蔡县"
    }, {
      id: "411730",
      pid: "411700",
      name: "其它区"
    }]
  }]
}, {
  id: "420000",
  pid: "420000",
  name: "湖北省",
  children: [{
    id: "420100",
    pid: "420000",
    name: "武汉市",
    children: [{
      id: "420102",
      pid: "420100",
      name: "江岸区"
    }, {
      id: "420103",
      pid: "420100",
      name: "江汉区"
    }, {
      id: "420104",
      pid: "420100",
      name: "硚口区"
    }, {
      id: "420105",
      pid: "420100",
      name: "汉阳区"
    }, {
      id: "420106",
      pid: "420100",
      name: "武昌区"
    }, {
      id: "420107",
      pid: "420100",
      name: "青山区"
    }, {
      id: "420111",
      pid: "420100",
      name: "洪山区"
    }, {
      id: "420112",
      pid: "420100",
      name: "东西湖区"
    }, {
      id: "420113",
      pid: "420100",
      name: "汉南区"
    }, {
      id: "420114",
      pid: "420100",
      name: "蔡甸区"
    }, {
      id: "420115",
      pid: "420100",
      name: "江夏区"
    }, {
      id: "420116",
      pid: "420100",
      name: "黄陂区"
    }, {
      id: "420117",
      pid: "420100",
      name: "新洲区"
    }, {
      id: "420118",
      pid: "420100",
      name: "其它区"
    }]
  }, {
    id: "420200",
    pid: "420000",
    name: "黄石市",
    children: [{
      id: "420202",
      pid: "420200",
      name: "黄石港区"
    }, {
      id: "420203",
      pid: "420200",
      name: "西塞山区"
    }, {
      id: "420204",
      pid: "420200",
      name: "下陆区"
    }, {
      id: "420205",
      pid: "420200",
      name: "铁山区"
    }, {
      id: "420222",
      pid: "420200",
      name: "阳新县"
    }, {
      id: "420281",
      pid: "420200",
      name: "大冶市"
    }, {
      id: "420282",
      pid: "420200",
      name: "其它区"
    }]
  }, {
    id: "420300",
    pid: "420000",
    name: "十堰市",
    children: [{
      id: "420302",
      pid: "420300",
      name: "茅箭区"
    }, {
      id: "420303",
      pid: "420300",
      name: "张湾区"
    }, {
      id: "420321",
      pid: "420300",
      name: "郧县"
    }, {
      id: "420322",
      pid: "420300",
      name: "郧西县"
    }, {
      id: "420323",
      pid: "420300",
      name: "竹山县"
    }, {
      id: "420324",
      pid: "420300",
      name: "竹溪县"
    }, {
      id: "420325",
      pid: "420300",
      name: "房县"
    }, {
      id: "420381",
      pid: "420300",
      name: "丹江口市"
    }, {
      id: "420383",
      pid: "420300",
      name: "其它区"
    }]
  }, {
    id: "420500",
    pid: "420000",
    name: "宜昌市",
    children: [{
      id: "420502",
      pid: "420500",
      name: "西陵区"
    }, {
      id: "420503",
      pid: "420500",
      name: "伍家岗区"
    }, {
      id: "420504",
      pid: "420500",
      name: "点军区"
    }, {
      id: "420505",
      pid: "420500",
      name: "猇亭区"
    }, {
      id: "420506",
      pid: "420500",
      name: "夷陵区"
    }, {
      id: "420525",
      pid: "420500",
      name: "远安县"
    }, {
      id: "420526",
      pid: "420500",
      name: "兴山县"
    }, {
      id: "420527",
      pid: "420500",
      name: "秭归县"
    }, {
      id: "420528",
      pid: "420500",
      name: "长阳土家族自治县"
    }, {
      id: "420529",
      pid: "420500",
      name: "五峰土家族自治县"
    }, {
      id: "420581",
      pid: "420500",
      name: "宜都市"
    }, {
      id: "420582",
      pid: "420500",
      name: "当阳市"
    }, {
      id: "420583",
      pid: "420500",
      name: "枝江市"
    }, {
      id: "420584",
      pid: "420500",
      name: "其它区"
    }]
  }, {
    id: "420600",
    pid: "420000",
    name: "襄阳市",
    children: [{
      id: "420602",
      pid: "420600",
      name: "襄城区"
    }, {
      id: "420606",
      pid: "420600",
      name: "樊城区"
    }, {
      id: "420607",
      pid: "420600",
      name: "襄州区"
    }, {
      id: "420624",
      pid: "420600",
      name: "南漳县"
    }, {
      id: "420625",
      pid: "420600",
      name: "谷城县"
    }, {
      id: "420626",
      pid: "420600",
      name: "保康县"
    }, {
      id: "420682",
      pid: "420600",
      name: "老河口市"
    }, {
      id: "420683",
      pid: "420600",
      name: "枣阳市"
    }, {
      id: "420684",
      pid: "420600",
      name: "宜城市"
    }, {
      id: "420685",
      pid: "420600",
      name: "其它区"
    }]
  }, {
    id: "420700",
    pid: "420000",
    name: "鄂州市",
    children: [{
      id: "420702",
      pid: "420700",
      name: "梁子湖区"
    }, {
      id: "420703",
      pid: "420700",
      name: "华容区"
    }, {
      id: "420704",
      pid: "420700",
      name: "鄂城区"
    }, {
      id: "420705",
      pid: "420700",
      name: "其它区"
    }]
  }, {
    id: "420800",
    pid: "420000",
    name: "荆门市",
    children: [{
      id: "420802",
      pid: "420800",
      name: "东宝区"
    }, {
      id: "420804",
      pid: "420800",
      name: "掇刀区"
    }, {
      id: "420821",
      pid: "420800",
      name: "京山县"
    }, {
      id: "420822",
      pid: "420800",
      name: "沙洋县"
    }, {
      id: "420881",
      pid: "420800",
      name: "钟祥市"
    }, {
      id: "420882",
      pid: "420800",
      name: "其它区"
    }]
  }, {
    id: "420900",
    pid: "420000",
    name: "孝感市",
    children: [{
      id: "420902",
      pid: "420900",
      name: "孝南区"
    }, {
      id: "420921",
      pid: "420900",
      name: "孝昌县"
    }, {
      id: "420922",
      pid: "420900",
      name: "大悟县"
    }, {
      id: "420923",
      pid: "420900",
      name: "云梦县"
    }, {
      id: "420981",
      pid: "420900",
      name: "应城市"
    }, {
      id: "420982",
      pid: "420900",
      name: "安陆市"
    }, {
      id: "420984",
      pid: "420900",
      name: "汉川市"
    }, {
      id: "420985",
      pid: "420900",
      name: "其它区"
    }]
  }, {
    id: "421000",
    pid: "420000",
    name: "荆州市",
    children: [{
      id: "421002",
      pid: "421000",
      name: "沙市区"
    }, {
      id: "421003",
      pid: "421000",
      name: "荆州区"
    }, {
      id: "421022",
      pid: "421000",
      name: "公安县"
    }, {
      id: "421023",
      pid: "421000",
      name: "监利县"
    }, {
      id: "421024",
      pid: "421000",
      name: "江陵县"
    }, {
      id: "421081",
      pid: "421000",
      name: "石首市"
    }, {
      id: "421083",
      pid: "421000",
      name: "洪湖市"
    }, {
      id: "421087",
      pid: "421000",
      name: "松滋市"
    }, {
      id: "421088",
      pid: "421000",
      name: "其它区"
    }]
  }, {
    id: "421100",
    pid: "420000",
    name: "黄冈市",
    children: [{
      id: "421102",
      pid: "421100",
      name: "黄州区"
    }, {
      id: "421121",
      pid: "421100",
      name: "团风县"
    }, {
      id: "421122",
      pid: "421100",
      name: "红安县"
    }, {
      id: "421123",
      pid: "421100",
      name: "罗田县"
    }, {
      id: "421124",
      pid: "421100",
      name: "英山县"
    }, {
      id: "421125",
      pid: "421100",
      name: "浠水县"
    }, {
      id: "421126",
      pid: "421100",
      name: "蕲春县"
    }, {
      id: "421127",
      pid: "421100",
      name: "黄梅县"
    }, {
      id: "421181",
      pid: "421100",
      name: "麻城市"
    }, {
      id: "421182",
      pid: "421100",
      name: "武穴市"
    }, {
      id: "421183",
      pid: "421100",
      name: "其它区"
    }]
  }, {
    id: "421200",
    pid: "420000",
    name: "咸宁市",
    children: [{
      id: "421202",
      pid: "421200",
      name: "咸安区"
    }, {
      id: "421221",
      pid: "421200",
      name: "嘉鱼县"
    }, {
      id: "421222",
      pid: "421200",
      name: "通城县"
    }, {
      id: "421223",
      pid: "421200",
      name: "崇阳县"
    }, {
      id: "421224",
      pid: "421200",
      name: "通山县"
    }, {
      id: "421281",
      pid: "421200",
      name: "赤壁市"
    }, {
      id: "421283",
      pid: "421200",
      name: "其它区"
    }]
  }, {
    id: "421300",
    pid: "420000",
    name: "随州市",
    children: [{
      id: "421302",
      pid: "421300",
      name: "曾都区"
    }, {
      id: "421321",
      pid: "421300",
      name: "随县"
    }, {
      id: "421381",
      pid: "421300",
      name: "广水市"
    }, {
      id: "421382",
      pid: "421300",
      name: "其它区"
    }]
  }, {
    id: "422800",
    pid: "420000",
    name: "恩施土家族苗族自治州",
    children: [{
      id: "422801",
      pid: "422800",
      name: "恩施市"
    }, {
      id: "422802",
      pid: "422800",
      name: "利川市"
    }, {
      id: "422822",
      pid: "422800",
      name: "建始县"
    }, {
      id: "422823",
      pid: "422800",
      name: "巴东县"
    }, {
      id: "422825",
      pid: "422800",
      name: "宣恩县"
    }, {
      id: "422826",
      pid: "422800",
      name: "咸丰县"
    }, {
      id: "422827",
      pid: "422800",
      name: "来凤县"
    }, {
      id: "422828",
      pid: "422800",
      name: "鹤峰县"
    }, {
      id: "422829",
      pid: "422800",
      name: "其它区"
    }]
  }, {
    id: "429004",
    pid: "420000",
    name: "仙桃市"
  }, {
    id: "429005",
    pid: "420000",
    name: "潜江市"
  }, {
    id: "429006",
    pid: "420000",
    name: "天门市"
  }, {
    id: "429021",
    pid: "420000",
    name: "神农架林区"
  }]
}, {
  id: "430000",
  pid: "430000",
  name: "湖南省",
  children: [{
    id: "430100",
    pid: "430000",
    name: "长沙市",
    children: [{
      id: "430102",
      pid: "430100",
      name: "芙蓉区"
    }, {
      id: "430103",
      pid: "430100",
      name: "天心区"
    }, {
      id: "430104",
      pid: "430100",
      name: "岳麓区"
    }, {
      id: "430105",
      pid: "430100",
      name: "开福区"
    }, {
      id: "430111",
      pid: "430100",
      name: "雨花区"
    }, {
      id: "430121",
      pid: "430100",
      name: "长沙县"
    }, {
      id: "430122",
      pid: "430100",
      name: "望城区"
    }, {
      id: "430124",
      pid: "430100",
      name: "宁乡县"
    }, {
      id: "430181",
      pid: "430100",
      name: "浏阳市"
    }, {
      id: "430182",
      pid: "430100",
      name: "其它区"
    }]
  }, {
    id: "430200",
    pid: "430000",
    name: "株洲市",
    children: [{
      id: "430202",
      pid: "430200",
      name: "荷塘区"
    }, {
      id: "430203",
      pid: "430200",
      name: "芦淞区"
    }, {
      id: "430204",
      pid: "430200",
      name: "石峰区"
    }, {
      id: "430211",
      pid: "430200",
      name: "天元区"
    }, {
      id: "430221",
      pid: "430200",
      name: "株洲县"
    }, {
      id: "430223",
      pid: "430200",
      name: "攸县"
    }, {
      id: "430224",
      pid: "430200",
      name: "茶陵县"
    }, {
      id: "430225",
      pid: "430200",
      name: "炎陵县"
    }, {
      id: "430281",
      pid: "430200",
      name: "醴陵市"
    }, {
      id: "430282",
      pid: "430200",
      name: "其它区"
    }]
  }, {
    id: "430300",
    pid: "430000",
    name: "湘潭市",
    children: [{
      id: "430302",
      pid: "430300",
      name: "雨湖区"
    }, {
      id: "430304",
      pid: "430300",
      name: "岳塘区"
    }, {
      id: "430321",
      pid: "430300",
      name: "湘潭县"
    }, {
      id: "430381",
      pid: "430300",
      name: "湘乡市"
    }, {
      id: "430382",
      pid: "430300",
      name: "韶山市"
    }, {
      id: "430383",
      pid: "430300",
      name: "其它区"
    }]
  }, {
    id: "430400",
    pid: "430000",
    name: "衡阳市",
    children: [{
      id: "430405",
      pid: "430400",
      name: "珠晖区"
    }, {
      id: "430406",
      pid: "430400",
      name: "雁峰区"
    }, {
      id: "430407",
      pid: "430400",
      name: "石鼓区"
    }, {
      id: "430408",
      pid: "430400",
      name: "蒸湘区"
    }, {
      id: "430412",
      pid: "430400",
      name: "南岳区"
    }, {
      id: "430421",
      pid: "430400",
      name: "衡阳县"
    }, {
      id: "430422",
      pid: "430400",
      name: "衡南县"
    }, {
      id: "430423",
      pid: "430400",
      name: "衡山县"
    }, {
      id: "430424",
      pid: "430400",
      name: "衡东县"
    }, {
      id: "430426",
      pid: "430400",
      name: "祁东县"
    }, {
      id: "430481",
      pid: "430400",
      name: "耒阳市"
    }, {
      id: "430482",
      pid: "430400",
      name: "常宁市"
    }, {
      id: "430483",
      pid: "430400",
      name: "其它区"
    }]
  }, {
    id: "430500",
    pid: "430000",
    name: "邵阳市",
    children: [{
      id: "430502",
      pid: "430500",
      name: "双清区"
    }, {
      id: "430503",
      pid: "430500",
      name: "大祥区"
    }, {
      id: "430511",
      pid: "430500",
      name: "北塔区"
    }, {
      id: "430521",
      pid: "430500",
      name: "邵东县"
    }, {
      id: "430522",
      pid: "430500",
      name: "新邵县"
    }, {
      id: "430523",
      pid: "430500",
      name: "邵阳县"
    }, {
      id: "430524",
      pid: "430500",
      name: "隆回县"
    }, {
      id: "430525",
      pid: "430500",
      name: "洞口县"
    }, {
      id: "430527",
      pid: "430500",
      name: "绥宁县"
    }, {
      id: "430528",
      pid: "430500",
      name: "新宁县"
    }, {
      id: "430529",
      pid: "430500",
      name: "城步苗族自治县"
    }, {
      id: "430581",
      pid: "430500",
      name: "武冈市"
    }, {
      id: "430582",
      pid: "430500",
      name: "其它区"
    }]
  }, {
    id: "430600",
    pid: "430000",
    name: "岳阳市",
    children: [{
      id: "430602",
      pid: "430600",
      name: "岳阳楼区"
    }, {
      id: "430603",
      pid: "430600",
      name: "云溪区"
    }, {
      id: "430611",
      pid: "430600",
      name: "君山区"
    }, {
      id: "430621",
      pid: "430600",
      name: "岳阳县"
    }, {
      id: "430623",
      pid: "430600",
      name: "华容县"
    }, {
      id: "430624",
      pid: "430600",
      name: "湘阴县"
    }, {
      id: "430626",
      pid: "430600",
      name: "平江县"
    }, {
      id: "430681",
      pid: "430600",
      name: "汨罗市"
    }, {
      id: "430682",
      pid: "430600",
      name: "临湘市"
    }, {
      id: "430683",
      pid: "430600",
      name: "其它区"
    }]
  }, {
    id: "430700",
    pid: "430000",
    name: "常德市",
    children: [{
      id: "430702",
      pid: "430700",
      name: "武陵区"
    }, {
      id: "430703",
      pid: "430700",
      name: "鼎城区"
    }, {
      id: "430721",
      pid: "430700",
      name: "安乡县"
    }, {
      id: "430722",
      pid: "430700",
      name: "汉寿县"
    }, {
      id: "430723",
      pid: "430700",
      name: "澧县"
    }, {
      id: "430724",
      pid: "430700",
      name: "临澧县"
    }, {
      id: "430725",
      pid: "430700",
      name: "桃源县"
    }, {
      id: "430726",
      pid: "430700",
      name: "石门县"
    }, {
      id: "430781",
      pid: "430700",
      name: "津市市"
    }, {
      id: "430782",
      pid: "430700",
      name: "其它区"
    }]
  }, {
    id: "430800",
    pid: "430000",
    name: "张家界市",
    children: [{
      id: "430802",
      pid: "430800",
      name: "永定区"
    }, {
      id: "430811",
      pid: "430800",
      name: "武陵源区"
    }, {
      id: "430821",
      pid: "430800",
      name: "慈利县"
    }, {
      id: "430822",
      pid: "430800",
      name: "桑植县"
    }, {
      id: "430823",
      pid: "430800",
      name: "其它区"
    }]
  }, {
    id: "430900",
    pid: "430000",
    name: "益阳市",
    children: [{
      id: "430902",
      pid: "430900",
      name: "资阳区"
    }, {
      id: "430903",
      pid: "430900",
      name: "赫山区"
    }, {
      id: "430921",
      pid: "430900",
      name: "南县"
    }, {
      id: "430922",
      pid: "430900",
      name: "桃江县"
    }, {
      id: "430923",
      pid: "430900",
      name: "安化县"
    }, {
      id: "430981",
      pid: "430900",
      name: "沅江市"
    }, {
      id: "430982",
      pid: "430900",
      name: "其它区"
    }]
  }, {
    id: "431000",
    pid: "430000",
    name: "郴州市",
    children: [{
      id: "431002",
      pid: "431000",
      name: "北湖区"
    }, {
      id: "431003",
      pid: "431000",
      name: "苏仙区"
    }, {
      id: "431021",
      pid: "431000",
      name: "桂阳县"
    }, {
      id: "431022",
      pid: "431000",
      name: "宜章县"
    }, {
      id: "431023",
      pid: "431000",
      name: "永兴县"
    }, {
      id: "431024",
      pid: "431000",
      name: "嘉禾县"
    }, {
      id: "431025",
      pid: "431000",
      name: "临武县"
    }, {
      id: "431026",
      pid: "431000",
      name: "汝城县"
    }, {
      id: "431027",
      pid: "431000",
      name: "桂东县"
    }, {
      id: "431028",
      pid: "431000",
      name: "安仁县"
    }, {
      id: "431081",
      pid: "431000",
      name: "资兴市"
    }, {
      id: "431082",
      pid: "431000",
      name: "其它区"
    }]
  }, {
    id: "431100",
    pid: "430000",
    name: "永州市",
    children: [{
      id: "431102",
      pid: "431100",
      name: "零陵区"
    }, {
      id: "431103",
      pid: "431100",
      name: "冷水滩区"
    }, {
      id: "431121",
      pid: "431100",
      name: "祁阳县"
    }, {
      id: "431122",
      pid: "431100",
      name: "东安县"
    }, {
      id: "431123",
      pid: "431100",
      name: "双牌县"
    }, {
      id: "431124",
      pid: "431100",
      name: "道县"
    }, {
      id: "431125",
      pid: "431100",
      name: "江永县"
    }, {
      id: "431126",
      pid: "431100",
      name: "宁远县"
    }, {
      id: "431127",
      pid: "431100",
      name: "蓝山县"
    }, {
      id: "431128",
      pid: "431100",
      name: "新田县"
    }, {
      id: "431129",
      pid: "431100",
      name: "江华瑶族自治县"
    }, {
      id: "431130",
      pid: "431100",
      name: "其它区"
    }]
  }, {
    id: "431200",
    pid: "430000",
    name: "怀化市",
    children: [{
      id: "431202",
      pid: "431200",
      name: "鹤城区"
    }, {
      id: "431221",
      pid: "431200",
      name: "中方县"
    }, {
      id: "431222",
      pid: "431200",
      name: "沅陵县"
    }, {
      id: "431223",
      pid: "431200",
      name: "辰溪县"
    }, {
      id: "431224",
      pid: "431200",
      name: "溆浦县"
    }, {
      id: "431225",
      pid: "431200",
      name: "会同县"
    }, {
      id: "431226",
      pid: "431200",
      name: "麻阳苗族自治县"
    }, {
      id: "431227",
      pid: "431200",
      name: "新晃侗族自治县"
    }, {
      id: "431228",
      pid: "431200",
      name: "芷江侗族自治县"
    }, {
      id: "431229",
      pid: "431200",
      name: "靖州苗族侗族自治县"
    }, {
      id: "431230",
      pid: "431200",
      name: "通道侗族自治县"
    }, {
      id: "431281",
      pid: "431200",
      name: "洪江市"
    }, {
      id: "431282",
      pid: "431200",
      name: "其它区"
    }]
  }, {
    id: "431300",
    pid: "430000",
    name: "娄底市",
    children: [{
      id: "431302",
      pid: "431300",
      name: "娄星区"
    }, {
      id: "431321",
      pid: "431300",
      name: "双峰县"
    }, {
      id: "431322",
      pid: "431300",
      name: "新化县"
    }, {
      id: "431381",
      pid: "431300",
      name: "冷水江市"
    }, {
      id: "431382",
      pid: "431300",
      name: "涟源市"
    }, {
      id: "431383",
      pid: "431300",
      name: "其它区"
    }]
  }, {
    id: "433100",
    pid: "430000",
    name: "湘西土家族苗族自治州",
    children: [{
      id: "433101",
      pid: "433100",
      name: "吉首市"
    }, {
      id: "433122",
      pid: "433100",
      name: "泸溪县"
    }, {
      id: "433123",
      pid: "433100",
      name: "凤凰县"
    }, {
      id: "433124",
      pid: "433100",
      name: "花垣县"
    }, {
      id: "433125",
      pid: "433100",
      name: "保靖县"
    }, {
      id: "433126",
      pid: "433100",
      name: "古丈县"
    }, {
      id: "433127",
      pid: "433100",
      name: "永顺县"
    }, {
      id: "433130",
      pid: "433100",
      name: "龙山县"
    }, {
      id: "433131",
      pid: "433100",
      name: "其它区"
    }]
  }]
}, {
  id: "440000",
  pid: "440000",
  name: "广东省",
  children: [{
    id: "440100",
    pid: "440000",
    name: "广州市",
    children: [{
      id: "440103",
      pid: "440100",
      name: "荔湾区"
    }, {
      id: "440104",
      pid: "440100",
      name: "越秀区"
    }, {
      id: "440105",
      pid: "440100",
      name: "海珠区"
    }, {
      id: "440106",
      pid: "440100",
      name: "天河区"
    }, {
      id: "440111",
      pid: "440100",
      name: "白云区"
    }, {
      id: "440112",
      pid: "440100",
      name: "黄埔区"
    }, {
      id: "440113",
      pid: "440100",
      name: "番禺区"
    }, {
      id: "440114",
      pid: "440100",
      name: "花都区"
    }, {
      id: "440115",
      pid: "440100",
      name: "南沙区"
    }, {
      id: "440116",
      pid: "440100",
      name: "萝岗区"
    }, {
      id: "440183",
      pid: "440100",
      name: "增城市"
    }, {
      id: "440184",
      pid: "440100",
      name: "从化市"
    }, {
      id: "440189",
      pid: "440100",
      name: "其它区"
    }]
  }, {
    id: "440200",
    pid: "440000",
    name: "韶关市",
    children: [{
      id: "440203",
      pid: "440200",
      name: "武江区"
    }, {
      id: "440204",
      pid: "440200",
      name: "浈江区"
    }, {
      id: "440205",
      pid: "440200",
      name: "曲江区"
    }, {
      id: "440222",
      pid: "440200",
      name: "始兴县"
    }, {
      id: "440224",
      pid: "440200",
      name: "仁化县"
    }, {
      id: "440229",
      pid: "440200",
      name: "翁源县"
    }, {
      id: "440232",
      pid: "440200",
      name: "乳源瑶族自治县"
    }, {
      id: "440233",
      pid: "440200",
      name: "新丰县"
    }, {
      id: "440281",
      pid: "440200",
      name: "乐昌市"
    }, {
      id: "440282",
      pid: "440200",
      name: "南雄市"
    }, {
      id: "440283",
      pid: "440200",
      name: "其它区"
    }]
  }, {
    id: "440300",
    pid: "440000",
    name: "深圳市",
    children: [{
      id: "440303",
      pid: "440300",
      name: "罗湖区"
    }, {
      id: "440304",
      pid: "440300",
      name: "福田区"
    }, {
      id: "440305",
      pid: "440300",
      name: "南山区"
    }, {
      id: "440306",
      pid: "440300",
      name: "宝安区"
    }, {
      id: "440307",
      pid: "440300",
      name: "龙岗区"
    }, {
      id: "440308",
      pid: "440300",
      name: "盐田区"
    }, {
      id: "440309",
      pid: "440300",
      name: "其它区"
    }, {
      id: "440320",
      pid: "440300",
      name: "光明新区"
    }, {
      id: "440321",
      pid: "440300",
      name: "坪山新区"
    }, {
      id: "440322",
      pid: "440300",
      name: "大鹏新区"
    }, {
      id: "440323",
      pid: "440300",
      name: "龙华新区"
    }]
  }, {
    id: "440400",
    pid: "440000",
    name: "珠海市",
    children: [{
      id: "440402",
      pid: "440400",
      name: "香洲区"
    }, {
      id: "440403",
      pid: "440400",
      name: "斗门区"
    }, {
      id: "440404",
      pid: "440400",
      name: "金湾区"
    }, {
      id: "440488",
      pid: "440400",
      name: "其它区"
    }]
  }, {
    id: "440500",
    pid: "440000",
    name: "汕头市",
    children: [{
      id: "440507",
      pid: "440500",
      name: "龙湖区"
    }, {
      id: "440511",
      pid: "440500",
      name: "金平区"
    }, {
      id: "440512",
      pid: "440500",
      name: "濠江区"
    }, {
      id: "440513",
      pid: "440500",
      name: "潮阳区"
    }, {
      id: "440514",
      pid: "440500",
      name: "潮南区"
    }, {
      id: "440515",
      pid: "440500",
      name: "澄海区"
    }, {
      id: "440523",
      pid: "440500",
      name: "南澳县"
    }, {
      id: "440524",
      pid: "440500",
      name: "其它区"
    }]
  }, {
    id: "440600",
    pid: "440000",
    name: "佛山市",
    children: [{
      id: "440604",
      pid: "440600",
      name: "禅城区"
    }, {
      id: "440605",
      pid: "440600",
      name: "南海区"
    }, {
      id: "440606",
      pid: "440600",
      name: "顺德区"
    }, {
      id: "440607",
      pid: "440600",
      name: "三水区"
    }, {
      id: "440608",
      pid: "440600",
      name: "高明区"
    }, {
      id: "440609",
      pid: "440600",
      name: "其它区"
    }]
  }, {
    id: "440700",
    pid: "440000",
    name: "江门市",
    children: [{
      id: "440703",
      pid: "440700",
      name: "蓬江区"
    }, {
      id: "440704",
      pid: "440700",
      name: "江海区"
    }, {
      id: "440705",
      pid: "440700",
      name: "新会区"
    }, {
      id: "440781",
      pid: "440700",
      name: "台山市"
    }, {
      id: "440783",
      pid: "440700",
      name: "开平市"
    }, {
      id: "440784",
      pid: "440700",
      name: "鹤山市"
    }, {
      id: "440785",
      pid: "440700",
      name: "恩平市"
    }, {
      id: "440786",
      pid: "440700",
      name: "其它区"
    }]
  }, {
    id: "440800",
    pid: "440000",
    name: "湛江市",
    children: [{
      id: "440802",
      pid: "440800",
      name: "赤坎区"
    }, {
      id: "440803",
      pid: "440800",
      name: "霞山区"
    }, {
      id: "440804",
      pid: "440800",
      name: "坡头区"
    }, {
      id: "440811",
      pid: "440800",
      name: "麻章区"
    }, {
      id: "440823",
      pid: "440800",
      name: "遂溪县"
    }, {
      id: "440825",
      pid: "440800",
      name: "徐闻县"
    }, {
      id: "440881",
      pid: "440800",
      name: "廉江市"
    }, {
      id: "440882",
      pid: "440800",
      name: "雷州市"
    }, {
      id: "440883",
      pid: "440800",
      name: "吴川市"
    }, {
      id: "440884",
      pid: "440800",
      name: "其它区"
    }]
  }, {
    id: "440900",
    pid: "440000",
    name: "茂名市",
    children: [{
      id: "440902",
      pid: "440900",
      name: "茂南区"
    }, {
      id: "440903",
      pid: "440900",
      name: "茂港区"
    }, {
      id: "440923",
      pid: "440900",
      name: "电白县"
    }, {
      id: "440981",
      pid: "440900",
      name: "高州市"
    }, {
      id: "440982",
      pid: "440900",
      name: "化州市"
    }, {
      id: "440983",
      pid: "440900",
      name: "信宜市"
    }, {
      id: "440984",
      pid: "440900",
      name: "其它区"
    }]
  }, {
    id: "441200",
    pid: "440000",
    name: "肇庆市",
    children: [{
      id: "441202",
      pid: "441200",
      name: "端州区"
    }, {
      id: "441203",
      pid: "441200",
      name: "鼎湖区"
    }, {
      id: "441223",
      pid: "441200",
      name: "广宁县"
    }, {
      id: "441224",
      pid: "441200",
      name: "怀集县"
    }, {
      id: "441225",
      pid: "441200",
      name: "封开县"
    }, {
      id: "441226",
      pid: "441200",
      name: "德庆县"
    }, {
      id: "441283",
      pid: "441200",
      name: "高要市"
    }, {
      id: "441284",
      pid: "441200",
      name: "四会市"
    }, {
      id: "441285",
      pid: "441200",
      name: "其它区"
    }]
  }, {
    id: "441300",
    pid: "440000",
    name: "惠州市",
    children: [{
      id: "441302",
      pid: "441300",
      name: "惠城区"
    }, {
      id: "441303",
      pid: "441300",
      name: "惠阳区"
    }, {
      id: "441322",
      pid: "441300",
      name: "博罗县"
    }, {
      id: "441323",
      pid: "441300",
      name: "惠东县"
    }, {
      id: "441324",
      pid: "441300",
      name: "龙门县"
    }, {
      id: "441325",
      pid: "441300",
      name: "其它区"
    }]
  }, {
    id: "441400",
    pid: "440000",
    name: "梅州市",
    children: [{
      id: "441402",
      pid: "441400",
      name: "梅江区"
    }, {
      id: "441421",
      pid: "441400",
      name: "梅县"
    }, {
      id: "441422",
      pid: "441400",
      name: "大埔县"
    }, {
      id: "441423",
      pid: "441400",
      name: "丰顺县"
    }, {
      id: "441424",
      pid: "441400",
      name: "五华县"
    }, {
      id: "441426",
      pid: "441400",
      name: "平远县"
    }, {
      id: "441427",
      pid: "441400",
      name: "蕉岭县"
    }, {
      id: "441481",
      pid: "441400",
      name: "兴宁市"
    }, {
      id: "441482",
      pid: "441400",
      name: "其它区"
    }]
  }, {
    id: "441500",
    pid: "440000",
    name: "汕尾市",
    children: [{
      id: "441502",
      pid: "441500",
      name: "城区"
    }, {
      id: "441521",
      pid: "441500",
      name: "海丰县"
    }, {
      id: "441523",
      pid: "441500",
      name: "陆河县"
    }, {
      id: "441581",
      pid: "441500",
      name: "陆丰市"
    }, {
      id: "441582",
      pid: "441500",
      name: "其它区"
    }]
  }, {
    id: "441600",
    pid: "440000",
    name: "河源市",
    children: [{
      id: "441602",
      pid: "441600",
      name: "源城区"
    }, {
      id: "441621",
      pid: "441600",
      name: "紫金县"
    }, {
      id: "441622",
      pid: "441600",
      name: "龙川县"
    }, {
      id: "441623",
      pid: "441600",
      name: "连平县"
    }, {
      id: "441624",
      pid: "441600",
      name: "和平县"
    }, {
      id: "441625",
      pid: "441600",
      name: "东源县"
    }, {
      id: "441626",
      pid: "441600",
      name: "其它区"
    }]
  }, {
    id: "441700",
    pid: "440000",
    name: "阳江市",
    children: [{
      id: "441702",
      pid: "441700",
      name: "江城区"
    }, {
      id: "441721",
      pid: "441700",
      name: "阳西县"
    }, {
      id: "441723",
      pid: "441700",
      name: "阳东县"
    }, {
      id: "441781",
      pid: "441700",
      name: "阳春市"
    }, {
      id: "441782",
      pid: "441700",
      name: "其它区"
    }]
  }, {
    id: "441800",
    pid: "440000",
    name: "清远市",
    children: [{
      id: "441802",
      pid: "441800",
      name: "清城区"
    }, {
      id: "441821",
      pid: "441800",
      name: "佛冈县"
    }, {
      id: "441823",
      pid: "441800",
      name: "阳山县"
    }, {
      id: "441825",
      pid: "441800",
      name: "连山壮族瑶族自治县"
    }, {
      id: "441826",
      pid: "441800",
      name: "连南瑶族自治县"
    }, {
      id: "441827",
      pid: "441800",
      name: "清新区"
    }, {
      id: "441881",
      pid: "441800",
      name: "英德市"
    }, {
      id: "441882",
      pid: "441800",
      name: "连州市"
    }, {
      id: "441883",
      pid: "441800",
      name: "其它区"
    }]
  }, {
    id: "441900",
    pid: "440000",
    name: "东莞市"
  }, {
    id: "442000",
    pid: "440000",
    name: "中山市"
  }, {
    id: "442101",
    pid: "440000",
    name: "东沙群岛"
  }, {
    id: "445100",
    pid: "440000",
    name: "潮州市",
    children: [{
      id: "445102",
      pid: "445100",
      name: "湘桥区"
    }, {
      id: "445121",
      pid: "445100",
      name: "潮安区"
    }, {
      id: "445122",
      pid: "445100",
      name: "饶平县"
    }, {
      id: "445186",
      pid: "445100",
      name: "其它区"
    }]
  }, {
    id: "445200",
    pid: "440000",
    name: "揭阳市",
    children: [{
      id: "445202",
      pid: "445200",
      name: "榕城区"
    }, {
      id: "445221",
      pid: "445200",
      name: "揭东区"
    }, {
      id: "445222",
      pid: "445200",
      name: "揭西县"
    }, {
      id: "445224",
      pid: "445200",
      name: "惠来县"
    }, {
      id: "445281",
      pid: "445200",
      name: "普宁市"
    }, {
      id: "445285",
      pid: "445200",
      name: "其它区"
    }]
  }, {
    id: "445300",
    pid: "440000",
    name: "云浮市",
    children: [{
      id: "445302",
      pid: "445300",
      name: "云城区"
    }, {
      id: "445321",
      pid: "445300",
      name: "新兴县"
    }, {
      id: "445322",
      pid: "445300",
      name: "郁南县"
    }, {
      id: "445323",
      pid: "445300",
      name: "云安县"
    }, {
      id: "445381",
      pid: "445300",
      name: "罗定市"
    }, {
      id: "445382",
      pid: "445300",
      name: "其它区"
    }]
  }]
}, {
  id: "450000",
  pid: "450000",
  name: "广西壮族自治区",
  children: [{
    id: "450100",
    pid: "450000",
    name: "南宁市",
    children: [{
      id: "450102",
      pid: "450100",
      name: "兴宁区"
    }, {
      id: "450103",
      pid: "450100",
      name: "青秀区"
    }, {
      id: "450105",
      pid: "450100",
      name: "江南区"
    }, {
      id: "450107",
      pid: "450100",
      name: "西乡塘区"
    }, {
      id: "450108",
      pid: "450100",
      name: "良庆区"
    }, {
      id: "450109",
      pid: "450100",
      name: "邕宁区"
    }, {
      id: "450122",
      pid: "450100",
      name: "武鸣县"
    }, {
      id: "450123",
      pid: "450100",
      name: "隆安县"
    }, {
      id: "450124",
      pid: "450100",
      name: "马山县"
    }, {
      id: "450125",
      pid: "450100",
      name: "上林县"
    }, {
      id: "450126",
      pid: "450100",
      name: "宾阳县"
    }, {
      id: "450127",
      pid: "450100",
      name: "横县"
    }, {
      id: "450128",
      pid: "450100",
      name: "其它区"
    }]
  }, {
    id: "450200",
    pid: "450000",
    name: "柳州市",
    children: [{
      id: "450202",
      pid: "450200",
      name: "城中区"
    }, {
      id: "450203",
      pid: "450200",
      name: "鱼峰区"
    }, {
      id: "450204",
      pid: "450200",
      name: "柳南区"
    }, {
      id: "450205",
      pid: "450200",
      name: "柳北区"
    }, {
      id: "450221",
      pid: "450200",
      name: "柳江县"
    }, {
      id: "450222",
      pid: "450200",
      name: "柳城县"
    }, {
      id: "450223",
      pid: "450200",
      name: "鹿寨县"
    }, {
      id: "450224",
      pid: "450200",
      name: "融安县"
    }, {
      id: "450225",
      pid: "450200",
      name: "融水苗族自治县"
    }, {
      id: "450226",
      pid: "450200",
      name: "三江侗族自治县"
    }, {
      id: "450227",
      pid: "450200",
      name: "其它区"
    }]
  }, {
    id: "450300",
    pid: "450000",
    name: "桂林市",
    children: [{
      id: "450302",
      pid: "450300",
      name: "秀峰区"
    }, {
      id: "450303",
      pid: "450300",
      name: "叠彩区"
    }, {
      id: "450304",
      pid: "450300",
      name: "象山区"
    }, {
      id: "450305",
      pid: "450300",
      name: "七星区"
    }, {
      id: "450311",
      pid: "450300",
      name: "雁山区"
    }, {
      id: "450321",
      pid: "450300",
      name: "阳朔县"
    }, {
      id: "450322",
      pid: "450300",
      name: "临桂区"
    }, {
      id: "450323",
      pid: "450300",
      name: "灵川县"
    }, {
      id: "450324",
      pid: "450300",
      name: "全州县"
    }, {
      id: "450325",
      pid: "450300",
      name: "兴安县"
    }, {
      id: "450326",
      pid: "450300",
      name: "永福县"
    }, {
      id: "450327",
      pid: "450300",
      name: "灌阳县"
    }, {
      id: "450328",
      pid: "450300",
      name: "龙胜各族自治县"
    }, {
      id: "450329",
      pid: "450300",
      name: "资源县"
    }, {
      id: "450330",
      pid: "450300",
      name: "平乐县"
    }, {
      id: "450331",
      pid: "450300",
      name: "荔浦县"
    }, {
      id: "450332",
      pid: "450300",
      name: "恭城瑶族自治县"
    }, {
      id: "450333",
      pid: "450300",
      name: "其它区"
    }]
  }, {
    id: "450400",
    pid: "450000",
    name: "梧州市",
    children: [{
      id: "450403",
      pid: "450400",
      name: "万秀区"
    }, {
      id: "450405",
      pid: "450400",
      name: "长洲区"
    }, {
      id: "450406",
      pid: "450400",
      name: "龙圩区"
    }, {
      id: "450421",
      pid: "450400",
      name: "苍梧县"
    }, {
      id: "450422",
      pid: "450400",
      name: "藤县"
    }, {
      id: "450423",
      pid: "450400",
      name: "蒙山县"
    }, {
      id: "450481",
      pid: "450400",
      name: "岑溪市"
    }, {
      id: "450482",
      pid: "450400",
      name: "其它区"
    }]
  }, {
    id: "450500",
    pid: "450000",
    name: "北海市",
    children: [{
      id: "450502",
      pid: "450500",
      name: "海城区"
    }, {
      id: "450503",
      pid: "450500",
      name: "银海区"
    }, {
      id: "450512",
      pid: "450500",
      name: "铁山港区"
    }, {
      id: "450521",
      pid: "450500",
      name: "合浦县"
    }, {
      id: "450522",
      pid: "450500",
      name: "其它区"
    }]
  }, {
    id: "450600",
    pid: "450000",
    name: "防城港市",
    children: [{
      id: "450602",
      pid: "450600",
      name: "港口区"
    }, {
      id: "450603",
      pid: "450600",
      name: "防城区"
    }, {
      id: "450621",
      pid: "450600",
      name: "上思县"
    }, {
      id: "450681",
      pid: "450600",
      name: "东兴市"
    }, {
      id: "450682",
      pid: "450600",
      name: "其它区"
    }]
  }, {
    id: "450700",
    pid: "450000",
    name: "钦州市",
    children: [{
      id: "450702",
      pid: "450700",
      name: "钦南区"
    }, {
      id: "450703",
      pid: "450700",
      name: "钦北区"
    }, {
      id: "450721",
      pid: "450700",
      name: "灵山县"
    }, {
      id: "450722",
      pid: "450700",
      name: "浦北县"
    }, {
      id: "450723",
      pid: "450700",
      name: "其它区"
    }]
  }, {
    id: "450800",
    pid: "450000",
    name: "贵港市",
    children: [{
      id: "450802",
      pid: "450800",
      name: "港北区"
    }, {
      id: "450803",
      pid: "450800",
      name: "港南区"
    }, {
      id: "450804",
      pid: "450800",
      name: "覃塘区"
    }, {
      id: "450821",
      pid: "450800",
      name: "平南县"
    }, {
      id: "450881",
      pid: "450800",
      name: "桂平市"
    }, {
      id: "450882",
      pid: "450800",
      name: "其它区"
    }]
  }, {
    id: "450900",
    pid: "450000",
    name: "玉林市",
    children: [{
      id: "450902",
      pid: "450900",
      name: "玉州区"
    }, {
      id: "450903",
      pid: "450900",
      name: "福绵区"
    }, {
      id: "450921",
      pid: "450900",
      name: "容县"
    }, {
      id: "450922",
      pid: "450900",
      name: "陆川县"
    }, {
      id: "450923",
      pid: "450900",
      name: "博白县"
    }, {
      id: "450924",
      pid: "450900",
      name: "兴业县"
    }, {
      id: "450981",
      pid: "450900",
      name: "北流市"
    }, {
      id: "450982",
      pid: "450900",
      name: "其它区"
    }]
  }, {
    id: "451000",
    pid: "450000",
    name: "百色市",
    children: [{
      id: "451002",
      pid: "451000",
      name: "右江区"
    }, {
      id: "451021",
      pid: "451000",
      name: "田阳县"
    }, {
      id: "451022",
      pid: "451000",
      name: "田东县"
    }, {
      id: "451023",
      pid: "451000",
      name: "平果县"
    }, {
      id: "451024",
      pid: "451000",
      name: "德保县"
    }, {
      id: "451025",
      pid: "451000",
      name: "靖西县"
    }, {
      id: "451026",
      pid: "451000",
      name: "那坡县"
    }, {
      id: "451027",
      pid: "451000",
      name: "凌云县"
    }, {
      id: "451028",
      pid: "451000",
      name: "乐业县"
    }, {
      id: "451029",
      pid: "451000",
      name: "田林县"
    }, {
      id: "451030",
      pid: "451000",
      name: "西林县"
    }, {
      id: "451031",
      pid: "451000",
      name: "隆林各族自治县"
    }, {
      id: "451032",
      pid: "451000",
      name: "其它区"
    }]
  }, {
    id: "451100",
    pid: "450000",
    name: "贺州市",
    children: [{
      id: "451102",
      pid: "451100",
      name: "八步区"
    }, {
      id: "451119",
      pid: "451100",
      name: "平桂管理区"
    }, {
      id: "451121",
      pid: "451100",
      name: "昭平县"
    }, {
      id: "451122",
      pid: "451100",
      name: "钟山县"
    }, {
      id: "451123",
      pid: "451100",
      name: "富川瑶族自治县"
    }, {
      id: "451124",
      pid: "451100",
      name: "其它区"
    }]
  }, {
    id: "451200",
    pid: "450000",
    name: "河池市",
    children: [{
      id: "451202",
      pid: "451200",
      name: "金城江区"
    }, {
      id: "451221",
      pid: "451200",
      name: "南丹县"
    }, {
      id: "451222",
      pid: "451200",
      name: "天峨县"
    }, {
      id: "451223",
      pid: "451200",
      name: "凤山县"
    }, {
      id: "451224",
      pid: "451200",
      name: "东兰县"
    }, {
      id: "451225",
      pid: "451200",
      name: "罗城仫佬族自治县"
    }, {
      id: "451226",
      pid: "451200",
      name: "环江毛南族自治县"
    }, {
      id: "451227",
      pid: "451200",
      name: "巴马瑶族自治县"
    }, {
      id: "451228",
      pid: "451200",
      name: "都安瑶族自治县"
    }, {
      id: "451229",
      pid: "451200",
      name: "大化瑶族自治县"
    }, {
      id: "451281",
      pid: "451200",
      name: "宜州市"
    }, {
      id: "451282",
      pid: "451200",
      name: "其它区"
    }]
  }, {
    id: "451300",
    pid: "450000",
    name: "来宾市",
    children: [{
      id: "451302",
      pid: "451300",
      name: "兴宾区"
    }, {
      id: "451321",
      pid: "451300",
      name: "忻城县"
    }, {
      id: "451322",
      pid: "451300",
      name: "象州县"
    }, {
      id: "451323",
      pid: "451300",
      name: "武宣县"
    }, {
      id: "451324",
      pid: "451300",
      name: "金秀瑶族自治县"
    }, {
      id: "451381",
      pid: "451300",
      name: "合山市"
    }, {
      id: "451382",
      pid: "451300",
      name: "其它区"
    }]
  }, {
    id: "451400",
    pid: "450000",
    name: "崇左市",
    children: [{
      id: "451402",
      pid: "451400",
      name: "江州区"
    }, {
      id: "451421",
      pid: "451400",
      name: "扶绥县"
    }, {
      id: "451422",
      pid: "451400",
      name: "宁明县"
    }, {
      id: "451423",
      pid: "451400",
      name: "龙州县"
    }, {
      id: "451424",
      pid: "451400",
      name: "大新县"
    }, {
      id: "451425",
      pid: "451400",
      name: "天等县"
    }, {
      id: "451481",
      pid: "451400",
      name: "凭祥市"
    }, {
      id: "451482",
      pid: "451400",
      name: "其它区"
    }]
  }]
}, {
  id: "460000",
  pid: "460000",
  name: "海南省",
  children: [{
    id: "460100",
    pid: "460000",
    name: "海口市",
    children: [{
      id: "460105",
      pid: "460100",
      name: "秀英区"
    }, {
      id: "460106",
      pid: "460100",
      name: "龙华区"
    }, {
      id: "460107",
      pid: "460100",
      name: "琼山区"
    }, {
      id: "460108",
      pid: "460100",
      name: "美兰区"
    }, {
      id: "460109",
      pid: "460100",
      name: "其它区"
    }]
  }, {
    id: "460200",
    pid: "460000",
    name: "三亚市"
  }, {
    id: "460300",
    pid: "460000",
    name: "三沙市",
    children: [{
      id: "460321",
      pid: "460300",
      name: "西沙群岛"
    }, {
      id: "460322",
      pid: "460300",
      name: "南沙群岛"
    }, {
      id: "460323",
      pid: "460300",
      name: "中沙群岛的岛礁及其海域"
    }]
  }, {
    id: "469001",
    pid: "460000",
    name: "五指山市"
  }, {
    id: "469002",
    pid: "460000",
    name: "琼海市"
  }, {
    id: "469003",
    pid: "460000",
    name: "儋州市"
  }, {
    id: "469005",
    pid: "460000",
    name: "文昌市"
  }, {
    id: "469006",
    pid: "460000",
    name: "万宁市"
  }, {
    id: "469007",
    pid: "460000",
    name: "东方市"
  }, {
    id: "469025",
    pid: "460000",
    name: "定安县"
  }, {
    id: "469026",
    pid: "460000",
    name: "屯昌县"
  }, {
    id: "469027",
    pid: "460000",
    name: "澄迈县"
  }, {
    id: "469028",
    pid: "460000",
    name: "临高县"
  }, {
    id: "469030",
    pid: "460000",
    name: "白沙黎族自治县"
  }, {
    id: "469031",
    pid: "460000",
    name: "昌江黎族自治县"
  }, {
    id: "469033",
    pid: "460000",
    name: "乐东黎族自治县"
  }, {
    id: "469034",
    pid: "460000",
    name: "陵水黎族自治县"
  }, {
    id: "469035",
    pid: "460000",
    name: "保亭黎族苗族自治县"
  }, {
    id: "469036",
    pid: "460000",
    name: "琼中黎族苗族自治县"
  }]
}, {
  id: "500000",
  pid: "500000",
  name: "重庆",
  children: [{
    id: "500100",
    pid: "500000",
    name: "重庆市",
    children: [{
      id: "500101",
      pid: "500100",
      name: "万州区"
    }, {
      id: "500102",
      pid: "500100",
      name: "涪陵区"
    }, {
      id: "500103",
      pid: "500100",
      name: "渝中区"
    }, {
      id: "500104",
      pid: "500100",
      name: "大渡口区"
    }, {
      id: "500105",
      pid: "500100",
      name: "江北区"
    }, {
      id: "500106",
      pid: "500100",
      name: "沙坪坝区"
    }, {
      id: "500107",
      pid: "500100",
      name: "九龙坡区"
    }, {
      id: "500108",
      pid: "500100",
      name: "南岸区"
    }, {
      id: "500109",
      pid: "500100",
      name: "北碚区"
    }, {
      id: "500110",
      pid: "500100",
      name: "万盛区"
    }, {
      id: "500111",
      pid: "500100",
      name: "双桥区"
    }, {
      id: "500112",
      pid: "500100",
      name: "渝北区"
    }, {
      id: "500113",
      pid: "500100",
      name: "巴南区"
    }, {
      id: "500114",
      pid: "500100",
      name: "黔江区"
    }, {
      id: "500115",
      pid: "500100",
      name: "长寿区"
    }]
  }, {
    id: "500222",
    pid: "500000",
    name: "綦江区"
  }, {
    id: "500223",
    pid: "500000",
    name: "潼南县"
  }, {
    id: "500224",
    pid: "500000",
    name: "铜梁县"
  }, {
    id: "500225",
    pid: "500000",
    name: "大足区"
  }, {
    id: "500226",
    pid: "500000",
    name: "荣昌县"
  }, {
    id: "500227",
    pid: "500000",
    name: "璧山县"
  }, {
    id: "500228",
    pid: "500000",
    name: "梁平县"
  }, {
    id: "500229",
    pid: "500000",
    name: "城口县"
  }, {
    id: "500230",
    pid: "500000",
    name: "丰都县"
  }, {
    id: "500231",
    pid: "500000",
    name: "垫江县"
  }, {
    id: "500232",
    pid: "500000",
    name: "武隆县"
  }, {
    id: "500233",
    pid: "500000",
    name: "忠县"
  }, {
    id: "500234",
    pid: "500000",
    name: "开县"
  }, {
    id: "500235",
    pid: "500000",
    name: "云阳县"
  }, {
    id: "500236",
    pid: "500000",
    name: "奉节县"
  }, {
    id: "500237",
    pid: "500000",
    name: "巫山县"
  }, {
    id: "500238",
    pid: "500000",
    name: "巫溪县"
  }, {
    id: "500240",
    pid: "500000",
    name: "石柱土家族自治县"
  }, {
    id: "500241",
    pid: "500000",
    name: "秀山土家族苗族自治县"
  }, {
    id: "500242",
    pid: "500000",
    name: "酉阳土家族苗族自治县"
  }, {
    id: "500243",
    pid: "500000",
    name: "彭水苗族土家族自治县"
  }, {
    id: "500381",
    pid: "500000",
    name: "江津区"
  }, {
    id: "500382",
    pid: "500000",
    name: "合川区"
  }, {
    id: "500383",
    pid: "500000",
    name: "永川区"
  }, {
    id: "500384",
    pid: "500000",
    name: "南川区"
  }, {
    id: "500385",
    pid: "500000",
    name: "其它区"
  }]
}, {
  id: "510000",
  pid: "510000",
  name: "四川省",
  children: [{
    id: "510100",
    pid: "510000",
    name: "成都市",
    children: [{
      id: "510104",
      pid: "510100",
      name: "锦江区"
    }, {
      id: "510105",
      pid: "510100",
      name: "青羊区"
    }, {
      id: "510106",
      pid: "510100",
      name: "金牛区"
    }, {
      id: "510107",
      pid: "510100",
      name: "武侯区"
    }, {
      id: "510108",
      pid: "510100",
      name: "成华区"
    }, {
      id: "510112",
      pid: "510100",
      name: "龙泉驿区"
    }, {
      id: "510113",
      pid: "510100",
      name: "青白江区"
    }, {
      id: "510114",
      pid: "510100",
      name: "新都区"
    }, {
      id: "510115",
      pid: "510100",
      name: "温江区"
    }, {
      id: "510121",
      pid: "510100",
      name: "金堂县"
    }, {
      id: "510122",
      pid: "510100",
      name: "双流县"
    }, {
      id: "510124",
      pid: "510100",
      name: "郫县"
    }, {
      id: "510129",
      pid: "510100",
      name: "大邑县"
    }, {
      id: "510131",
      pid: "510100",
      name: "蒲江县"
    }, {
      id: "510132",
      pid: "510100",
      name: "新津县"
    }, {
      id: "510181",
      pid: "510100",
      name: "都江堰市"
    }, {
      id: "510182",
      pid: "510100",
      name: "彭州市"
    }, {
      id: "510183",
      pid: "510100",
      name: "邛崃市"
    }, {
      id: "510184",
      pid: "510100",
      name: "崇州市"
    }, {
      id: "510185",
      pid: "510100",
      name: "其它区"
    }]
  }, {
    id: "510300",
    pid: "510000",
    name: "自贡市",
    children: [{
      id: "510302",
      pid: "510300",
      name: "自流井区"
    }, {
      id: "510303",
      pid: "510300",
      name: "贡井区"
    }, {
      id: "510304",
      pid: "510300",
      name: "大安区"
    }, {
      id: "510311",
      pid: "510300",
      name: "沿滩区"
    }, {
      id: "510321",
      pid: "510300",
      name: "荣县"
    }, {
      id: "510322",
      pid: "510300",
      name: "富顺县"
    }, {
      id: "510323",
      pid: "510300",
      name: "其它区"
    }]
  }, {
    id: "510400",
    pid: "510000",
    name: "攀枝花市",
    children: [{
      id: "510402",
      pid: "510400",
      name: "东区"
    }, {
      id: "510403",
      pid: "510400",
      name: "西区"
    }, {
      id: "510411",
      pid: "510400",
      name: "仁和区"
    }, {
      id: "510421",
      pid: "510400",
      name: "米易县"
    }, {
      id: "510422",
      pid: "510400",
      name: "盐边县"
    }, {
      id: "510423",
      pid: "510400",
      name: "其它区"
    }]
  }, {
    id: "510500",
    pid: "510000",
    name: "泸州市",
    children: [{
      id: "510502",
      pid: "510500",
      name: "江阳区"
    }, {
      id: "510503",
      pid: "510500",
      name: "纳溪区"
    }, {
      id: "510504",
      pid: "510500",
      name: "龙马潭区"
    }, {
      id: "510521",
      pid: "510500",
      name: "泸县"
    }, {
      id: "510522",
      pid: "510500",
      name: "合江县"
    }, {
      id: "510524",
      pid: "510500",
      name: "叙永县"
    }, {
      id: "510525",
      pid: "510500",
      name: "古蔺县"
    }, {
      id: "510526",
      pid: "510500",
      name: "其它区"
    }]
  }, {
    id: "510600",
    pid: "510000",
    name: "德阳市",
    children: [{
      id: "510603",
      pid: "510600",
      name: "旌阳区"
    }, {
      id: "510623",
      pid: "510600",
      name: "中江县"
    }, {
      id: "510626",
      pid: "510600",
      name: "罗江县"
    }, {
      id: "510681",
      pid: "510600",
      name: "广汉市"
    }, {
      id: "510682",
      pid: "510600",
      name: "什邡市"
    }, {
      id: "510683",
      pid: "510600",
      name: "绵竹市"
    }, {
      id: "510684",
      pid: "510600",
      name: "其它区"
    }]
  }, {
    id: "510700",
    pid: "510000",
    name: "绵阳市",
    children: [{
      id: "510703",
      pid: "510700",
      name: "涪城区"
    }, {
      id: "510704",
      pid: "510700",
      name: "游仙区"
    }, {
      id: "510722",
      pid: "510700",
      name: "三台县"
    }, {
      id: "510723",
      pid: "510700",
      name: "盐亭县"
    }, {
      id: "510724",
      pid: "510700",
      name: "安县"
    }, {
      id: "510725",
      pid: "510700",
      name: "梓潼县"
    }, {
      id: "510726",
      pid: "510700",
      name: "北川羌族自治县"
    }, {
      id: "510727",
      pid: "510700",
      name: "平武县"
    }, {
      id: "510781",
      pid: "510700",
      name: "江油市"
    }, {
      id: "510782",
      pid: "510700",
      name: "其它区"
    }]
  }, {
    id: "510800",
    pid: "510000",
    name: "广元市",
    children: [{
      id: "510802",
      pid: "510800",
      name: "利州区"
    }, {
      id: "510811",
      pid: "510800",
      name: "昭化区"
    }, {
      id: "510812",
      pid: "510800",
      name: "朝天区"
    }, {
      id: "510821",
      pid: "510800",
      name: "旺苍县"
    }, {
      id: "510822",
      pid: "510800",
      name: "青川县"
    }, {
      id: "510823",
      pid: "510800",
      name: "剑阁县"
    }, {
      id: "510824",
      pid: "510800",
      name: "苍溪县"
    }, {
      id: "510825",
      pid: "510800",
      name: "其它区"
    }]
  }, {
    id: "510900",
    pid: "510000",
    name: "遂宁市",
    children: [{
      id: "510903",
      pid: "510900",
      name: "船山区"
    }, {
      id: "510904",
      pid: "510900",
      name: "安居区"
    }, {
      id: "510921",
      pid: "510900",
      name: "蓬溪县"
    }, {
      id: "510922",
      pid: "510900",
      name: "射洪县"
    }, {
      id: "510923",
      pid: "510900",
      name: "大英县"
    }, {
      id: "510924",
      pid: "510900",
      name: "其它区"
    }]
  }, {
    id: "511000",
    pid: "510000",
    name: "内江市",
    children: [{
      id: "511002",
      pid: "511000",
      name: "市中区"
    }, {
      id: "511011",
      pid: "511000",
      name: "东兴区"
    }, {
      id: "511024",
      pid: "511000",
      name: "威远县"
    }, {
      id: "511025",
      pid: "511000",
      name: "资中县"
    }, {
      id: "511028",
      pid: "511000",
      name: "隆昌县"
    }, {
      id: "511029",
      pid: "511000",
      name: "其它区"
    }]
  }, {
    id: "511100",
    pid: "510000",
    name: "乐山市",
    children: [{
      id: "511102",
      pid: "511100",
      name: "市中区"
    }, {
      id: "511111",
      pid: "511100",
      name: "沙湾区"
    }, {
      id: "511112",
      pid: "511100",
      name: "五通桥区"
    }, {
      id: "511113",
      pid: "511100",
      name: "金口河区"
    }, {
      id: "511123",
      pid: "511100",
      name: "犍为县"
    }, {
      id: "511124",
      pid: "511100",
      name: "井研县"
    }, {
      id: "511126",
      pid: "511100",
      name: "夹江县"
    }, {
      id: "511129",
      pid: "511100",
      name: "沐川县"
    }, {
      id: "511132",
      pid: "511100",
      name: "峨边彝族自治县"
    }, {
      id: "511133",
      pid: "511100",
      name: "马边彝族自治县"
    }, {
      id: "511181",
      pid: "511100",
      name: "峨眉山市"
    }, {
      id: "511182",
      pid: "511100",
      name: "其它区"
    }]
  }, {
    id: "511300",
    pid: "510000",
    name: "南充市",
    children: [{
      id: "511302",
      pid: "511300",
      name: "顺庆区"
    }, {
      id: "511303",
      pid: "511300",
      name: "高坪区"
    }, {
      id: "511304",
      pid: "511300",
      name: "嘉陵区"
    }, {
      id: "511321",
      pid: "511300",
      name: "南部县"
    }, {
      id: "511322",
      pid: "511300",
      name: "营山县"
    }, {
      id: "511323",
      pid: "511300",
      name: "蓬安县"
    }, {
      id: "511324",
      pid: "511300",
      name: "仪陇县"
    }, {
      id: "511325",
      pid: "511300",
      name: "西充县"
    }, {
      id: "511381",
      pid: "511300",
      name: "阆中市"
    }, {
      id: "511382",
      pid: "511300",
      name: "其它区"
    }]
  }, {
    id: "511400",
    pid: "510000",
    name: "眉山市",
    children: [{
      id: "511402",
      pid: "511400",
      name: "东坡区"
    }, {
      id: "511421",
      pid: "511400",
      name: "仁寿县"
    }, {
      id: "511422",
      pid: "511400",
      name: "彭山县"
    }, {
      id: "511423",
      pid: "511400",
      name: "洪雅县"
    }, {
      id: "511424",
      pid: "511400",
      name: "丹棱县"
    }, {
      id: "511425",
      pid: "511400",
      name: "青神县"
    }, {
      id: "511426",
      pid: "511400",
      name: "其它区"
    }]
  }, {
    id: "511500",
    pid: "510000",
    name: "宜宾市",
    children: [{
      id: "511502",
      pid: "511500",
      name: "翠屏区"
    }, {
      id: "511521",
      pid: "511500",
      name: "宜宾县"
    }, {
      id: "511522",
      pid: "511500",
      name: "南溪区"
    }, {
      id: "511523",
      pid: "511500",
      name: "江安县"
    }, {
      id: "511524",
      pid: "511500",
      name: "长宁县"
    }, {
      id: "511525",
      pid: "511500",
      name: "高县"
    }, {
      id: "511526",
      pid: "511500",
      name: "珙县"
    }, {
      id: "511527",
      pid: "511500",
      name: "筠连县"
    }, {
      id: "511528",
      pid: "511500",
      name: "兴文县"
    }, {
      id: "511529",
      pid: "511500",
      name: "屏山县"
    }, {
      id: "511530",
      pid: "511500",
      name: "其它区"
    }]
  }, {
    id: "511600",
    pid: "510000",
    name: "广安市",
    children: [{
      id: "511602",
      pid: "511600",
      name: "广安区"
    }, {
      id: "511603",
      pid: "511600",
      name: "前锋区"
    }, {
      id: "511621",
      pid: "511600",
      name: "岳池县"
    }, {
      id: "511622",
      pid: "511600",
      name: "武胜县"
    }, {
      id: "511623",
      pid: "511600",
      name: "邻水县"
    }, {
      id: "511681",
      pid: "511600",
      name: "华蓥市"
    }, {
      id: "511683",
      pid: "511600",
      name: "其它区"
    }]
  }, {
    id: "511700",
    pid: "510000",
    name: "达州市",
    children: [{
      id: "511702",
      pid: "511700",
      name: "通川区"
    }, {
      id: "511721",
      pid: "511700",
      name: "达川区"
    }, {
      id: "511722",
      pid: "511700",
      name: "宣汉县"
    }, {
      id: "511723",
      pid: "511700",
      name: "开江县"
    }, {
      id: "511724",
      pid: "511700",
      name: "大竹县"
    }, {
      id: "511725",
      pid: "511700",
      name: "渠县"
    }, {
      id: "511781",
      pid: "511700",
      name: "万源市"
    }, {
      id: "511782",
      pid: "511700",
      name: "其它区"
    }]
  }, {
    id: "511800",
    pid: "510000",
    name: "雅安市",
    children: [{
      id: "511802",
      pid: "511800",
      name: "雨城区"
    }, {
      id: "511821",
      pid: "511800",
      name: "名山区"
    }, {
      id: "511822",
      pid: "511800",
      name: "荥经县"
    }, {
      id: "511823",
      pid: "511800",
      name: "汉源县"
    }, {
      id: "511824",
      pid: "511800",
      name: "石棉县"
    }, {
      id: "511825",
      pid: "511800",
      name: "天全县"
    }, {
      id: "511826",
      pid: "511800",
      name: "芦山县"
    }, {
      id: "511827",
      pid: "511800",
      name: "宝兴县"
    }, {
      id: "511828",
      pid: "511800",
      name: "其它区"
    }]
  }, {
    id: "511900",
    pid: "510000",
    name: "巴中市",
    children: [{
      id: "511902",
      pid: "511900",
      name: "巴州区"
    }, {
      id: "511903",
      pid: "511900",
      name: "恩阳区"
    }, {
      id: "511921",
      pid: "511900",
      name: "通江县"
    }, {
      id: "511922",
      pid: "511900",
      name: "南江县"
    }, {
      id: "511923",
      pid: "511900",
      name: "平昌县"
    }, {
      id: "511924",
      pid: "511900",
      name: "其它区"
    }]
  }, {
    id: "512000",
    pid: "510000",
    name: "资阳市",
    children: [{
      id: "512002",
      pid: "512000",
      name: "雁江区"
    }, {
      id: "512021",
      pid: "512000",
      name: "安岳县"
    }, {
      id: "512022",
      pid: "512000",
      name: "乐至县"
    }, {
      id: "512081",
      pid: "512000",
      name: "简阳市"
    }, {
      id: "512082",
      pid: "512000",
      name: "其它区"
    }]
  }, {
    id: "513200",
    pid: "510000",
    name: "阿坝藏族羌族自治州",
    children: [{
      id: "513221",
      pid: "513200",
      name: "汶川县"
    }, {
      id: "513222",
      pid: "513200",
      name: "理县"
    }, {
      id: "513223",
      pid: "513200",
      name: "茂县"
    }, {
      id: "513224",
      pid: "513200",
      name: "松潘县"
    }, {
      id: "513225",
      pid: "513200",
      name: "九寨沟县"
    }, {
      id: "513226",
      pid: "513200",
      name: "金川县"
    }, {
      id: "513227",
      pid: "513200",
      name: "小金县"
    }, {
      id: "513228",
      pid: "513200",
      name: "黑水县"
    }, {
      id: "513229",
      pid: "513200",
      name: "马尔康县"
    }, {
      id: "513230",
      pid: "513200",
      name: "壤塘县"
    }, {
      id: "513231",
      pid: "513200",
      name: "阿坝县"
    }, {
      id: "513232",
      pid: "513200",
      name: "若尔盖县"
    }, {
      id: "513233",
      pid: "513200",
      name: "红原县"
    }, {
      id: "513234",
      pid: "513200",
      name: "其它区"
    }]
  }, {
    id: "513300",
    pid: "510000",
    name: "甘孜藏族自治州",
    children: [{
      id: "513321",
      pid: "513300",
      name: "康定县"
    }, {
      id: "513322",
      pid: "513300",
      name: "泸定县"
    }, {
      id: "513323",
      pid: "513300",
      name: "丹巴县"
    }, {
      id: "513324",
      pid: "513300",
      name: "九龙县"
    }, {
      id: "513325",
      pid: "513300",
      name: "雅江县"
    }, {
      id: "513326",
      pid: "513300",
      name: "道孚县"
    }, {
      id: "513327",
      pid: "513300",
      name: "炉霍县"
    }, {
      id: "513328",
      pid: "513300",
      name: "甘孜县"
    }, {
      id: "513329",
      pid: "513300",
      name: "新龙县"
    }, {
      id: "513330",
      pid: "513300",
      name: "德格县"
    }, {
      id: "513331",
      pid: "513300",
      name: "白玉县"
    }, {
      id: "513332",
      pid: "513300",
      name: "石渠县"
    }, {
      id: "513333",
      pid: "513300",
      name: "色达县"
    }, {
      id: "513334",
      pid: "513300",
      name: "理塘县"
    }, {
      id: "513335",
      pid: "513300",
      name: "巴塘县"
    }, {
      id: "513336",
      pid: "513300",
      name: "乡城县"
    }, {
      id: "513337",
      pid: "513300",
      name: "稻城县"
    }, {
      id: "513338",
      pid: "513300",
      name: "得荣县"
    }, {
      id: "513339",
      pid: "513300",
      name: "其它区"
    }]
  }, {
    id: "513400",
    pid: "510000",
    name: "凉山彝族自治州",
    children: [{
      id: "513401",
      pid: "513400",
      name: "西昌市"
    }, {
      id: "513422",
      pid: "513400",
      name: "木里藏族自治县"
    }, {
      id: "513423",
      pid: "513400",
      name: "盐源县"
    }, {
      id: "513424",
      pid: "513400",
      name: "德昌县"
    }, {
      id: "513425",
      pid: "513400",
      name: "会理县"
    }, {
      id: "513426",
      pid: "513400",
      name: "会东县"
    }, {
      id: "513427",
      pid: "513400",
      name: "宁南县"
    }, {
      id: "513428",
      pid: "513400",
      name: "普格县"
    }, {
      id: "513429",
      pid: "513400",
      name: "布拖县"
    }, {
      id: "513430",
      pid: "513400",
      name: "金阳县"
    }, {
      id: "513431",
      pid: "513400",
      name: "昭觉县"
    }, {
      id: "513432",
      pid: "513400",
      name: "喜德县"
    }, {
      id: "513433",
      pid: "513400",
      name: "冕宁县"
    }, {
      id: "513434",
      pid: "513400",
      name: "越西县"
    }, {
      id: "513435",
      pid: "513400",
      name: "甘洛县"
    }, {
      id: "513436",
      pid: "513400",
      name: "美姑县"
    }, {
      id: "513437",
      pid: "513400",
      name: "雷波县"
    }, {
      id: "513438",
      pid: "513400",
      name: "其它区"
    }]
  }]
}, {
  id: "520000",
  pid: "520000",
  name: "贵州省",
  children: [{
    id: "520100",
    pid: "520000",
    name: "贵阳市",
    children: [{
      id: "520102",
      pid: "520100",
      name: "南明区"
    }, {
      id: "520103",
      pid: "520100",
      name: "云岩区"
    }, {
      id: "520111",
      pid: "520100",
      name: "花溪区"
    }, {
      id: "520112",
      pid: "520100",
      name: "乌当区"
    }, {
      id: "520113",
      pid: "520100",
      name: "白云区"
    }, {
      id: "520121",
      pid: "520100",
      name: "开阳县"
    }, {
      id: "520122",
      pid: "520100",
      name: "息烽县"
    }, {
      id: "520123",
      pid: "520100",
      name: "修文县"
    }, {
      id: "520151",
      pid: "520100",
      name: "观山湖区"
    }, {
      id: "520181",
      pid: "520100",
      name: "清镇市"
    }, {
      id: "520182",
      pid: "520100",
      name: "其它区"
    }]
  }, {
    id: "520200",
    pid: "520000",
    name: "六盘水市",
    children: [{
      id: "520201",
      pid: "520200",
      name: "钟山区"
    }, {
      id: "520203",
      pid: "520200",
      name: "六枝特区"
    }, {
      id: "520221",
      pid: "520200",
      name: "水城县"
    }, {
      id: "520222",
      pid: "520200",
      name: "盘县"
    }, {
      id: "520223",
      pid: "520200",
      name: "其它区"
    }]
  }, {
    id: "520300",
    pid: "520000",
    name: "遵义市",
    children: [{
      id: "520302",
      pid: "520300",
      name: "红花岗区"
    }, {
      id: "520303",
      pid: "520300",
      name: "汇川区"
    }, {
      id: "520321",
      pid: "520300",
      name: "遵义县"
    }, {
      id: "520322",
      pid: "520300",
      name: "桐梓县"
    }, {
      id: "520323",
      pid: "520300",
      name: "绥阳县"
    }, {
      id: "520324",
      pid: "520300",
      name: "正安县"
    }, {
      id: "520325",
      pid: "520300",
      name: "道真仡佬族苗族自治县"
    }, {
      id: "520326",
      pid: "520300",
      name: "务川仡佬族苗族自治县"
    }, {
      id: "520327",
      pid: "520300",
      name: "凤冈县"
    }, {
      id: "520328",
      pid: "520300",
      name: "湄潭县"
    }, {
      id: "520329",
      pid: "520300",
      name: "余庆县"
    }, {
      id: "520330",
      pid: "520300",
      name: "习水县"
    }, {
      id: "520381",
      pid: "520300",
      name: "赤水市"
    }, {
      id: "520382",
      pid: "520300",
      name: "仁怀市"
    }, {
      id: "520383",
      pid: "520300",
      name: "其它区"
    }]
  }, {
    id: "520400",
    pid: "520000",
    name: "安顺市",
    children: [{
      id: "520402",
      pid: "520400",
      name: "西秀区"
    }, {
      id: "520421",
      pid: "520400",
      name: "平坝县"
    }, {
      id: "520422",
      pid: "520400",
      name: "普定县"
    }, {
      id: "520423",
      pid: "520400",
      name: "镇宁布依族苗族自治县"
    }, {
      id: "520424",
      pid: "520400",
      name: "关岭布依族苗族自治县"
    }, {
      id: "520425",
      pid: "520400",
      name: "紫云苗族布依族自治县"
    }, {
      id: "520426",
      pid: "520400",
      name: "其它区"
    }]
  }, {
    id: "522200",
    pid: "520000",
    name: "铜仁市",
    children: [{
      id: "522201",
      pid: "522200",
      name: "碧江区"
    }, {
      id: "522222",
      pid: "522200",
      name: "江口县"
    }, {
      id: "522223",
      pid: "522200",
      name: "玉屏侗族自治县"
    }, {
      id: "522224",
      pid: "522200",
      name: "石阡县"
    }, {
      id: "522225",
      pid: "522200",
      name: "思南县"
    }, {
      id: "522226",
      pid: "522200",
      name: "印江土家族苗族自治县"
    }, {
      id: "522227",
      pid: "522200",
      name: "德江县"
    }, {
      id: "522228",
      pid: "522200",
      name: "沿河土家族自治县"
    }, {
      id: "522229",
      pid: "522200",
      name: "松桃苗族自治县"
    }, {
      id: "522230",
      pid: "522200",
      name: "万山区"
    }, {
      id: "522231",
      pid: "522200",
      name: "其它区"
    }]
  }, {
    id: "522300",
    pid: "520000",
    name: "黔西南布依族苗族自治州",
    children: [{
      id: "522301",
      pid: "522300",
      name: "兴义市"
    }, {
      id: "522322",
      pid: "522300",
      name: "兴仁县"
    }, {
      id: "522323",
      pid: "522300",
      name: "普安县"
    }, {
      id: "522324",
      pid: "522300",
      name: "晴隆县"
    }, {
      id: "522325",
      pid: "522300",
      name: "贞丰县"
    }, {
      id: "522326",
      pid: "522300",
      name: "望谟县"
    }, {
      id: "522327",
      pid: "522300",
      name: "册亨县"
    }, {
      id: "522328",
      pid: "522300",
      name: "安龙县"
    }, {
      id: "522329",
      pid: "522300",
      name: "其它区"
    }]
  }, {
    id: "522400",
    pid: "520000",
    name: "毕节市",
    children: [{
      id: "522401",
      pid: "522400",
      name: "七星关区"
    }, {
      id: "522422",
      pid: "522400",
      name: "大方县"
    }, {
      id: "522423",
      pid: "522400",
      name: "黔西县"
    }, {
      id: "522424",
      pid: "522400",
      name: "金沙县"
    }, {
      id: "522425",
      pid: "522400",
      name: "织金县"
    }, {
      id: "522426",
      pid: "522400",
      name: "纳雍县"
    }, {
      id: "522427",
      pid: "522400",
      name: "威宁彝族回族苗族自治县"
    }, {
      id: "522428",
      pid: "522400",
      name: "赫章县"
    }, {
      id: "522429",
      pid: "522400",
      name: "其它区"
    }]
  }, {
    id: "522600",
    pid: "520000",
    name: "黔东南苗族侗族自治州",
    children: [{
      id: "522601",
      pid: "522600",
      name: "凯里市"
    }, {
      id: "522622",
      pid: "522600",
      name: "黄平县"
    }, {
      id: "522623",
      pid: "522600",
      name: "施秉县"
    }, {
      id: "522624",
      pid: "522600",
      name: "三穗县"
    }, {
      id: "522625",
      pid: "522600",
      name: "镇远县"
    }, {
      id: "522626",
      pid: "522600",
      name: "岑巩县"
    }, {
      id: "522627",
      pid: "522600",
      name: "天柱县"
    }, {
      id: "522628",
      pid: "522600",
      name: "锦屏县"
    }, {
      id: "522629",
      pid: "522600",
      name: "剑河县"
    }, {
      id: "522630",
      pid: "522600",
      name: "台江县"
    }, {
      id: "522631",
      pid: "522600",
      name: "黎平县"
    }, {
      id: "522632",
      pid: "522600",
      name: "榕江县"
    }, {
      id: "522633",
      pid: "522600",
      name: "从江县"
    }, {
      id: "522634",
      pid: "522600",
      name: "雷山县"
    }, {
      id: "522635",
      pid: "522600",
      name: "麻江县"
    }, {
      id: "522636",
      pid: "522600",
      name: "丹寨县"
    }, {
      id: "522637",
      pid: "522600",
      name: "其它区"
    }]
  }, {
    id: "522700",
    pid: "520000",
    name: "黔南布依族苗族自治州",
    children: [{
      id: "522701",
      pid: "522700",
      name: "都匀市"
    }, {
      id: "522702",
      pid: "522700",
      name: "福泉市"
    }, {
      id: "522722",
      pid: "522700",
      name: "荔波县"
    }, {
      id: "522723",
      pid: "522700",
      name: "贵定县"
    }, {
      id: "522725",
      pid: "522700",
      name: "瓮安县"
    }, {
      id: "522726",
      pid: "522700",
      name: "独山县"
    }, {
      id: "522727",
      pid: "522700",
      name: "平塘县"
    }, {
      id: "522728",
      pid: "522700",
      name: "罗甸县"
    }, {
      id: "522729",
      pid: "522700",
      name: "长顺县"
    }, {
      id: "522730",
      pid: "522700",
      name: "龙里县"
    }, {
      id: "522731",
      pid: "522700",
      name: "惠水县"
    }, {
      id: "522732",
      pid: "522700",
      name: "三都水族自治县"
    }, {
      id: "522733",
      pid: "522700",
      name: "其它区"
    }]
  }]
}, {
  id: "530000",
  pid: "530000",
  name: "云南省",
  children: [{
    id: "530100",
    pid: "530000",
    name: "昆明市",
    children: [{
      id: "530102",
      pid: "530100",
      name: "五华区"
    }, {
      id: "530103",
      pid: "530100",
      name: "盘龙区"
    }, {
      id: "530111",
      pid: "530100",
      name: "官渡区"
    }, {
      id: "530112",
      pid: "530100",
      name: "西山区"
    }, {
      id: "530113",
      pid: "530100",
      name: "东川区"
    }, {
      id: "530121",
      pid: "530100",
      name: "呈贡区"
    }, {
      id: "530122",
      pid: "530100",
      name: "晋宁县"
    }, {
      id: "530124",
      pid: "530100",
      name: "富民县"
    }, {
      id: "530125",
      pid: "530100",
      name: "宜良县"
    }, {
      id: "530126",
      pid: "530100",
      name: "石林彝族自治县"
    }, {
      id: "530127",
      pid: "530100",
      name: "嵩明县"
    }, {
      id: "530128",
      pid: "530100",
      name: "禄劝彝族苗族自治县"
    }, {
      id: "530129",
      pid: "530100",
      name: "寻甸回族彝族自治县"
    }, {
      id: "530181",
      pid: "530100",
      name: "安宁市"
    }, {
      id: "530182",
      pid: "530100",
      name: "其它区"
    }]
  }, {
    id: "530300",
    pid: "530000",
    name: "曲靖市",
    children: [{
      id: "530302",
      pid: "530300",
      name: "麒麟区"
    }, {
      id: "530321",
      pid: "530300",
      name: "马龙县"
    }, {
      id: "530322",
      pid: "530300",
      name: "陆良县"
    }, {
      id: "530323",
      pid: "530300",
      name: "师宗县"
    }, {
      id: "530324",
      pid: "530300",
      name: "罗平县"
    }, {
      id: "530325",
      pid: "530300",
      name: "富源县"
    }, {
      id: "530326",
      pid: "530300",
      name: "会泽县"
    }, {
      id: "530328",
      pid: "530300",
      name: "沾益县"
    }, {
      id: "530381",
      pid: "530300",
      name: "宣威市"
    }, {
      id: "530382",
      pid: "530300",
      name: "其它区"
    }]
  }, {
    id: "530400",
    pid: "530000",
    name: "玉溪市",
    children: [{
      id: "530402",
      pid: "530400",
      name: "红塔区"
    }, {
      id: "530421",
      pid: "530400",
      name: "江川县"
    }, {
      id: "530422",
      pid: "530400",
      name: "澄江县"
    }, {
      id: "530423",
      pid: "530400",
      name: "通海县"
    }, {
      id: "530424",
      pid: "530400",
      name: "华宁县"
    }, {
      id: "530425",
      pid: "530400",
      name: "易门县"
    }, {
      id: "530426",
      pid: "530400",
      name: "峨山彝族自治县"
    }, {
      id: "530427",
      pid: "530400",
      name: "新平彝族傣族自治县"
    }, {
      id: "530428",
      pid: "530400",
      name: "元江哈尼族彝族傣族自治县"
    }, {
      id: "530429",
      pid: "530400",
      name: "其它区"
    }]
  }, {
    id: "530500",
    pid: "530000",
    name: "保山市",
    children: [{
      id: "530502",
      pid: "530500",
      name: "隆阳区"
    }, {
      id: "530521",
      pid: "530500",
      name: "施甸县"
    }, {
      id: "530522",
      pid: "530500",
      name: "腾冲县"
    }, {
      id: "530523",
      pid: "530500",
      name: "龙陵县"
    }, {
      id: "530524",
      pid: "530500",
      name: "昌宁县"
    }, {
      id: "530525",
      pid: "530500",
      name: "其它区"
    }]
  }, {
    id: "530600",
    pid: "530000",
    name: "昭通市",
    children: [{
      id: "530602",
      pid: "530600",
      name: "昭阳区"
    }, {
      id: "530621",
      pid: "530600",
      name: "鲁甸县"
    }, {
      id: "530622",
      pid: "530600",
      name: "巧家县"
    }, {
      id: "530623",
      pid: "530600",
      name: "盐津县"
    }, {
      id: "530624",
      pid: "530600",
      name: "大关县"
    }, {
      id: "530625",
      pid: "530600",
      name: "永善县"
    }, {
      id: "530626",
      pid: "530600",
      name: "绥江县"
    }, {
      id: "530627",
      pid: "530600",
      name: "镇雄县"
    }, {
      id: "530628",
      pid: "530600",
      name: "彝良县"
    }, {
      id: "530629",
      pid: "530600",
      name: "威信县"
    }, {
      id: "530630",
      pid: "530600",
      name: "水富县"
    }, {
      id: "530631",
      pid: "530600",
      name: "其它区"
    }]
  }, {
    id: "530700",
    pid: "530000",
    name: "丽江市",
    children: [{
      id: "530702",
      pid: "530700",
      name: "古城区"
    }, {
      id: "530721",
      pid: "530700",
      name: "玉龙纳西族自治县"
    }, {
      id: "530722",
      pid: "530700",
      name: "永胜县"
    }, {
      id: "530723",
      pid: "530700",
      name: "华坪县"
    }, {
      id: "530724",
      pid: "530700",
      name: "宁蒗彝族自治县"
    }, {
      id: "530725",
      pid: "530700",
      name: "其它区"
    }]
  }, {
    id: "530800",
    pid: "530000",
    name: "普洱市",
    children: [{
      id: "530802",
      pid: "530800",
      name: "思茅区"
    }, {
      id: "530821",
      pid: "530800",
      name: "宁洱哈尼族彝族自治县"
    }, {
      id: "530822",
      pid: "530800",
      name: "墨江哈尼族自治县"
    }, {
      id: "530823",
      pid: "530800",
      name: "景东彝族自治县"
    }, {
      id: "530824",
      pid: "530800",
      name: "景谷傣族彝族自治县"
    }, {
      id: "530825",
      pid: "530800",
      name: "镇沅彝族哈尼族拉祜族自治县"
    }, {
      id: "530826",
      pid: "530800",
      name: "江城哈尼族彝族自治县"
    }, {
      id: "530827",
      pid: "530800",
      name: "孟连傣族拉祜族佤族自治县"
    }, {
      id: "530828",
      pid: "530800",
      name: "澜沧拉祜族自治县"
    }, {
      id: "530829",
      pid: "530800",
      name: "西盟佤族自治县"
    }, {
      id: "530830",
      pid: "530800",
      name: "其它区"
    }]
  }, {
    id: "530900",
    pid: "530000",
    name: "临沧市",
    children: [{
      id: "530902",
      pid: "530900",
      name: "临翔区"
    }, {
      id: "530921",
      pid: "530900",
      name: "凤庆县"
    }, {
      id: "530922",
      pid: "530900",
      name: "云县"
    }, {
      id: "530923",
      pid: "530900",
      name: "永德县"
    }, {
      id: "530924",
      pid: "530900",
      name: "镇康县"
    }, {
      id: "530925",
      pid: "530900",
      name: "双江拉祜族佤族布朗族傣族自治县"
    }, {
      id: "530926",
      pid: "530900",
      name: "耿马傣族佤族自治县"
    }, {
      id: "530927",
      pid: "530900",
      name: "沧源佤族自治县"
    }, {
      id: "530928",
      pid: "530900",
      name: "其它区"
    }]
  }, {
    id: "532300",
    pid: "530000",
    name: "楚雄彝族自治州",
    children: [{
      id: "532301",
      pid: "532300",
      name: "楚雄市"
    }, {
      id: "532322",
      pid: "532300",
      name: "双柏县"
    }, {
      id: "532323",
      pid: "532300",
      name: "牟定县"
    }, {
      id: "532324",
      pid: "532300",
      name: "南华县"
    }, {
      id: "532325",
      pid: "532300",
      name: "姚安县"
    }, {
      id: "532326",
      pid: "532300",
      name: "大姚县"
    }, {
      id: "532327",
      pid: "532300",
      name: "永仁县"
    }, {
      id: "532328",
      pid: "532300",
      name: "元谋县"
    }, {
      id: "532329",
      pid: "532300",
      name: "武定县"
    }, {
      id: "532331",
      pid: "532300",
      name: "禄丰县"
    }, {
      id: "532332",
      pid: "532300",
      name: "其它区"
    }]
  }, {
    id: "532500",
    pid: "530000",
    name: "红河哈尼族彝族自治州",
    children: [{
      id: "532501",
      pid: "532500",
      name: "个旧市"
    }, {
      id: "532502",
      pid: "532500",
      name: "开远市"
    }, {
      id: "532522",
      pid: "532500",
      name: "蒙自市"
    }, {
      id: "532523",
      pid: "532500",
      name: "屏边苗族自治县"
    }, {
      id: "532524",
      pid: "532500",
      name: "建水县"
    }, {
      id: "532525",
      pid: "532500",
      name: "石屏县"
    }, {
      id: "532526",
      pid: "532500",
      name: "弥勒市"
    }, {
      id: "532527",
      pid: "532500",
      name: "泸西县"
    }, {
      id: "532528",
      pid: "532500",
      name: "元阳县"
    }, {
      id: "532529",
      pid: "532500",
      name: "红河县"
    }, {
      id: "532530",
      pid: "532500",
      name: "金平苗族瑶族傣族自治县"
    }, {
      id: "532531",
      pid: "532500",
      name: "绿春县"
    }, {
      id: "532532",
      pid: "532500",
      name: "河口瑶族自治县"
    }, {
      id: "532533",
      pid: "532500",
      name: "其它区"
    }]
  }, {
    id: "532600",
    pid: "530000",
    name: "文山壮族苗族自治州",
    children: [{
      id: "532621",
      pid: "532600",
      name: "文山市"
    }, {
      id: "532622",
      pid: "532600",
      name: "砚山县"
    }, {
      id: "532623",
      pid: "532600",
      name: "西畴县"
    }, {
      id: "532624",
      pid: "532600",
      name: "麻栗坡县"
    }, {
      id: "532625",
      pid: "532600",
      name: "马关县"
    }, {
      id: "532626",
      pid: "532600",
      name: "丘北县"
    }, {
      id: "532627",
      pid: "532600",
      name: "广南县"
    }, {
      id: "532628",
      pid: "532600",
      name: "富宁县"
    }, {
      id: "532629",
      pid: "532600",
      name: "其它区"
    }]
  }, {
    id: "532800",
    pid: "530000",
    name: "西双版纳傣族自治州",
    children: [{
      id: "532801",
      pid: "532800",
      name: "景洪市"
    }, {
      id: "532822",
      pid: "532800",
      name: "勐海县"
    }, {
      id: "532823",
      pid: "532800",
      name: "勐腊县"
    }, {
      id: "532824",
      pid: "532800",
      name: "其它区"
    }]
  }, {
    id: "532900",
    pid: "530000",
    name: "大理白族自治州",
    children: [{
      id: "532901",
      pid: "532900",
      name: "大理市"
    }, {
      id: "532922",
      pid: "532900",
      name: "漾濞彝族自治县"
    }, {
      id: "532923",
      pid: "532900",
      name: "祥云县"
    }, {
      id: "532924",
      pid: "532900",
      name: "宾川县"
    }, {
      id: "532925",
      pid: "532900",
      name: "弥渡县"
    }, {
      id: "532926",
      pid: "532900",
      name: "南涧彝族自治县"
    }, {
      id: "532927",
      pid: "532900",
      name: "巍山彝族回族自治县"
    }, {
      id: "532928",
      pid: "532900",
      name: "永平县"
    }, {
      id: "532929",
      pid: "532900",
      name: "云龙县"
    }, {
      id: "532930",
      pid: "532900",
      name: "洱源县"
    }, {
      id: "532931",
      pid: "532900",
      name: "剑川县"
    }, {
      id: "532932",
      pid: "532900",
      name: "鹤庆县"
    }, {
      id: "532933",
      pid: "532900",
      name: "其它区"
    }]
  }, {
    id: "533100",
    pid: "530000",
    name: "德宏傣族景颇族自治州",
    children: [{
      id: "533102",
      pid: "533100",
      name: "瑞丽市"
    }, {
      id: "533103",
      pid: "533100",
      name: "芒市"
    }, {
      id: "533122",
      pid: "533100",
      name: "梁河县"
    }, {
      id: "533123",
      pid: "533100",
      name: "盈江县"
    }, {
      id: "533124",
      pid: "533100",
      name: "陇川县"
    }, {
      id: "533125",
      pid: "533100",
      name: "其它区"
    }]
  }, {
    id: "533300",
    pid: "530000",
    name: "怒江傈僳族自治州",
    children: [{
      id: "533321",
      pid: "533300",
      name: "泸水县"
    }, {
      id: "533323",
      pid: "533300",
      name: "福贡县"
    }, {
      id: "533324",
      pid: "533300",
      name: "贡山独龙族怒族自治县"
    }, {
      id: "533325",
      pid: "533300",
      name: "兰坪白族普米族自治县"
    }, {
      id: "533326",
      pid: "533300",
      name: "其它区"
    }]
  }, {
    id: "533400",
    pid: "530000",
    name: "迪庆藏族自治州",
    children: [{
      id: "533421",
      pid: "533400",
      name: "香格里拉县"
    }, {
      id: "533422",
      pid: "533400",
      name: "德钦县"
    }, {
      id: "533423",
      pid: "533400",
      name: "维西傈僳族自治县"
    }, {
      id: "533424",
      pid: "533400",
      name: "其它区"
    }]
  }]
}, {
  id: "540000",
  pid: "540000",
  name: "西藏自治区",
  children: [{
    id: "540100",
    pid: "540000",
    name: "拉萨市",
    children: [{
      id: "540102",
      pid: "540100",
      name: "城关区"
    }, {
      id: "540121",
      pid: "540100",
      name: "林周县"
    }, {
      id: "540122",
      pid: "540100",
      name: "当雄县"
    }, {
      id: "540123",
      pid: "540100",
      name: "尼木县"
    }, {
      id: "540124",
      pid: "540100",
      name: "曲水县"
    }, {
      id: "540125",
      pid: "540100",
      name: "堆龙德庆县"
    }, {
      id: "540126",
      pid: "540100",
      name: "达孜县"
    }, {
      id: "540127",
      pid: "540100",
      name: "墨竹工卡县"
    }, {
      id: "540128",
      pid: "540100",
      name: "其它区"
    }]
  }, {
    id: "542100",
    pid: "540000",
    name: "昌都地区",
    children: [{
      id: "542121",
      pid: "542100",
      name: "昌都县"
    }, {
      id: "542122",
      pid: "542100",
      name: "江达县"
    }, {
      id: "542123",
      pid: "542100",
      name: "贡觉县"
    }, {
      id: "542124",
      pid: "542100",
      name: "类乌齐县"
    }, {
      id: "542125",
      pid: "542100",
      name: "丁青县"
    }, {
      id: "542126",
      pid: "542100",
      name: "察雅县"
    }, {
      id: "542127",
      pid: "542100",
      name: "八宿县"
    }, {
      id: "542128",
      pid: "542100",
      name: "左贡县"
    }, {
      id: "542129",
      pid: "542100",
      name: "芒康县"
    }, {
      id: "542132",
      pid: "542100",
      name: "洛隆县"
    }, {
      id: "542133",
      pid: "542100",
      name: "边坝县"
    }, {
      id: "542134",
      pid: "542100",
      name: "其它区"
    }]
  }, {
    id: "542200",
    pid: "540000",
    name: "山南地区",
    children: [{
      id: "542221",
      pid: "542200",
      name: "乃东县"
    }, {
      id: "542222",
      pid: "542200",
      name: "扎囊县"
    }, {
      id: "542223",
      pid: "542200",
      name: "贡嘎县"
    }, {
      id: "542224",
      pid: "542200",
      name: "桑日县"
    }, {
      id: "542225",
      pid: "542200",
      name: "琼结县"
    }, {
      id: "542226",
      pid: "542200",
      name: "曲松县"
    }, {
      id: "542227",
      pid: "542200",
      name: "措美县"
    }, {
      id: "542228",
      pid: "542200",
      name: "洛扎县"
    }, {
      id: "542229",
      pid: "542200",
      name: "加查县"
    }, {
      id: "542231",
      pid: "542200",
      name: "隆子县"
    }, {
      id: "542232",
      pid: "542200",
      name: "错那县"
    }, {
      id: "542233",
      pid: "542200",
      name: "浪卡子县"
    }, {
      id: "542234",
      pid: "542200",
      name: "其它区"
    }]
  }, {
    id: "542300",
    pid: "540000",
    name: "日喀则地区",
    children: [{
      id: "542301",
      pid: "542300",
      name: "日喀则市"
    }, {
      id: "542322",
      pid: "542300",
      name: "南木林县"
    }, {
      id: "542323",
      pid: "542300",
      name: "江孜县"
    }, {
      id: "542324",
      pid: "542300",
      name: "定日县"
    }, {
      id: "542325",
      pid: "542300",
      name: "萨迦县"
    }, {
      id: "542326",
      pid: "542300",
      name: "拉孜县"
    }, {
      id: "542327",
      pid: "542300",
      name: "昂仁县"
    }, {
      id: "542328",
      pid: "542300",
      name: "谢通门县"
    }, {
      id: "542329",
      pid: "542300",
      name: "白朗县"
    }, {
      id: "542330",
      pid: "542300",
      name: "仁布县"
    }, {
      id: "542331",
      pid: "542300",
      name: "康马县"
    }, {
      id: "542332",
      pid: "542300",
      name: "定结县"
    }, {
      id: "542333",
      pid: "542300",
      name: "仲巴县"
    }, {
      id: "542334",
      pid: "542300",
      name: "亚东县"
    }, {
      id: "542335",
      pid: "542300",
      name: "吉隆县"
    }, {
      id: "542336",
      pid: "542300",
      name: "聂拉木县"
    }, {
      id: "542337",
      pid: "542300",
      name: "萨嘎县"
    }, {
      id: "542338",
      pid: "542300",
      name: "岗巴县"
    }, {
      id: "542339",
      pid: "542300",
      name: "其它区"
    }]
  }, {
    id: "542400",
    pid: "540000",
    name: "那曲地区",
    children: [{
      id: "542421",
      pid: "542400",
      name: "那曲县"
    }, {
      id: "542422",
      pid: "542400",
      name: "嘉黎县"
    }, {
      id: "542423",
      pid: "542400",
      name: "比如县"
    }, {
      id: "542424",
      pid: "542400",
      name: "聂荣县"
    }, {
      id: "542425",
      pid: "542400",
      name: "安多县"
    }, {
      id: "542426",
      pid: "542400",
      name: "申扎县"
    }, {
      id: "542427",
      pid: "542400",
      name: "索县"
    }, {
      id: "542428",
      pid: "542400",
      name: "班戈县"
    }, {
      id: "542429",
      pid: "542400",
      name: "巴青县"
    }, {
      id: "542430",
      pid: "542400",
      name: "尼玛县"
    }, {
      id: "542431",
      pid: "542400",
      name: "其它区"
    }, {
      id: "542432",
      pid: "542400",
      name: "双湖县"
    }]
  }, {
    id: "542500",
    pid: "540000",
    name: "阿里地区",
    children: [{
      id: "542521",
      pid: "542500",
      name: "普兰县"
    }, {
      id: "542522",
      pid: "542500",
      name: "札达县"
    }, {
      id: "542523",
      pid: "542500",
      name: "噶尔县"
    }, {
      id: "542524",
      pid: "542500",
      name: "日土县"
    }, {
      id: "542525",
      pid: "542500",
      name: "革吉县"
    }, {
      id: "542526",
      pid: "542500",
      name: "改则县"
    }, {
      id: "542527",
      pid: "542500",
      name: "措勤县"
    }, {
      id: "542528",
      pid: "542500",
      name: "其它区"
    }]
  }, {
    id: "542600",
    pid: "540000",
    name: "林芝地区",
    children: [{
      id: "542621",
      pid: "542600",
      name: "林芝县"
    }, {
      id: "542622",
      pid: "542600",
      name: "工布江达县"
    }, {
      id: "542623",
      pid: "542600",
      name: "米林县"
    }, {
      id: "542624",
      pid: "542600",
      name: "墨脱县"
    }, {
      id: "542625",
      pid: "542600",
      name: "波密县"
    }, {
      id: "542626",
      pid: "542600",
      name: "察隅县"
    }, {
      id: "542627",
      pid: "542600",
      name: "朗县"
    }, {
      id: "542628",
      pid: "542600",
      name: "其它区"
    }]
  }]
}, {
  id: "610000",
  pid: "610000",
  name: "陕西省",
  children: [{
    id: "610100",
    pid: "610000",
    name: "西安市",
    children: [{
      id: "610102",
      pid: "610100",
      name: "新城区"
    }, {
      id: "610103",
      pid: "610100",
      name: "碑林区"
    }, {
      id: "610104",
      pid: "610100",
      name: "莲湖区"
    }, {
      id: "610111",
      pid: "610100",
      name: "灞桥区"
    }, {
      id: "610112",
      pid: "610100",
      name: "未央区"
    }, {
      id: "610113",
      pid: "610100",
      name: "雁塔区"
    }, {
      id: "610114",
      pid: "610100",
      name: "阎良区"
    }, {
      id: "610115",
      pid: "610100",
      name: "临潼区"
    }, {
      id: "610116",
      pid: "610100",
      name: "长安区"
    }, {
      id: "610122",
      pid: "610100",
      name: "蓝田县"
    }, {
      id: "610124",
      pid: "610100",
      name: "周至县"
    }, {
      id: "610125",
      pid: "610100",
      name: "户县"
    }, {
      id: "610126",
      pid: "610100",
      name: "高陵县"
    }, {
      id: "610127",
      pid: "610100",
      name: "其它区"
    }]
  }, {
    id: "610200",
    pid: "610000",
    name: "铜川市",
    children: [{
      id: "610202",
      pid: "610200",
      name: "王益区"
    }, {
      id: "610203",
      pid: "610200",
      name: "印台区"
    }, {
      id: "610204",
      pid: "610200",
      name: "耀州区"
    }, {
      id: "610222",
      pid: "610200",
      name: "宜君县"
    }, {
      id: "610223",
      pid: "610200",
      name: "其它区"
    }]
  }, {
    id: "610300",
    pid: "610000",
    name: "宝鸡市",
    children: [{
      id: "610302",
      pid: "610300",
      name: "渭滨区"
    }, {
      id: "610303",
      pid: "610300",
      name: "金台区"
    }, {
      id: "610304",
      pid: "610300",
      name: "陈仓区"
    }, {
      id: "610322",
      pid: "610300",
      name: "凤翔县"
    }, {
      id: "610323",
      pid: "610300",
      name: "岐山县"
    }, {
      id: "610324",
      pid: "610300",
      name: "扶风县"
    }, {
      id: "610326",
      pid: "610300",
      name: "眉县"
    }, {
      id: "610327",
      pid: "610300",
      name: "陇县"
    }, {
      id: "610328",
      pid: "610300",
      name: "千阳县"
    }, {
      id: "610329",
      pid: "610300",
      name: "麟游县"
    }, {
      id: "610330",
      pid: "610300",
      name: "凤县"
    }, {
      id: "610331",
      pid: "610300",
      name: "太白县"
    }, {
      id: "610332",
      pid: "610300",
      name: "其它区"
    }]
  }, {
    id: "610400",
    pid: "610000",
    name: "咸阳市",
    children: [{
      id: "610402",
      pid: "610400",
      name: "秦都区"
    }, {
      id: "610403",
      pid: "610400",
      name: "杨陵区"
    }, {
      id: "610404",
      pid: "610400",
      name: "渭城区"
    }, {
      id: "610422",
      pid: "610400",
      name: "三原县"
    }, {
      id: "610423",
      pid: "610400",
      name: "泾阳县"
    }, {
      id: "610424",
      pid: "610400",
      name: "乾县"
    }, {
      id: "610425",
      pid: "610400",
      name: "礼泉县"
    }, {
      id: "610426",
      pid: "610400",
      name: "永寿县"
    }, {
      id: "610427",
      pid: "610400",
      name: "彬县"
    }, {
      id: "610428",
      pid: "610400",
      name: "长武县"
    }, {
      id: "610429",
      pid: "610400",
      name: "旬邑县"
    }, {
      id: "610430",
      pid: "610400",
      name: "淳化县"
    }, {
      id: "610431",
      pid: "610400",
      name: "武功县"
    }, {
      id: "610481",
      pid: "610400",
      name: "兴平市"
    }, {
      id: "610482",
      pid: "610400",
      name: "其它区"
    }]
  }, {
    id: "610500",
    pid: "610000",
    name: "渭南市",
    children: [{
      id: "610502",
      pid: "610500",
      name: "临渭区"
    }, {
      id: "610521",
      pid: "610500",
      name: "华县"
    }, {
      id: "610522",
      pid: "610500",
      name: "潼关县"
    }, {
      id: "610523",
      pid: "610500",
      name: "大荔县"
    }, {
      id: "610524",
      pid: "610500",
      name: "合阳县"
    }, {
      id: "610525",
      pid: "610500",
      name: "澄城县"
    }, {
      id: "610526",
      pid: "610500",
      name: "蒲城县"
    }, {
      id: "610527",
      pid: "610500",
      name: "白水县"
    }, {
      id: "610528",
      pid: "610500",
      name: "富平县"
    }, {
      id: "610581",
      pid: "610500",
      name: "韩城市"
    }, {
      id: "610582",
      pid: "610500",
      name: "华阴市"
    }, {
      id: "610583",
      pid: "610500",
      name: "其它区"
    }]
  }, {
    id: "610600",
    pid: "610000",
    name: "延安市",
    children: [{
      id: "610602",
      pid: "610600",
      name: "宝塔区"
    }, {
      id: "610621",
      pid: "610600",
      name: "延长县"
    }, {
      id: "610622",
      pid: "610600",
      name: "延川县"
    }, {
      id: "610623",
      pid: "610600",
      name: "子长县"
    }, {
      id: "610624",
      pid: "610600",
      name: "安塞县"
    }, {
      id: "610625",
      pid: "610600",
      name: "志丹县"
    }, {
      id: "610626",
      pid: "610600",
      name: "吴起县"
    }, {
      id: "610627",
      pid: "610600",
      name: "甘泉县"
    }, {
      id: "610628",
      pid: "610600",
      name: "富县"
    }, {
      id: "610629",
      pid: "610600",
      name: "洛川县"
    }, {
      id: "610630",
      pid: "610600",
      name: "宜川县"
    }, {
      id: "610631",
      pid: "610600",
      name: "黄龙县"
    }, {
      id: "610632",
      pid: "610600",
      name: "黄陵县"
    }, {
      id: "610633",
      pid: "610600",
      name: "其它区"
    }]
  }, {
    id: "610700",
    pid: "610000",
    name: "汉中市",
    children: [{
      id: "610702",
      pid: "610700",
      name: "汉台区"
    }, {
      id: "610721",
      pid: "610700",
      name: "南郑县"
    }, {
      id: "610722",
      pid: "610700",
      name: "城固县"
    }, {
      id: "610723",
      pid: "610700",
      name: "洋县"
    }, {
      id: "610724",
      pid: "610700",
      name: "西乡县"
    }, {
      id: "610725",
      pid: "610700",
      name: "勉县"
    }, {
      id: "610726",
      pid: "610700",
      name: "宁强县"
    }, {
      id: "610727",
      pid: "610700",
      name: "略阳县"
    }, {
      id: "610728",
      pid: "610700",
      name: "镇巴县"
    }, {
      id: "610729",
      pid: "610700",
      name: "留坝县"
    }, {
      id: "610730",
      pid: "610700",
      name: "佛坪县"
    }, {
      id: "610731",
      pid: "610700",
      name: "其它区"
    }]
  }, {
    id: "610800",
    pid: "610000",
    name: "榆林市",
    children: [{
      id: "610802",
      pid: "610800",
      name: "榆阳区"
    }, {
      id: "610821",
      pid: "610800",
      name: "神木县"
    }, {
      id: "610822",
      pid: "610800",
      name: "府谷县"
    }, {
      id: "610823",
      pid: "610800",
      name: "横山县"
    }, {
      id: "610824",
      pid: "610800",
      name: "靖边县"
    }, {
      id: "610825",
      pid: "610800",
      name: "定边县"
    }, {
      id: "610826",
      pid: "610800",
      name: "绥德县"
    }, {
      id: "610827",
      pid: "610800",
      name: "米脂县"
    }, {
      id: "610828",
      pid: "610800",
      name: "佳县"
    }, {
      id: "610829",
      pid: "610800",
      name: "吴堡县"
    }, {
      id: "610830",
      pid: "610800",
      name: "清涧县"
    }, {
      id: "610831",
      pid: "610800",
      name: "子洲县"
    }, {
      id: "610832",
      pid: "610800",
      name: "其它区"
    }]
  }, {
    id: "610900",
    pid: "610000",
    name: "安康市",
    children: [{
      id: "610902",
      pid: "610900",
      name: "汉滨区"
    }, {
      id: "610921",
      pid: "610900",
      name: "汉阴县"
    }, {
      id: "610922",
      pid: "610900",
      name: "石泉县"
    }, {
      id: "610923",
      pid: "610900",
      name: "宁陕县"
    }, {
      id: "610924",
      pid: "610900",
      name: "紫阳县"
    }, {
      id: "610925",
      pid: "610900",
      name: "岚皋县"
    }, {
      id: "610926",
      pid: "610900",
      name: "平利县"
    }, {
      id: "610927",
      pid: "610900",
      name: "镇坪县"
    }, {
      id: "610928",
      pid: "610900",
      name: "旬阳县"
    }, {
      id: "610929",
      pid: "610900",
      name: "白河县"
    }, {
      id: "610930",
      pid: "610900",
      name: "其它区"
    }]
  }, {
    id: "611000",
    pid: "610000",
    name: "商洛市",
    children: [{
      id: "611002",
      pid: "611000",
      name: "商州区"
    }, {
      id: "611021",
      pid: "611000",
      name: "洛南县"
    }, {
      id: "611022",
      pid: "611000",
      name: "丹凤县"
    }, {
      id: "611023",
      pid: "611000",
      name: "商南县"
    }, {
      id: "611024",
      pid: "611000",
      name: "山阳县"
    }, {
      id: "611025",
      pid: "611000",
      name: "镇安县"
    }, {
      id: "611026",
      pid: "611000",
      name: "柞水县"
    }, {
      id: "611027",
      pid: "611000",
      name: "其它区"
    }]
  }]
}, {
  id: "620000",
  pid: "620000",
  name: "甘肃省",
  children: [{
    id: "620100",
    pid: "620000",
    name: "兰州市",
    children: [{
      id: "620102",
      pid: "620100",
      name: "城关区"
    }, {
      id: "620103",
      pid: "620100",
      name: "七里河区"
    }, {
      id: "620104",
      pid: "620100",
      name: "西固区"
    }, {
      id: "620105",
      pid: "620100",
      name: "安宁区"
    }, {
      id: "620111",
      pid: "620100",
      name: "红古区"
    }, {
      id: "620121",
      pid: "620100",
      name: "永登县"
    }, {
      id: "620122",
      pid: "620100",
      name: "皋兰县"
    }, {
      id: "620123",
      pid: "620100",
      name: "榆中县"
    }, {
      id: "620124",
      pid: "620100",
      name: "其它区"
    }]
  }, {
    id: "620200",
    pid: "620000",
    name: "嘉峪关市"
  }, {
    id: "620300",
    pid: "620000",
    name: "金昌市",
    children: [{
      id: "620302",
      pid: "620300",
      name: "金川区"
    }, {
      id: "620321",
      pid: "620300",
      name: "永昌县"
    }, {
      id: "620322",
      pid: "620300",
      name: "其它区"
    }]
  }, {
    id: "620400",
    pid: "620000",
    name: "白银市",
    children: [{
      id: "620402",
      pid: "620400",
      name: "白银区"
    }, {
      id: "620403",
      pid: "620400",
      name: "平川区"
    }, {
      id: "620421",
      pid: "620400",
      name: "靖远县"
    }, {
      id: "620422",
      pid: "620400",
      name: "会宁县"
    }, {
      id: "620423",
      pid: "620400",
      name: "景泰县"
    }, {
      id: "620424",
      pid: "620400",
      name: "其它区"
    }]
  }, {
    id: "620500",
    pid: "620000",
    name: "天水市",
    children: [{
      id: "620502",
      pid: "620500",
      name: "秦州区"
    }, {
      id: "620503",
      pid: "620500",
      name: "麦积区"
    }, {
      id: "620521",
      pid: "620500",
      name: "清水县"
    }, {
      id: "620522",
      pid: "620500",
      name: "秦安县"
    }, {
      id: "620523",
      pid: "620500",
      name: "甘谷县"
    }, {
      id: "620524",
      pid: "620500",
      name: "武山县"
    }, {
      id: "620525",
      pid: "620500",
      name: "张家川回族自治县"
    }, {
      id: "620526",
      pid: "620500",
      name: "其它区"
    }]
  }, {
    id: "620600",
    pid: "620000",
    name: "武威市",
    children: [{
      id: "620602",
      pid: "620600",
      name: "凉州区"
    }, {
      id: "620621",
      pid: "620600",
      name: "民勤县"
    }, {
      id: "620622",
      pid: "620600",
      name: "古浪县"
    }, {
      id: "620623",
      pid: "620600",
      name: "天祝藏族自治县"
    }, {
      id: "620624",
      pid: "620600",
      name: "其它区"
    }]
  }, {
    id: "620700",
    pid: "620000",
    name: "张掖市",
    children: [{
      id: "620702",
      pid: "620700",
      name: "甘州区"
    }, {
      id: "620721",
      pid: "620700",
      name: "肃南裕固族自治县"
    }, {
      id: "620722",
      pid: "620700",
      name: "民乐县"
    }, {
      id: "620723",
      pid: "620700",
      name: "临泽县"
    }, {
      id: "620724",
      pid: "620700",
      name: "高台县"
    }, {
      id: "620725",
      pid: "620700",
      name: "山丹县"
    }, {
      id: "620726",
      pid: "620700",
      name: "其它区"
    }]
  }, {
    id: "620800",
    pid: "620000",
    name: "平凉市",
    children: [{
      id: "620802",
      pid: "620800",
      name: "崆峒区"
    }, {
      id: "620821",
      pid: "620800",
      name: "泾川县"
    }, {
      id: "620822",
      pid: "620800",
      name: "灵台县"
    }, {
      id: "620823",
      pid: "620800",
      name: "崇信县"
    }, {
      id: "620824",
      pid: "620800",
      name: "华亭县"
    }, {
      id: "620825",
      pid: "620800",
      name: "庄浪县"
    }, {
      id: "620826",
      pid: "620800",
      name: "静宁县"
    }, {
      id: "620827",
      pid: "620800",
      name: "其它区"
    }]
  }, {
    id: "620900",
    pid: "620000",
    name: "酒泉市",
    children: [{
      id: "620902",
      pid: "620900",
      name: "肃州区"
    }, {
      id: "620921",
      pid: "620900",
      name: "金塔县"
    }, {
      id: "620922",
      pid: "620900",
      name: "瓜州县"
    }, {
      id: "620923",
      pid: "620900",
      name: "肃北蒙古族自治县"
    }, {
      id: "620924",
      pid: "620900",
      name: "阿克塞哈萨克族自治县"
    }, {
      id: "620981",
      pid: "620900",
      name: "玉门市"
    }, {
      id: "620982",
      pid: "620900",
      name: "敦煌市"
    }, {
      id: "620983",
      pid: "620900",
      name: "其它区"
    }]
  }, {
    id: "621000",
    pid: "620000",
    name: "庆阳市",
    children: [{
      id: "621002",
      pid: "621000",
      name: "西峰区"
    }, {
      id: "621021",
      pid: "621000",
      name: "庆城县"
    }, {
      id: "621022",
      pid: "621000",
      name: "环县"
    }, {
      id: "621023",
      pid: "621000",
      name: "华池县"
    }, {
      id: "621024",
      pid: "621000",
      name: "合水县"
    }, {
      id: "621025",
      pid: "621000",
      name: "正宁县"
    }, {
      id: "621026",
      pid: "621000",
      name: "宁县"
    }, {
      id: "621027",
      pid: "621000",
      name: "镇原县"
    }, {
      id: "621028",
      pid: "621000",
      name: "其它区"
    }]
  }, {
    id: "621100",
    pid: "620000",
    name: "定西市",
    children: [{
      id: "621102",
      pid: "621100",
      name: "安定区"
    }, {
      id: "621121",
      pid: "621100",
      name: "通渭县"
    }, {
      id: "621122",
      pid: "621100",
      name: "陇西县"
    }, {
      id: "621123",
      pid: "621100",
      name: "渭源县"
    }, {
      id: "621124",
      pid: "621100",
      name: "临洮县"
    }, {
      id: "621125",
      pid: "621100",
      name: "漳县"
    }, {
      id: "621126",
      pid: "621100",
      name: "岷县"
    }, {
      id: "621127",
      pid: "621100",
      name: "其它区"
    }]
  }, {
    id: "621200",
    pid: "620000",
    name: "陇南市",
    children: [{
      id: "621202",
      pid: "621200",
      name: "武都区"
    }, {
      id: "621221",
      pid: "621200",
      name: "成县"
    }, {
      id: "621222",
      pid: "621200",
      name: "文县"
    }, {
      id: "621223",
      pid: "621200",
      name: "宕昌县"
    }, {
      id: "621224",
      pid: "621200",
      name: "康县"
    }, {
      id: "621225",
      pid: "621200",
      name: "西和县"
    }, {
      id: "621226",
      pid: "621200",
      name: "礼县"
    }, {
      id: "621227",
      pid: "621200",
      name: "徽县"
    }, {
      id: "621228",
      pid: "621200",
      name: "两当县"
    }, {
      id: "621229",
      pid: "621200",
      name: "其它区"
    }]
  }, {
    id: "622900",
    pid: "620000",
    name: "临夏回族自治州",
    children: [{
      id: "622901",
      pid: "622900",
      name: "临夏市"
    }, {
      id: "622921",
      pid: "622900",
      name: "临夏县"
    }, {
      id: "622922",
      pid: "622900",
      name: "康乐县"
    }, {
      id: "622923",
      pid: "622900",
      name: "永靖县"
    }, {
      id: "622924",
      pid: "622900",
      name: "广河县"
    }, {
      id: "622925",
      pid: "622900",
      name: "和政县"
    }, {
      id: "622926",
      pid: "622900",
      name: "东乡族自治县"
    }, {
      id: "622927",
      pid: "622900",
      name: "积石山保安族东乡族撒拉族自治县"
    }, {
      id: "622928",
      pid: "622900",
      name: "其它区"
    }]
  }, {
    id: "623000",
    pid: "620000",
    name: "甘南藏族自治州",
    children: [{
      id: "623001",
      pid: "623000",
      name: "合作市"
    }, {
      id: "623021",
      pid: "623000",
      name: "临潭县"
    }, {
      id: "623022",
      pid: "623000",
      name: "卓尼县"
    }, {
      id: "623023",
      pid: "623000",
      name: "舟曲县"
    }, {
      id: "623024",
      pid: "623000",
      name: "迭部县"
    }, {
      id: "623025",
      pid: "623000",
      name: "玛曲县"
    }, {
      id: "623026",
      pid: "623000",
      name: "碌曲县"
    }, {
      id: "623027",
      pid: "623000",
      name: "夏河县"
    }, {
      id: "623028",
      pid: "623000",
      name: "其它区"
    }]
  }]
}, {
  id: "630000",
  pid: "630000",
  name: "青海省",
  children: [{
    id: "630100",
    pid: "630000",
    name: "西宁市",
    children: [{
      id: "630102",
      pid: "630100",
      name: "城东区"
    }, {
      id: "630103",
      pid: "630100",
      name: "城中区"
    }, {
      id: "630104",
      pid: "630100",
      name: "城西区"
    }, {
      id: "630105",
      pid: "630100",
      name: "城北区"
    }, {
      id: "630121",
      pid: "630100",
      name: "大通回族土族自治县"
    }, {
      id: "630122",
      pid: "630100",
      name: "湟中县"
    }, {
      id: "630123",
      pid: "630100",
      name: "湟源县"
    }, {
      id: "630124",
      pid: "630100",
      name: "其它区"
    }]
  }, {
    id: "632100",
    pid: "630000",
    name: "海东市",
    children: [{
      id: "632121",
      pid: "632100",
      name: "平安县"
    }, {
      id: "632122",
      pid: "632100",
      name: "民和回族土族自治县"
    }, {
      id: "632123",
      pid: "632100",
      name: "乐都区"
    }, {
      id: "632126",
      pid: "632100",
      name: "互助土族自治县"
    }, {
      id: "632127",
      pid: "632100",
      name: "化隆回族自治县"
    }, {
      id: "632128",
      pid: "632100",
      name: "循化撒拉族自治县"
    }, {
      id: "632129",
      pid: "632100",
      name: "其它区"
    }]
  }, {
    id: "632200",
    pid: "630000",
    name: "海北藏族自治州",
    children: [{
      id: "632221",
      pid: "632200",
      name: "门源回族自治县"
    }, {
      id: "632222",
      pid: "632200",
      name: "祁连县"
    }, {
      id: "632223",
      pid: "632200",
      name: "海晏县"
    }, {
      id: "632224",
      pid: "632200",
      name: "刚察县"
    }, {
      id: "632225",
      pid: "632200",
      name: "其它区"
    }]
  }, {
    id: "632300",
    pid: "630000",
    name: "黄南藏族自治州",
    children: [{
      id: "632321",
      pid: "632300",
      name: "同仁县"
    }, {
      id: "632322",
      pid: "632300",
      name: "尖扎县"
    }, {
      id: "632323",
      pid: "632300",
      name: "泽库县"
    }, {
      id: "632324",
      pid: "632300",
      name: "河南蒙古族自治县"
    }, {
      id: "632325",
      pid: "632300",
      name: "其它区"
    }]
  }, {
    id: "632500",
    pid: "630000",
    name: "海南藏族自治州",
    children: [{
      id: "632521",
      pid: "632500",
      name: "共和县"
    }, {
      id: "632522",
      pid: "632500",
      name: "同德县"
    }, {
      id: "632523",
      pid: "632500",
      name: "贵德县"
    }, {
      id: "632524",
      pid: "632500",
      name: "兴海县"
    }, {
      id: "632525",
      pid: "632500",
      name: "贵南县"
    }, {
      id: "632526",
      pid: "632500",
      name: "其它区"
    }]
  }, {
    id: "632600",
    pid: "630000",
    name: "果洛藏族自治州",
    children: [{
      id: "632621",
      pid: "632600",
      name: "玛沁县"
    }, {
      id: "632622",
      pid: "632600",
      name: "班玛县"
    }, {
      id: "632623",
      pid: "632600",
      name: "甘德县"
    }, {
      id: "632624",
      pid: "632600",
      name: "达日县"
    }, {
      id: "632625",
      pid: "632600",
      name: "久治县"
    }, {
      id: "632626",
      pid: "632600",
      name: "玛多县"
    }, {
      id: "632627",
      pid: "632600",
      name: "其它区"
    }]
  }, {
    id: "632700",
    pid: "630000",
    name: "玉树藏族自治州",
    children: [{
      id: "632721",
      pid: "632700",
      name: "玉树市"
    }, {
      id: "632722",
      pid: "632700",
      name: "杂多县"
    }, {
      id: "632723",
      pid: "632700",
      name: "称多县"
    }, {
      id: "632724",
      pid: "632700",
      name: "治多县"
    }, {
      id: "632725",
      pid: "632700",
      name: "囊谦县"
    }, {
      id: "632726",
      pid: "632700",
      name: "曲麻莱县"
    }, {
      id: "632727",
      pid: "632700",
      name: "其它区"
    }]
  }, {
    id: "632800",
    pid: "630000",
    name: "海西蒙古族藏族自治州",
    children: [{
      id: "632801",
      pid: "632800",
      name: "格尔木市"
    }, {
      id: "632802",
      pid: "632800",
      name: "德令哈市"
    }, {
      id: "632821",
      pid: "632800",
      name: "乌兰县"
    }, {
      id: "632822",
      pid: "632800",
      name: "都兰县"
    }, {
      id: "632823",
      pid: "632800",
      name: "天峻县"
    }, {
      id: "632824",
      pid: "632800",
      name: "其它区"
    }]
  }]
}, {
  id: "640000",
  pid: "640000",
  name: "宁夏回族自治区",
  children: [{
    id: "640100",
    pid: "640000",
    name: "银川市",
    children: [{
      id: "640104",
      pid: "640100",
      name: "兴庆区"
    }, {
      id: "640105",
      pid: "640100",
      name: "西夏区"
    }, {
      id: "640106",
      pid: "640100",
      name: "金凤区"
    }, {
      id: "640121",
      pid: "640100",
      name: "永宁县"
    }, {
      id: "640122",
      pid: "640100",
      name: "贺兰县"
    }, {
      id: "640181",
      pid: "640100",
      name: "灵武市"
    }, {
      id: "640182",
      pid: "640100",
      name: "其它区"
    }]
  }, {
    id: "640200",
    pid: "640000",
    name: "石嘴山市",
    children: [{
      id: "640202",
      pid: "640200",
      name: "大武口区"
    }, {
      id: "640205",
      pid: "640200",
      name: "惠农区"
    }, {
      id: "640221",
      pid: "640200",
      name: "平罗县"
    }, {
      id: "640222",
      pid: "640200",
      name: "其它区"
    }]
  }, {
    id: "640300",
    pid: "640000",
    name: "吴忠市",
    children: [{
      id: "640302",
      pid: "640300",
      name: "利通区"
    }, {
      id: "640303",
      pid: "640300",
      name: "红寺堡区"
    }, {
      id: "640323",
      pid: "640300",
      name: "盐池县"
    }, {
      id: "640324",
      pid: "640300",
      name: "同心县"
    }, {
      id: "640381",
      pid: "640300",
      name: "青铜峡市"
    }, {
      id: "640382",
      pid: "640300",
      name: "其它区"
    }]
  }, {
    id: "640400",
    pid: "640000",
    name: "固原市",
    children: [{
      id: "640402",
      pid: "640400",
      name: "原州区"
    }, {
      id: "640422",
      pid: "640400",
      name: "西吉县"
    }, {
      id: "640423",
      pid: "640400",
      name: "隆德县"
    }, {
      id: "640424",
      pid: "640400",
      name: "泾源县"
    }, {
      id: "640425",
      pid: "640400",
      name: "彭阳县"
    }, {
      id: "640426",
      pid: "640400",
      name: "其它区"
    }]
  }, {
    id: "640500",
    pid: "640000",
    name: "中卫市",
    children: [{
      id: "640502",
      pid: "640500",
      name: "沙坡头区"
    }, {
      id: "640521",
      pid: "640500",
      name: "中宁县"
    }, {
      id: "640522",
      pid: "640500",
      name: "海原县"
    }, {
      id: "640523",
      pid: "640500",
      name: "其它区"
    }]
  }]
}, {
  id: "650000",
  pid: "650000",
  name: "新疆维吾尔自治区",
  children: [{
    id: "650100",
    pid: "650000",
    name: "乌鲁木齐市",
    children: [{
      id: "650102",
      pid: "650100",
      name: "天山区"
    }, {
      id: "650103",
      pid: "650100",
      name: "沙依巴克区"
    }, {
      id: "650104",
      pid: "650100",
      name: "新市区"
    }, {
      id: "650105",
      pid: "650100",
      name: "水磨沟区"
    }, {
      id: "650106",
      pid: "650100",
      name: "头屯河区"
    }, {
      id: "650107",
      pid: "650100",
      name: "达坂城区"
    }, {
      id: "650109",
      pid: "650100",
      name: "米东区"
    }, {
      id: "650121",
      pid: "650100",
      name: "乌鲁木齐县"
    }, {
      id: "650122",
      pid: "650100",
      name: "其它区"
    }]
  }, {
    id: "650200",
    pid: "650000",
    name: "克拉玛依市",
    children: [{
      id: "650202",
      pid: "650200",
      name: "独山子区"
    }, {
      id: "650203",
      pid: "650200",
      name: "克拉玛依区"
    }, {
      id: "650204",
      pid: "650200",
      name: "白碱滩区"
    }, {
      id: "650205",
      pid: "650200",
      name: "乌尔禾区"
    }, {
      id: "650206",
      pid: "650200",
      name: "其它区"
    }]
  }, {
    id: "652100",
    pid: "650000",
    name: "吐鲁番地区",
    children: [{
      id: "652101",
      pid: "652100",
      name: "吐鲁番市"
    }, {
      id: "652122",
      pid: "652100",
      name: "鄯善县"
    }, {
      id: "652123",
      pid: "652100",
      name: "托克逊县"
    }, {
      id: "652124",
      pid: "652100",
      name: "其它区"
    }]
  }, {
    id: "652200",
    pid: "650000",
    name: "哈密地区",
    children: [{
      id: "652201",
      pid: "652200",
      name: "哈密市"
    }, {
      id: "652222",
      pid: "652200",
      name: "巴里坤哈萨克自治县"
    }, {
      id: "652223",
      pid: "652200",
      name: "伊吾县"
    }, {
      id: "652224",
      pid: "652200",
      name: "其它区"
    }]
  }, {
    id: "652300",
    pid: "650000",
    name: "昌吉回族自治州",
    children: [{
      id: "652301",
      pid: "652300",
      name: "昌吉市"
    }, {
      id: "652302",
      pid: "652300",
      name: "阜康市"
    }, {
      id: "652323",
      pid: "652300",
      name: "呼图壁县"
    }, {
      id: "652324",
      pid: "652300",
      name: "玛纳斯县"
    }, {
      id: "652325",
      pid: "652300",
      name: "奇台县"
    }, {
      id: "652327",
      pid: "652300",
      name: "吉木萨尔县"
    }, {
      id: "652328",
      pid: "652300",
      name: "木垒哈萨克自治县"
    }, {
      id: "652329",
      pid: "652300",
      name: "其它区"
    }]
  }, {
    id: "652700",
    pid: "650000",
    name: "博尔塔拉蒙古自治州",
    children: [{
      id: "652701",
      pid: "652700",
      name: "博乐市"
    }, {
      id: "652702",
      pid: "652700",
      name: "阿拉山口市"
    }, {
      id: "652722",
      pid: "652700",
      name: "精河县"
    }, {
      id: "652723",
      pid: "652700",
      name: "温泉县"
    }, {
      id: "652724",
      pid: "652700",
      name: "其它区"
    }]
  }, {
    id: "652800",
    pid: "650000",
    name: "巴音郭楞蒙古自治州",
    children: [{
      id: "652801",
      pid: "652800",
      name: "库尔勒市"
    }, {
      id: "652822",
      pid: "652800",
      name: "轮台县"
    }, {
      id: "652823",
      pid: "652800",
      name: "尉犁县"
    }, {
      id: "652824",
      pid: "652800",
      name: "若羌县"
    }, {
      id: "652825",
      pid: "652800",
      name: "且末县"
    }, {
      id: "652826",
      pid: "652800",
      name: "焉耆回族自治县"
    }, {
      id: "652827",
      pid: "652800",
      name: "和静县"
    }, {
      id: "652828",
      pid: "652800",
      name: "和硕县"
    }, {
      id: "652829",
      pid: "652800",
      name: "博湖县"
    }, {
      id: "652830",
      pid: "652800",
      name: "其它区"
    }]
  }, {
    id: "652900",
    pid: "650000",
    name: "阿克苏地区",
    children: [{
      id: "652901",
      pid: "652900",
      name: "阿克苏市"
    }, {
      id: "652922",
      pid: "652900",
      name: "温宿县"
    }, {
      id: "652923",
      pid: "652900",
      name: "库车县"
    }, {
      id: "652924",
      pid: "652900",
      name: "沙雅县"
    }, {
      id: "652925",
      pid: "652900",
      name: "新和县"
    }, {
      id: "652926",
      pid: "652900",
      name: "拜城县"
    }, {
      id: "652927",
      pid: "652900",
      name: "乌什县"
    }, {
      id: "652928",
      pid: "652900",
      name: "阿瓦提县"
    }, {
      id: "652929",
      pid: "652900",
      name: "柯坪县"
    }, {
      id: "652930",
      pid: "652900",
      name: "其它区"
    }]
  }, {
    id: "653000",
    pid: "650000",
    name: "克孜勒苏柯尔克孜自治州",
    children: [{
      id: "653001",
      pid: "653000",
      name: "阿图什市"
    }, {
      id: "653022",
      pid: "653000",
      name: "阿克陶县"
    }, {
      id: "653023",
      pid: "653000",
      name: "阿合奇县"
    }, {
      id: "653024",
      pid: "653000",
      name: "乌恰县"
    }, {
      id: "653025",
      pid: "653000",
      name: "其它区"
    }]
  }, {
    id: "653100",
    pid: "650000",
    name: "喀什地区",
    children: [{
      id: "653101",
      pid: "653100",
      name: "喀什市"
    }, {
      id: "653121",
      pid: "653100",
      name: "疏附县"
    }, {
      id: "653122",
      pid: "653100",
      name: "疏勒县"
    }, {
      id: "653123",
      pid: "653100",
      name: "英吉沙县"
    }, {
      id: "653124",
      pid: "653100",
      name: "泽普县"
    }, {
      id: "653125",
      pid: "653100",
      name: "莎车县"
    }, {
      id: "653126",
      pid: "653100",
      name: "叶城县"
    }, {
      id: "653127",
      pid: "653100",
      name: "麦盖提县"
    }, {
      id: "653128",
      pid: "653100",
      name: "岳普湖县"
    }, {
      id: "653129",
      pid: "653100",
      name: "伽师县"
    }, {
      id: "653130",
      pid: "653100",
      name: "巴楚县"
    }, {
      id: "653131",
      pid: "653100",
      name: "塔什库尔干塔吉克自治县"
    }, {
      id: "653132",
      pid: "653100",
      name: "其它区"
    }]
  }, {
    id: "653200",
    pid: "650000",
    name: "和田地区",
    children: [{
      id: "653201",
      pid: "653200",
      name: "和田市"
    }, {
      id: "653221",
      pid: "653200",
      name: "和田县"
    }, {
      id: "653222",
      pid: "653200",
      name: "墨玉县"
    }, {
      id: "653223",
      pid: "653200",
      name: "皮山县"
    }, {
      id: "653224",
      pid: "653200",
      name: "洛浦县"
    }, {
      id: "653225",
      pid: "653200",
      name: "策勒县"
    }, {
      id: "653226",
      pid: "653200",
      name: "于田县"
    }, {
      id: "653227",
      pid: "653200",
      name: "民丰县"
    }, {
      id: "653228",
      pid: "653200",
      name: "其它区"
    }]
  }, {
    id: "654000",
    pid: "650000",
    name: "伊犁哈萨克自治州",
    children: [{
      id: "654002",
      pid: "654000",
      name: "伊宁市"
    }, {
      id: "654003",
      pid: "654000",
      name: "奎屯市"
    }, {
      id: "654021",
      pid: "654000",
      name: "伊宁县"
    }, {
      id: "654022",
      pid: "654000",
      name: "察布查尔锡伯自治县"
    }, {
      id: "654023",
      pid: "654000",
      name: "霍城县"
    }, {
      id: "654024",
      pid: "654000",
      name: "巩留县"
    }, {
      id: "654025",
      pid: "654000",
      name: "新源县"
    }, {
      id: "654026",
      pid: "654000",
      name: "昭苏县"
    }, {
      id: "654027",
      pid: "654000",
      name: "特克斯县"
    }, {
      id: "654028",
      pid: "654000",
      name: "尼勒克县"
    }, {
      id: "654029",
      pid: "654000",
      name: "其它区"
    }]
  }, {
    id: "654200",
    pid: "650000",
    name: "塔城地区",
    children: [{
      id: "654201",
      pid: "654200",
      name: "塔城市"
    }, {
      id: "654202",
      pid: "654200",
      name: "乌苏市"
    }, {
      id: "654221",
      pid: "654200",
      name: "额敏县"
    }, {
      id: "654223",
      pid: "654200",
      name: "沙湾县"
    }, {
      id: "654224",
      pid: "654200",
      name: "托里县"
    }, {
      id: "654225",
      pid: "654200",
      name: "裕民县"
    }, {
      id: "654226",
      pid: "654200",
      name: "和布克赛尔蒙古自治县"
    }, {
      id: "654227",
      pid: "654200",
      name: "其它区"
    }]
  }, {
    id: "654300",
    pid: "650000",
    name: "阿勒泰地区",
    children: [{
      id: "654301",
      pid: "654300",
      name: "阿勒泰市"
    }, {
      id: "654321",
      pid: "654300",
      name: "布尔津县"
    }, {
      id: "654322",
      pid: "654300",
      name: "富蕴县"
    }, {
      id: "654323",
      pid: "654300",
      name: "福海县"
    }, {
      id: "654324",
      pid: "654300",
      name: "哈巴河县"
    }, {
      id: "654325",
      pid: "654300",
      name: "青河县"
    }, {
      id: "654326",
      pid: "654300",
      name: "吉木乃县"
    }, {
      id: "654327",
      pid: "654300",
      name: "其它区"
    }]
  }, {
    id: "659001",
    pid: "650000",
    name: "石河子市"
  }, {
    id: "659002",
    pid: "650000",
    name: "阿拉尔市"
  }, {
    id: "659003",
    pid: "650000",
    name: "图木舒克市"
  }, {
    id: "659004",
    pid: "650000",
    name: "五家渠市"
  }]
}, {
  id: "710000",
  pid: "710000",
  name: "台湾",
  children: [{
    id: "710100",
    pid: "710000",
    name: "台北市",
    children: [{
      id: "710101",
      pid: "710100",
      name: "中正区"
    }, {
      id: "710102",
      pid: "710100",
      name: "大同区"
    }, {
      id: "710103",
      pid: "710100",
      name: "中山区"
    }, {
      id: "710104",
      pid: "710100",
      name: "松山区"
    }, {
      id: "710105",
      pid: "710100",
      name: "大安区"
    }, {
      id: "710106",
      pid: "710100",
      name: "万华区"
    }, {
      id: "710107",
      pid: "710100",
      name: "信义区"
    }, {
      id: "710108",
      pid: "710100",
      name: "士林区"
    }, {
      id: "710109",
      pid: "710100",
      name: "北投区"
    }, {
      id: "710110",
      pid: "710100",
      name: "内湖区"
    }, {
      id: "710111",
      pid: "710100",
      name: "南港区"
    }, {
      id: "710112",
      pid: "710100",
      name: "文山区"
    }, {
      id: "710113",
      pid: "710100",
      name: "其它区"
    }]
  }, {
    id: "710200",
    pid: "710000",
    name: "高雄市",
    children: [{
      id: "710201",
      pid: "710200",
      name: "新兴区"
    }, {
      id: "710202",
      pid: "710200",
      name: "前金区"
    }, {
      id: "710203",
      pid: "710200",
      name: "芩雅区"
    }, {
      id: "710204",
      pid: "710200",
      name: "盐埕区"
    }, {
      id: "710205",
      pid: "710200",
      name: "鼓山区"
    }, {
      id: "710206",
      pid: "710200",
      name: "旗津区"
    }, {
      id: "710207",
      pid: "710200",
      name: "前镇区"
    }, {
      id: "710208",
      pid: "710200",
      name: "三民区"
    }, {
      id: "710209",
      pid: "710200",
      name: "左营区"
    }, {
      id: "710210",
      pid: "710200",
      name: "楠梓区"
    }, {
      id: "710211",
      pid: "710200",
      name: "小港区"
    }, {
      id: "710212",
      pid: "710200",
      name: "其它区"
    }, {
      id: "710241",
      pid: "710200",
      name: "苓雅区"
    }, {
      id: "710242",
      pid: "710200",
      name: "仁武区"
    }, {
      id: "710243",
      pid: "710200",
      name: "大社区"
    }, {
      id: "710244",
      pid: "710200",
      name: "冈山区"
    }, {
      id: "710245",
      pid: "710200",
      name: "路竹区"
    }, {
      id: "710246",
      pid: "710200",
      name: "阿莲区"
    }, {
      id: "710247",
      pid: "710200",
      name: "田寮区"
    }, {
      id: "710248",
      pid: "710200",
      name: "燕巢区"
    }, {
      id: "710249",
      pid: "710200",
      name: "桥头区"
    }, {
      id: "710250",
      pid: "710200",
      name: "梓官区"
    }, {
      id: "710251",
      pid: "710200",
      name: "弥陀区"
    }, {
      id: "710252",
      pid: "710200",
      name: "永安区"
    }, {
      id: "710253",
      pid: "710200",
      name: "湖内区"
    }, {
      id: "710254",
      pid: "710200",
      name: "凤山区"
    }, {
      id: "710255",
      pid: "710200",
      name: "大寮区"
    }, {
      id: "710256",
      pid: "710200",
      name: "林园区"
    }, {
      id: "710257",
      pid: "710200",
      name: "鸟松区"
    }, {
      id: "710258",
      pid: "710200",
      name: "大树区"
    }, {
      id: "710259",
      pid: "710200",
      name: "旗山区"
    }, {
      id: "710260",
      pid: "710200",
      name: "美浓区"
    }, {
      id: "710261",
      pid: "710200",
      name: "六龟区"
    }, {
      id: "710262",
      pid: "710200",
      name: "内门区"
    }, {
      id: "710263",
      pid: "710200",
      name: "杉林区"
    }, {
      id: "710264",
      pid: "710200",
      name: "甲仙区"
    }, {
      id: "710265",
      pid: "710200",
      name: "桃源区"
    }, {
      id: "710266",
      pid: "710200",
      name: "那玛夏区"
    }, {
      id: "710267",
      pid: "710200",
      name: "茂林区"
    }, {
      id: "710268",
      pid: "710200",
      name: "茄萣区"
    }]
  }, {
    id: "710300",
    pid: "710000",
    name: "台南市",
    children: [{
      id: "710301",
      pid: "710300",
      name: "中西区"
    }, {
      id: "710302",
      pid: "710300",
      name: "东区"
    }, {
      id: "710303",
      pid: "710300",
      name: "南区"
    }, {
      id: "710304",
      pid: "710300",
      name: "北区"
    }, {
      id: "710305",
      pid: "710300",
      name: "安平区"
    }, {
      id: "710306",
      pid: "710300",
      name: "安南区"
    }, {
      id: "710307",
      pid: "710300",
      name: "其它区"
    }, {
      id: "710339",
      pid: "710300",
      name: "永康区"
    }, {
      id: "710340",
      pid: "710300",
      name: "归仁区"
    }, {
      id: "710341",
      pid: "710300",
      name: "新化区"
    }, {
      id: "710342",
      pid: "710300",
      name: "左镇区"
    }, {
      id: "710343",
      pid: "710300",
      name: "玉井区"
    }, {
      id: "710344",
      pid: "710300",
      name: "楠西区"
    }, {
      id: "710345",
      pid: "710300",
      name: "南化区"
    }, {
      id: "710346",
      pid: "710300",
      name: "仁德区"
    }, {
      id: "710347",
      pid: "710300",
      name: "关庙区"
    }, {
      id: "710348",
      pid: "710300",
      name: "龙崎区"
    }, {
      id: "710349",
      pid: "710300",
      name: "官田区"
    }, {
      id: "710350",
      pid: "710300",
      name: "麻豆区"
    }, {
      id: "710351",
      pid: "710300",
      name: "佳里区"
    }, {
      id: "710352",
      pid: "710300",
      name: "西港区"
    }, {
      id: "710353",
      pid: "710300",
      name: "七股区"
    }, {
      id: "710354",
      pid: "710300",
      name: "将军区"
    }, {
      id: "710355",
      pid: "710300",
      name: "学甲区"
    }, {
      id: "710356",
      pid: "710300",
      name: "北门区"
    }, {
      id: "710357",
      pid: "710300",
      name: "新营区"
    }, {
      id: "710358",
      pid: "710300",
      name: "后壁区"
    }, {
      id: "710359",
      pid: "710300",
      name: "白河区"
    }, {
      id: "710360",
      pid: "710300",
      name: "东山区"
    }, {
      id: "710361",
      pid: "710300",
      name: "六甲区"
    }, {
      id: "710362",
      pid: "710300",
      name: "下营区"
    }, {
      id: "710363",
      pid: "710300",
      name: "柳营区"
    }, {
      id: "710364",
      pid: "710300",
      name: "盐水区"
    }, {
      id: "710365",
      pid: "710300",
      name: "善化区"
    }, {
      id: "710366",
      pid: "710300",
      name: "大内区"
    }, {
      id: "710367",
      pid: "710300",
      name: "山上区"
    }, {
      id: "710368",
      pid: "710300",
      name: "新市区"
    }, {
      id: "710369",
      pid: "710300",
      name: "安定区"
    }]
  }, {
    id: "710400",
    pid: "710000",
    name: "台中市",
    children: [{
      id: "710401",
      pid: "710400",
      name: "中区"
    }, {
      id: "710402",
      pid: "710400",
      name: "东区"
    }, {
      id: "710403",
      pid: "710400",
      name: "南区"
    }, {
      id: "710404",
      pid: "710400",
      name: "西区"
    }, {
      id: "710405",
      pid: "710400",
      name: "北区"
    }, {
      id: "710406",
      pid: "710400",
      name: "北屯区"
    }, {
      id: "710407",
      pid: "710400",
      name: "西屯区"
    }, {
      id: "710408",
      pid: "710400",
      name: "南屯区"
    }, {
      id: "710409",
      pid: "710400",
      name: "其它区"
    }, {
      id: "710431",
      pid: "710400",
      name: "太平区"
    }, {
      id: "710432",
      pid: "710400",
      name: "大里区"
    }, {
      id: "710433",
      pid: "710400",
      name: "雾峰区"
    }, {
      id: "710434",
      pid: "710400",
      name: "乌日区"
    }, {
      id: "710435",
      pid: "710400",
      name: "丰原区"
    }, {
      id: "710436",
      pid: "710400",
      name: "后里区"
    }, {
      id: "710437",
      pid: "710400",
      name: "石冈区"
    }, {
      id: "710438",
      pid: "710400",
      name: "东势区"
    }, {
      id: "710439",
      pid: "710400",
      name: "和平区"
    }, {
      id: "710440",
      pid: "710400",
      name: "新社区"
    }, {
      id: "710441",
      pid: "710400",
      name: "潭子区"
    }, {
      id: "710442",
      pid: "710400",
      name: "大雅区"
    }, {
      id: "710443",
      pid: "710400",
      name: "神冈区"
    }, {
      id: "710444",
      pid: "710400",
      name: "大肚区"
    }, {
      id: "710445",
      pid: "710400",
      name: "沙鹿区"
    }, {
      id: "710446",
      pid: "710400",
      name: "龙井区"
    }, {
      id: "710447",
      pid: "710400",
      name: "梧栖区"
    }, {
      id: "710448",
      pid: "710400",
      name: "清水区"
    }, {
      id: "710449",
      pid: "710400",
      name: "大甲区"
    }, {
      id: "710450",
      pid: "710400",
      name: "外埔区"
    }, {
      id: "710451",
      pid: "710400",
      name: "大安区"
    }]
  }, {
    id: "710500",
    pid: "710000",
    name: "金门县",
    children: [{
      id: "710507",
      pid: "710500",
      name: "金沙镇"
    }, {
      id: "710508",
      pid: "710500",
      name: "金湖镇"
    }, {
      id: "710509",
      pid: "710500",
      name: "金宁乡"
    }, {
      id: "710510",
      pid: "710500",
      name: "金城镇"
    }, {
      id: "710511",
      pid: "710500",
      name: "烈屿乡"
    }, {
      id: "710512",
      pid: "710500",
      name: "乌坵乡"
    }]
  }, {
    id: "710600",
    pid: "710000",
    name: "南投县",
    children: [{
      id: "710614",
      pid: "710600",
      name: "南投市"
    }, {
      id: "710615",
      pid: "710600",
      name: "中寮乡"
    }, {
      id: "710616",
      pid: "710600",
      name: "草屯镇"
    }, {
      id: "710617",
      pid: "710600",
      name: "国姓乡"
    }, {
      id: "710618",
      pid: "710600",
      name: "埔里镇"
    }, {
      id: "710619",
      pid: "710600",
      name: "仁爱乡"
    }, {
      id: "710620",
      pid: "710600",
      name: "名间乡"
    }, {
      id: "710621",
      pid: "710600",
      name: "集集镇"
    }, {
      id: "710622",
      pid: "710600",
      name: "水里乡"
    }, {
      id: "710623",
      pid: "710600",
      name: "鱼池乡"
    }, {
      id: "710624",
      pid: "710600",
      name: "信义乡"
    }, {
      id: "710625",
      pid: "710600",
      name: "竹山镇"
    }, {
      id: "710626",
      pid: "710600",
      name: "鹿谷乡"
    }]
  }, {
    id: "710700",
    pid: "710000",
    name: "基隆市",
    children: [{
      id: "710701",
      pid: "710700",
      name: "仁爱区"
    }, {
      id: "710702",
      pid: "710700",
      name: "信义区"
    }, {
      id: "710703",
      pid: "710700",
      name: "中正区"
    }, {
      id: "710704",
      pid: "710700",
      name: "中山区"
    }, {
      id: "710705",
      pid: "710700",
      name: "安乐区"
    }, {
      id: "710706",
      pid: "710700",
      name: "暖暖区"
    }, {
      id: "710707",
      pid: "710700",
      name: "七堵区"
    }, {
      id: "710708",
      pid: "710700",
      name: "其它区"
    }]
  }, {
    id: "710800",
    pid: "710000",
    name: "新竹市",
    children: [{
      id: "710801",
      pid: "710800",
      name: "东区"
    }, {
      id: "710802",
      pid: "710800",
      name: "北区"
    }, {
      id: "710803",
      pid: "710800",
      name: "香山区"
    }, {
      id: "710804",
      pid: "710800",
      name: "其它区"
    }]
  }, {
    id: "710900",
    pid: "710000",
    name: "嘉义市",
    children: [{
      id: "710901",
      pid: "710900",
      name: "东区"
    }, {
      id: "710902",
      pid: "710900",
      name: "西区"
    }, {
      id: "710903",
      pid: "710900",
      name: "其它区"
    }]
  }, {
    id: "711100",
    pid: "710000",
    name: "新北市",
    children: [{
      id: "711130",
      pid: "711100",
      name: "万里区"
    }, {
      id: "711131",
      pid: "711100",
      name: "金山区"
    }, {
      id: "711132",
      pid: "711100",
      name: "板桥区"
    }, {
      id: "711133",
      pid: "711100",
      name: "汐止区"
    }, {
      id: "711134",
      pid: "711100",
      name: "深坑区"
    }, {
      id: "711135",
      pid: "711100",
      name: "石碇区"
    }, {
      id: "711136",
      pid: "711100",
      name: "瑞芳区"
    }, {
      id: "711137",
      pid: "711100",
      name: "平溪区"
    }, {
      id: "711138",
      pid: "711100",
      name: "双溪区"
    }, {
      id: "711139",
      pid: "711100",
      name: "贡寮区"
    }, {
      id: "711140",
      pid: "711100",
      name: "新店区"
    }, {
      id: "711141",
      pid: "711100",
      name: "坪林区"
    }, {
      id: "711142",
      pid: "711100",
      name: "乌来区"
    }, {
      id: "711143",
      pid: "711100",
      name: "永和区"
    }, {
      id: "711144",
      pid: "711100",
      name: "中和区"
    }, {
      id: "711145",
      pid: "711100",
      name: "土城区"
    }, {
      id: "711146",
      pid: "711100",
      name: "三峡区"
    }, {
      id: "711147",
      pid: "711100",
      name: "树林区"
    }, {
      id: "711148",
      pid: "711100",
      name: "莺歌区"
    }, {
      id: "711149",
      pid: "711100",
      name: "三重区"
    }, {
      id: "711150",
      pid: "711100",
      name: "新庄区"
    }, {
      id: "711151",
      pid: "711100",
      name: "泰山区"
    }, {
      id: "711152",
      pid: "711100",
      name: "林口区"
    }, {
      id: "711153",
      pid: "711100",
      name: "芦洲区"
    }, {
      id: "711154",
      pid: "711100",
      name: "五股区"
    }, {
      id: "711155",
      pid: "711100",
      name: "八里区"
    }, {
      id: "711156",
      pid: "711100",
      name: "淡水区"
    }, {
      id: "711157",
      pid: "711100",
      name: "三芝区"
    }, {
      id: "711158",
      pid: "711100",
      name: "石门区"
    }]
  }, {
    id: "711200",
    pid: "710000",
    name: "宜兰县",
    children: [{
      id: "711214",
      pid: "711200",
      name: "宜兰市"
    }, {
      id: "711215",
      pid: "711200",
      name: "头城镇"
    }, {
      id: "711216",
      pid: "711200",
      name: "礁溪乡"
    }, {
      id: "711217",
      pid: "711200",
      name: "壮围乡"
    }, {
      id: "711218",
      pid: "711200",
      name: "员山乡"
    }, {
      id: "711219",
      pid: "711200",
      name: "罗东镇"
    }, {
      id: "711220",
      pid: "711200",
      name: "三星乡"
    }, {
      id: "711221",
      pid: "711200",
      name: "大同乡"
    }, {
      id: "711222",
      pid: "711200",
      name: "五结乡"
    }, {
      id: "711223",
      pid: "711200",
      name: "冬山乡"
    }, {
      id: "711224",
      pid: "711200",
      name: "苏澳镇"
    }, {
      id: "711225",
      pid: "711200",
      name: "南澳乡"
    }, {
      id: "711226",
      pid: "711200",
      name: "钓鱼台"
    }]
  }, {
    id: "711300",
    pid: "710000",
    name: "新竹县",
    children: [{
      id: "711314",
      pid: "711300",
      name: "竹北市"
    }, {
      id: "711315",
      pid: "711300",
      name: "湖口乡"
    }, {
      id: "711316",
      pid: "711300",
      name: "新丰乡"
    }, {
      id: "711317",
      pid: "711300",
      name: "新埔镇"
    }, {
      id: "711318",
      pid: "711300",
      name: "关西镇"
    }, {
      id: "711319",
      pid: "711300",
      name: "芎林乡"
    }, {
      id: "711320",
      pid: "711300",
      name: "宝山乡"
    }, {
      id: "711321",
      pid: "711300",
      name: "竹东镇"
    }, {
      id: "711322",
      pid: "711300",
      name: "五峰乡"
    }, {
      id: "711323",
      pid: "711300",
      name: "横山乡"
    }, {
      id: "711324",
      pid: "711300",
      name: "尖石乡"
    }, {
      id: "711325",
      pid: "711300",
      name: "北埔乡"
    }, {
      id: "711326",
      pid: "711300",
      name: "峨眉乡"
    }]
  }, {
    id: "711400",
    pid: "710000",
    name: "桃园县",
    children: [{
      id: "711414",
      pid: "711400",
      name: "中坜市"
    }, {
      id: "711415",
      pid: "711400",
      name: "平镇市"
    }, {
      id: "711416",
      pid: "711400",
      name: "龙潭乡"
    }, {
      id: "711417",
      pid: "711400",
      name: "杨梅市"
    }, {
      id: "711418",
      pid: "711400",
      name: "新屋乡"
    }, {
      id: "711419",
      pid: "711400",
      name: "观音乡"
    }, {
      id: "711420",
      pid: "711400",
      name: "桃园市"
    }, {
      id: "711421",
      pid: "711400",
      name: "龟山乡"
    }, {
      id: "711422",
      pid: "711400",
      name: "八德市"
    }, {
      id: "711423",
      pid: "711400",
      name: "大溪镇"
    }, {
      id: "711424",
      pid: "711400",
      name: "复兴乡"
    }, {
      id: "711425",
      pid: "711400",
      name: "大园乡"
    }, {
      id: "711426",
      pid: "711400",
      name: "芦竹乡"
    }]
  }, {
    id: "711500",
    pid: "710000",
    name: "苗栗县",
    children: [{
      id: "711519",
      pid: "711500",
      name: "竹南镇"
    }, {
      id: "711520",
      pid: "711500",
      name: "头份镇"
    }, {
      id: "711521",
      pid: "711500",
      name: "三湾乡"
    }, {
      id: "711522",
      pid: "711500",
      name: "南庄乡"
    }, {
      id: "711523",
      pid: "711500",
      name: "狮潭乡"
    }, {
      id: "711524",
      pid: "711500",
      name: "后龙镇"
    }, {
      id: "711525",
      pid: "711500",
      name: "通霄镇"
    }, {
      id: "711526",
      pid: "711500",
      name: "苑里镇"
    }, {
      id: "711527",
      pid: "711500",
      name: "苗栗市"
    }, {
      id: "711528",
      pid: "711500",
      name: "造桥乡"
    }, {
      id: "711529",
      pid: "711500",
      name: "头屋乡"
    }, {
      id: "711530",
      pid: "711500",
      name: "公馆乡"
    }, {
      id: "711531",
      pid: "711500",
      name: "大湖乡"
    }, {
      id: "711532",
      pid: "711500",
      name: "泰安乡"
    }, {
      id: "711533",
      pid: "711500",
      name: "铜锣乡"
    }, {
      id: "711534",
      pid: "711500",
      name: "三义乡"
    }, {
      id: "711535",
      pid: "711500",
      name: "西湖乡"
    }, {
      id: "711536",
      pid: "711500",
      name: "卓兰镇"
    }]
  }, {
    id: "711700",
    pid: "710000",
    name: "彰化县",
    children: [{
      id: "711727",
      pid: "711700",
      name: "彰化市"
    }, {
      id: "711728",
      pid: "711700",
      name: "芬园乡"
    }, {
      id: "711729",
      pid: "711700",
      name: "花坛乡"
    }, {
      id: "711730",
      pid: "711700",
      name: "秀水乡"
    }, {
      id: "711731",
      pid: "711700",
      name: "鹿港镇"
    }, {
      id: "711732",
      pid: "711700",
      name: "福兴乡"
    }, {
      id: "711733",
      pid: "711700",
      name: "线西乡"
    }, {
      id: "711734",
      pid: "711700",
      name: "和美镇"
    }, {
      id: "711735",
      pid: "711700",
      name: "伸港乡"
    }, {
      id: "711736",
      pid: "711700",
      name: "员林镇"
    }, {
      id: "711737",
      pid: "711700",
      name: "社头乡"
    }, {
      id: "711738",
      pid: "711700",
      name: "永靖乡"
    }, {
      id: "711739",
      pid: "711700",
      name: "埔心乡"
    }, {
      id: "711740",
      pid: "711700",
      name: "溪湖镇"
    }, {
      id: "711741",
      pid: "711700",
      name: "大村乡"
    }, {
      id: "711742",
      pid: "711700",
      name: "埔盐乡"
    }, {
      id: "711743",
      pid: "711700",
      name: "田中镇"
    }, {
      id: "711744",
      pid: "711700",
      name: "北斗镇"
    }, {
      id: "711745",
      pid: "711700",
      name: "田尾乡"
    }, {
      id: "711746",
      pid: "711700",
      name: "埤头乡"
    }, {
      id: "711747",
      pid: "711700",
      name: "溪州乡"
    }, {
      id: "711748",
      pid: "711700",
      name: "竹塘乡"
    }, {
      id: "711749",
      pid: "711700",
      name: "二林镇"
    }, {
      id: "711750",
      pid: "711700",
      name: "大城乡"
    }, {
      id: "711751",
      pid: "711700",
      name: "芳苑乡"
    }, {
      id: "711752",
      pid: "711700",
      name: "二水乡"
    }]
  }, {
    id: "711900",
    pid: "710000",
    name: "嘉义县",
    children: [{
      id: "711919",
      pid: "711900",
      name: "番路乡"
    }, {
      id: "711920",
      pid: "711900",
      name: "梅山乡"
    }, {
      id: "711921",
      pid: "711900",
      name: "竹崎乡"
    }, {
      id: "711922",
      pid: "711900",
      name: "阿里山乡"
    }, {
      id: "711923",
      pid: "711900",
      name: "中埔乡"
    }, {
      id: "711924",
      pid: "711900",
      name: "大埔乡"
    }, {
      id: "711925",
      pid: "711900",
      name: "水上乡"
    }, {
      id: "711926",
      pid: "711900",
      name: "鹿草乡"
    }, {
      id: "711927",
      pid: "711900",
      name: "太保市"
    }, {
      id: "711928",
      pid: "711900",
      name: "朴子市"
    }, {
      id: "711929",
      pid: "711900",
      name: "东石乡"
    }, {
      id: "711930",
      pid: "711900",
      name: "六脚乡"
    }, {
      id: "711931",
      pid: "711900",
      name: "新港乡"
    }, {
      id: "711932",
      pid: "711900",
      name: "民雄乡"
    }, {
      id: "711933",
      pid: "711900",
      name: "大林镇"
    }, {
      id: "711934",
      pid: "711900",
      name: "溪口乡"
    }, {
      id: "711935",
      pid: "711900",
      name: "义竹乡"
    }, {
      id: "711936",
      pid: "711900",
      name: "布袋镇"
    }]
  }, {
    id: "712100",
    pid: "710000",
    name: "云林县",
    children: [{
      id: "712121",
      pid: "712100",
      name: "斗南镇"
    }, {
      id: "712122",
      pid: "712100",
      name: "大埤乡"
    }, {
      id: "712123",
      pid: "712100",
      name: "虎尾镇"
    }, {
      id: "712124",
      pid: "712100",
      name: "土库镇"
    }, {
      id: "712125",
      pid: "712100",
      name: "褒忠乡"
    }, {
      id: "712126",
      pid: "712100",
      name: "东势乡"
    }, {
      id: "712127",
      pid: "712100",
      name: "台西乡"
    }, {
      id: "712128",
      pid: "712100",
      name: "仑背乡"
    }, {
      id: "712129",
      pid: "712100",
      name: "麦寮乡"
    }, {
      id: "712130",
      pid: "712100",
      name: "斗六市"
    }, {
      id: "712131",
      pid: "712100",
      name: "林内乡"
    }, {
      id: "712132",
      pid: "712100",
      name: "古坑乡"
    }, {
      id: "712133",
      pid: "712100",
      name: "莿桐乡"
    }, {
      id: "712134",
      pid: "712100",
      name: "西螺镇"
    }, {
      id: "712135",
      pid: "712100",
      name: "二仑乡"
    }, {
      id: "712136",
      pid: "712100",
      name: "北港镇"
    }, {
      id: "712137",
      pid: "712100",
      name: "水林乡"
    }, {
      id: "712138",
      pid: "712100",
      name: "口湖乡"
    }, {
      id: "712139",
      pid: "712100",
      name: "四湖乡"
    }, {
      id: "712140",
      pid: "712100",
      name: "元长乡"
    }]
  }, {
    id: "712400",
    pid: "710000",
    name: "屏东县",
    children: [{
      id: "712434",
      pid: "712400",
      name: "屏东市"
    }, {
      id: "712435",
      pid: "712400",
      name: "三地门乡"
    }, {
      id: "712436",
      pid: "712400",
      name: "雾台乡"
    }, {
      id: "712437",
      pid: "712400",
      name: "玛家乡"
    }, {
      id: "712438",
      pid: "712400",
      name: "九如乡"
    }, {
      id: "712439",
      pid: "712400",
      name: "里港乡"
    }, {
      id: "712440",
      pid: "712400",
      name: "高树乡"
    }, {
      id: "712441",
      pid: "712400",
      name: "盐埔乡"
    }, {
      id: "712442",
      pid: "712400",
      name: "长治乡"
    }, {
      id: "712443",
      pid: "712400",
      name: "麟洛乡"
    }, {
      id: "712444",
      pid: "712400",
      name: "竹田乡"
    }, {
      id: "712445",
      pid: "712400",
      name: "内埔乡"
    }, {
      id: "712446",
      pid: "712400",
      name: "万丹乡"
    }, {
      id: "712447",
      pid: "712400",
      name: "潮州镇"
    }, {
      id: "712448",
      pid: "712400",
      name: "泰武乡"
    }, {
      id: "712449",
      pid: "712400",
      name: "来义乡"
    }, {
      id: "712450",
      pid: "712400",
      name: "万峦乡"
    }, {
      id: "712451",
      pid: "712400",
      name: "崁顶乡"
    }, {
      id: "712452",
      pid: "712400",
      name: "新埤乡"
    }, {
      id: "712453",
      pid: "712400",
      name: "南州乡"
    }, {
      id: "712454",
      pid: "712400",
      name: "林边乡"
    }, {
      id: "712455",
      pid: "712400",
      name: "东港镇"
    }, {
      id: "712456",
      pid: "712400",
      name: "琉球乡"
    }, {
      id: "712457",
      pid: "712400",
      name: "佳冬乡"
    }, {
      id: "712458",
      pid: "712400",
      name: "新园乡"
    }, {
      id: "712459",
      pid: "712400",
      name: "枋寮乡"
    }, {
      id: "712460",
      pid: "712400",
      name: "枋山乡"
    }, {
      id: "712461",
      pid: "712400",
      name: "春日乡"
    }, {
      id: "712462",
      pid: "712400",
      name: "狮子乡"
    }, {
      id: "712463",
      pid: "712400",
      name: "车城乡"
    }, {
      id: "712464",
      pid: "712400",
      name: "牡丹乡"
    }, {
      id: "712465",
      pid: "712400",
      name: "恒春镇"
    }, {
      id: "712466",
      pid: "712400",
      name: "满州乡"
    }]
  }, {
    id: "712500",
    pid: "710000",
    name: "台东县",
    children: [{
      id: "712517",
      pid: "712500",
      name: "台东市"
    }, {
      id: "712518",
      pid: "712500",
      name: "绿岛乡"
    }, {
      id: "712519",
      pid: "712500",
      name: "兰屿乡"
    }, {
      id: "712520",
      pid: "712500",
      name: "延平乡"
    }, {
      id: "712521",
      pid: "712500",
      name: "卑南乡"
    }, {
      id: "712522",
      pid: "712500",
      name: "鹿野乡"
    }, {
      id: "712523",
      pid: "712500",
      name: "关山镇"
    }, {
      id: "712524",
      pid: "712500",
      name: "海端乡"
    }, {
      id: "712525",
      pid: "712500",
      name: "池上乡"
    }, {
      id: "712526",
      pid: "712500",
      name: "东河乡"
    }, {
      id: "712527",
      pid: "712500",
      name: "成功镇"
    }, {
      id: "712528",
      pid: "712500",
      name: "长滨乡"
    }, {
      id: "712529",
      pid: "712500",
      name: "金峰乡"
    }, {
      id: "712530",
      pid: "712500",
      name: "大武乡"
    }, {
      id: "712531",
      pid: "712500",
      name: "达仁乡"
    }, {
      id: "712532",
      pid: "712500",
      name: "太麻里乡"
    }]
  }, {
    id: "712600",
    pid: "710000",
    name: "花莲县",
    children: [{
      id: "712615",
      pid: "712600",
      name: "花莲市"
    }, {
      id: "712616",
      pid: "712600",
      name: "新城乡"
    }, {
      id: "712617",
      pid: "712600",
      name: "太鲁阁"
    }, {
      id: "712618",
      pid: "712600",
      name: "秀林乡"
    }, {
      id: "712619",
      pid: "712600",
      name: "吉安乡"
    }, {
      id: "712620",
      pid: "712600",
      name: "寿丰乡"
    }, {
      id: "712621",
      pid: "712600",
      name: "凤林镇"
    }, {
      id: "712622",
      pid: "712600",
      name: "光复乡"
    }, {
      id: "712623",
      pid: "712600",
      name: "丰滨乡"
    }, {
      id: "712624",
      pid: "712600",
      name: "瑞穗乡"
    }, {
      id: "712625",
      pid: "712600",
      name: "万荣乡"
    }, {
      id: "712626",
      pid: "712600",
      name: "玉里镇"
    }, {
      id: "712627",
      pid: "712600",
      name: "卓溪乡"
    }, {
      id: "712628",
      pid: "712600",
      name: "富里乡"
    }]
  }, {
    id: "712700",
    pid: "710000",
    name: "澎湖县",
    children: [{
      id: "712707",
      pid: "712700",
      name: "马公市"
    }, {
      id: "712708",
      pid: "712700",
      name: "西屿乡"
    }, {
      id: "712709",
      pid: "712700",
      name: "望安乡"
    }, {
      id: "712710",
      pid: "712700",
      name: "七美乡"
    }, {
      id: "712711",
      pid: "712700",
      name: "白沙乡"
    }, {
      id: "712712",
      pid: "712700",
      name: "湖西乡"
    }]
  }, {
    id: "712800",
    pid: "710000",
    name: "连江县",
    children: [{
      id: "712805",
      pid: "712800",
      name: "南竿乡"
    }, {
      id: "712806",
      pid: "712800",
      name: "北竿乡"
    }, {
      id: "712807",
      pid: "712800",
      name: "莒光乡"
    }, {
      id: "712808",
      pid: "712800",
      name: "东引乡"
    }]
  }]
}, {
  id: "810000",
  pid: "810000",
  name: "香港特别行政区",
  children: [{
    id: "810100",
    pid: "810000",
    name: "香港岛",
    children: [{
      id: "810101",
      pid: "810100",
      name: "中西区"
    }, {
      id: "810102",
      pid: "810100",
      name: "湾仔"
    }, {
      id: "810103",
      pid: "810100",
      name: "东区"
    }, {
      id: "810104",
      pid: "810100",
      name: "南区"
    }]
  }, {
    id: "810200",
    pid: "810000",
    name: "九龙",
    children: [{
      id: "810201",
      pid: "810200",
      name: "九龙城区"
    }, {
      id: "810202",
      pid: "810200",
      name: "油尖旺区"
    }, {
      id: "810203",
      pid: "810200",
      name: "深水埗区"
    }, {
      id: "810204",
      pid: "810200",
      name: "黄大仙区"
    }, {
      id: "810205",
      pid: "810200",
      name: "观塘区"
    }]
  }, {
    id: "810300",
    pid: "810000",
    name: "新界",
    children: [{
      id: "810301",
      pid: "810300",
      name: "北区"
    }, {
      id: "810302",
      pid: "810300",
      name: "大埔区"
    }, {
      id: "810303",
      pid: "810300",
      name: "沙田区"
    }, {
      id: "810304",
      pid: "810300",
      name: "西贡区"
    }, {
      id: "810305",
      pid: "810300",
      name: "元朗区"
    }, {
      id: "810306",
      pid: "810300",
      name: "屯门区"
    }, {
      id: "810307",
      pid: "810300",
      name: "荃湾区"
    }, {
      id: "810308",
      pid: "810300",
      name: "葵青区"
    }, {
      id: "810309",
      pid: "810300",
      name: "离岛区"
    }]
  }]
}, {
  id: "820000",
  pid: "820000",
  name: "澳门特别行政区",
  children: [{
    id: "820100",
    pid: "820000",
    name: "澳门半岛"
  }, {
    id: "820200",
    pid: "820000",
    name: "离岛"
  }]
}, {
  id: "990000",
  pid: "990000",
  name: "海外",
  children: [{
    id: "990100",
    pid: "990000",
    name: "海外"
  }]
}];

var dayjs_min = {exports: {}};

(function (module, exports) {
!function(t,e){module.exports=e();}(commonjsGlobal,(function(){var t=1e3,e=6e4,n=36e5,r="millisecond",i="second",s="minute",u="hour",a="day",o="week",f="month",h="quarter",c="year",d="date",$="Invalid Date",l=/^(\d{4})[-/]?(\d{1,2})?[-/]?(\d{0,2})[Tt\s]*(\d{1,2})?:?(\d{1,2})?:?(\d{1,2})?[.:]?(\d+)?$/,y=/\[([^\]]+)]|Y{1,4}|M{1,4}|D{1,2}|d{1,4}|H{1,2}|h{1,2}|a|A|m{1,2}|s{1,2}|Z{1,2}|SSS/g,M={name:"en",weekdays:"Sunday_Monday_Tuesday_Wednesday_Thursday_Friday_Saturday".split("_"),months:"January_February_March_April_May_June_July_August_September_October_November_December".split("_")},m=function(t,e,n){var r=String(t);return !r||r.length>=e?t:""+Array(e+1-r.length).join(n)+t},g={s:m,z:function(t){var e=-t.utcOffset(),n=Math.abs(e),r=Math.floor(n/60),i=n%60;return (e<=0?"+":"-")+m(r,2,"0")+":"+m(i,2,"0")},m:function t(e,n){if(e.date()<n.date())return -t(n,e);var r=12*(n.year()-e.year())+(n.month()-e.month()),i=e.clone().add(r,f),s=n-i<0,u=e.clone().add(r+(s?-1:1),f);return +(-(r+(n-i)/(s?i-u:u-i))||0)},a:function(t){return t<0?Math.ceil(t)||0:Math.floor(t)},p:function(t){return {M:f,y:c,w:o,d:a,D:d,h:u,m:s,s:i,ms:r,Q:h}[t]||String(t||"").toLowerCase().replace(/s$/,"")},u:function(t){return void 0===t}},v="en",D={};D[v]=M;var p=function(t){return t instanceof _},S=function t(e,n,r){var i;if(!e)return v;if("string"==typeof e){var s=e.toLowerCase();D[s]&&(i=s),n&&(D[s]=n,i=s);var u=e.split("-");if(!i&&u.length>1)return t(u[0])}else {var a=e.name;D[a]=e,i=a;}return !r&&i&&(v=i),i||!r&&v},w=function(t,e){if(p(t))return t.clone();var n="object"==typeof e?e:{};return n.date=t,n.args=arguments,new _(n)},O=g;O.l=S,O.i=p,O.w=function(t,e){return w(t,{locale:e.$L,utc:e.$u,x:e.$x,$offset:e.$offset})};var _=function(){function M(t){this.$L=S(t.locale,null,!0),this.parse(t);}var m=M.prototype;return m.parse=function(t){this.$d=function(t){var e=t.date,n=t.utc;if(null===e)return new Date(NaN);if(O.u(e))return new Date;if(e instanceof Date)return new Date(e);if("string"==typeof e&&!/Z$/i.test(e)){var r=e.match(l);if(r){var i=r[2]-1||0,s=(r[7]||"0").substring(0,3);return n?new Date(Date.UTC(r[1],i,r[3]||1,r[4]||0,r[5]||0,r[6]||0,s)):new Date(r[1],i,r[3]||1,r[4]||0,r[5]||0,r[6]||0,s)}}return new Date(e)}(t),this.$x=t.x||{},this.init();},m.init=function(){var t=this.$d;this.$y=t.getFullYear(),this.$M=t.getMonth(),this.$D=t.getDate(),this.$W=t.getDay(),this.$H=t.getHours(),this.$m=t.getMinutes(),this.$s=t.getSeconds(),this.$ms=t.getMilliseconds();},m.$utils=function(){return O},m.isValid=function(){return !(this.$d.toString()===$)},m.isSame=function(t,e){var n=w(t);return this.startOf(e)<=n&&n<=this.endOf(e)},m.isAfter=function(t,e){return w(t)<this.startOf(e)},m.isBefore=function(t,e){return this.endOf(e)<w(t)},m.$g=function(t,e,n){return O.u(t)?this[e]:this.set(n,t)},m.unix=function(){return Math.floor(this.valueOf()/1e3)},m.valueOf=function(){return this.$d.getTime()},m.startOf=function(t,e){var n=this,r=!!O.u(e)||e,h=O.p(t),$=function(t,e){var i=O.w(n.$u?Date.UTC(n.$y,e,t):new Date(n.$y,e,t),n);return r?i:i.endOf(a)},l=function(t,e){return O.w(n.toDate()[t].apply(n.toDate("s"),(r?[0,0,0,0]:[23,59,59,999]).slice(e)),n)},y=this.$W,M=this.$M,m=this.$D,g="set"+(this.$u?"UTC":"");switch(h){case c:return r?$(1,0):$(31,11);case f:return r?$(1,M):$(0,M+1);case o:var v=this.$locale().weekStart||0,D=(y<v?y+7:y)-v;return $(r?m-D:m+(6-D),M);case a:case d:return l(g+"Hours",0);case u:return l(g+"Minutes",1);case s:return l(g+"Seconds",2);case i:return l(g+"Milliseconds",3);default:return this.clone()}},m.endOf=function(t){return this.startOf(t,!1)},m.$set=function(t,e){var n,o=O.p(t),h="set"+(this.$u?"UTC":""),$=(n={},n[a]=h+"Date",n[d]=h+"Date",n[f]=h+"Month",n[c]=h+"FullYear",n[u]=h+"Hours",n[s]=h+"Minutes",n[i]=h+"Seconds",n[r]=h+"Milliseconds",n)[o],l=o===a?this.$D+(e-this.$W):e;if(o===f||o===c){var y=this.clone().set(d,1);y.$d[$](l),y.init(),this.$d=y.set(d,Math.min(this.$D,y.daysInMonth())).$d;}else $&&this.$d[$](l);return this.init(),this},m.set=function(t,e){return this.clone().$set(t,e)},m.get=function(t){return this[O.p(t)]()},m.add=function(r,h){var d,$=this;r=Number(r);var l=O.p(h),y=function(t){var e=w($);return O.w(e.date(e.date()+Math.round(t*r)),$)};if(l===f)return this.set(f,this.$M+r);if(l===c)return this.set(c,this.$y+r);if(l===a)return y(1);if(l===o)return y(7);var M=(d={},d[s]=e,d[u]=n,d[i]=t,d)[l]||1,m=this.$d.getTime()+r*M;return O.w(m,this)},m.subtract=function(t,e){return this.add(-1*t,e)},m.format=function(t){var e=this,n=this.$locale();if(!this.isValid())return n.invalidDate||$;var r=t||"YYYY-MM-DDTHH:mm:ssZ",i=O.z(this),s=this.$H,u=this.$m,a=this.$M,o=n.weekdays,f=n.months,h=function(t,n,i,s){return t&&(t[n]||t(e,r))||i[n].slice(0,s)},c=function(t){return O.s(s%12||12,t,"0")},d=n.meridiem||function(t,e,n){var r=t<12?"AM":"PM";return n?r.toLowerCase():r},l={YY:String(this.$y).slice(-2),YYYY:this.$y,M:a+1,MM:O.s(a+1,2,"0"),MMM:h(n.monthsShort,a,f,3),MMMM:h(f,a),D:this.$D,DD:O.s(this.$D,2,"0"),d:String(this.$W),dd:h(n.weekdaysMin,this.$W,o,2),ddd:h(n.weekdaysShort,this.$W,o,3),dddd:o[this.$W],H:String(s),HH:O.s(s,2,"0"),h:c(1),hh:c(2),a:d(s,u,!0),A:d(s,u,!1),m:String(u),mm:O.s(u,2,"0"),s:String(this.$s),ss:O.s(this.$s,2,"0"),SSS:O.s(this.$ms,3,"0"),Z:i};return r.replace(y,(function(t,e){return e||l[t]||i.replace(":","")}))},m.utcOffset=function(){return 15*-Math.round(this.$d.getTimezoneOffset()/15)},m.diff=function(r,d,$){var l,y=O.p(d),M=w(r),m=(M.utcOffset()-this.utcOffset())*e,g=this-M,v=O.m(this,M);return v=(l={},l[c]=v/12,l[f]=v,l[h]=v/3,l[o]=(g-m)/6048e5,l[a]=(g-m)/864e5,l[u]=g/n,l[s]=g/e,l[i]=g/t,l)[y]||g,$?v:O.a(v)},m.daysInMonth=function(){return this.endOf(f).$D},m.$locale=function(){return D[this.$L]},m.locale=function(t,e){if(!t)return this.$L;var n=this.clone(),r=S(t,e,!0);return r&&(n.$L=r),n},m.clone=function(){return O.w(this.$d,this)},m.toDate=function(){return new Date(this.valueOf())},m.toJSON=function(){return this.isValid()?this.toISOString():null},m.toISOString=function(){return this.$d.toISOString()},m.toString=function(){return this.$d.toUTCString()},M}(),T=_.prototype;return w.prototype=T,[["$ms",r],["$s",i],["$m",s],["$H",u],["$W",a],["$M",f],["$y",c],["$D",d]].forEach((function(t){T[t[1]]=function(e){return this.$g(e,t[0],t[1])};})),w.extend=function(t,e){return t.$i||(t(e,_,w),t.$i=!0),w},w.locale=S,w.isDayjs=p,w.unix=function(t){return w(1e3*t)},w.en=D[v],w.Ls=D,w.p={},w}));
}(dayjs_min));

var dayjs = dayjs_min.exports;

var customParseFormat$1 = {exports: {}};

(function (module, exports) {
!function(e,t){module.exports=t();}(commonjsGlobal,(function(){var e={LTS:"h:mm:ss A",LT:"h:mm A",L:"MM/DD/YYYY",LL:"MMMM D, YYYY",LLL:"MMMM D, YYYY h:mm A",LLLL:"dddd, MMMM D, YYYY h:mm A"},t=/(\[[^[]*\])|([-:/.()\s]+)|(A|a|YYYY|YY?|MM?M?M?|Do|DD?|hh?|HH?|mm?|ss?|S{1,3}|z|ZZ?)/g,n=/\d\d/,r=/\d\d?/,i=/\d*[^\s\d-_:/()]+/,o={},s=function(e){return (e=+e)+(e>68?1900:2e3)};var a=function(e){return function(t){this[e]=+t;}},f=[/[+-]\d\d:?(\d\d)?|Z/,function(e){(this.zone||(this.zone={})).offset=function(e){if(!e)return 0;if("Z"===e)return 0;var t=e.match(/([+-]|\d\d)/g),n=60*t[1]+(+t[2]||0);return 0===n?0:"+"===t[0]?-n:n}(e);}],h=function(e){var t=o[e];return t&&(t.indexOf?t:t.s.concat(t.f))},u=function(e,t){var n,r=o.meridiem;if(r){for(var i=1;i<=24;i+=1)if(e.indexOf(r(i,0,t))>-1){n=i>12;break}}else n=e===(t?"pm":"PM");return n},d={A:[i,function(e){this.afternoon=u(e,!1);}],a:[i,function(e){this.afternoon=u(e,!0);}],S:[/\d/,function(e){this.milliseconds=100*+e;}],SS:[n,function(e){this.milliseconds=10*+e;}],SSS:[/\d{3}/,function(e){this.milliseconds=+e;}],s:[r,a("seconds")],ss:[r,a("seconds")],m:[r,a("minutes")],mm:[r,a("minutes")],H:[r,a("hours")],h:[r,a("hours")],HH:[r,a("hours")],hh:[r,a("hours")],D:[r,a("day")],DD:[n,a("day")],Do:[i,function(e){var t=o.ordinal,n=e.match(/\d+/);if(this.day=n[0],t)for(var r=1;r<=31;r+=1)t(r).replace(/\[|\]/g,"")===e&&(this.day=r);}],M:[r,a("month")],MM:[n,a("month")],MMM:[i,function(e){var t=h("months"),n=(h("monthsShort")||t.map((function(e){return e.slice(0,3)}))).indexOf(e)+1;if(n<1)throw new Error;this.month=n%12||n;}],MMMM:[i,function(e){var t=h("months").indexOf(e)+1;if(t<1)throw new Error;this.month=t%12||t;}],Y:[/[+-]?\d+/,a("year")],YY:[n,function(e){this.year=s(e);}],YYYY:[/\d{4}/,a("year")],Z:f,ZZ:f};function c(n){var r,i;r=n,i=o&&o.formats;for(var s=(n=r.replace(/(\[[^\]]+])|(LTS?|l{1,4}|L{1,4})/g,(function(t,n,r){var o=r&&r.toUpperCase();return n||i[r]||e[r]||i[o].replace(/(\[[^\]]+])|(MMMM|MM|DD|dddd)/g,(function(e,t,n){return t||n.slice(1)}))}))).match(t),a=s.length,f=0;f<a;f+=1){var h=s[f],u=d[h],c=u&&u[0],l=u&&u[1];s[f]=l?{regex:c,parser:l}:h.replace(/^\[|\]$/g,"");}return function(e){for(var t={},n=0,r=0;n<a;n+=1){var i=s[n];if("string"==typeof i)r+=i.length;else {var o=i.regex,f=i.parser,h=e.slice(r),u=o.exec(h)[0];f.call(t,u),e=e.replace(u,"");}}return function(e){var t=e.afternoon;if(void 0!==t){var n=e.hours;t?n<12&&(e.hours+=12):12===n&&(e.hours=0),delete e.afternoon;}}(t),t}}return function(e,t,n){n.p.customParseFormat=!0,e&&e.parseTwoDigitYear&&(s=e.parseTwoDigitYear);var r=t.prototype,i=r.parse;r.parse=function(e){var t=e.date,r=e.utc,s=e.args;this.$u=r;var a=s[1];if("string"==typeof a){var f=!0===s[2],h=!0===s[3],u=f||h,d=s[2];h&&(d=s[2]),o=this.$locale(),!f&&d&&(o=n.Ls[d]),this.$d=function(e,t,n){try{if(["x","X"].indexOf(t)>-1)return new Date(("X"===t?1e3:1)*e);var r=c(t)(e),i=r.year,o=r.month,s=r.day,a=r.hours,f=r.minutes,h=r.seconds,u=r.milliseconds,d=r.zone,l=new Date,m=s||(i||o?1:l.getDate()),M=i||l.getFullYear(),Y=0;i&&!o||(Y=o>0?o-1:l.getMonth());var p=a||0,v=f||0,D=h||0,g=u||0;return d?new Date(Date.UTC(M,Y,m,p,v,D,g+60*d.offset*1e3)):n?new Date(Date.UTC(M,Y,m,p,v,D,g)):new Date(M,Y,m,p,v,D,g)}catch(e){return new Date("")}}(t,a,r),this.init(),d&&!0!==d&&(this.$L=this.locale(d).$L),u&&t!=this.format(a)&&(this.$d=new Date("")),o={};}else if(a instanceof Array)for(var l=a.length,m=1;m<=l;m+=1){s[1]=a[m-1];var M=n.apply(this,s);if(M.isValid()){this.$d=M.$d,this.$L=M.$L,this.init();break}m===l&&(this.$d=new Date(""));}else i.call(this,e);};}}));
}(customParseFormat$1));

var customParseFormat = customParseFormat$1.exports;

dayjs.extend(customParseFormat);

function date() {
  var format = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : 'YYYY-MM-DD';
  return dayjs(faker.date.past()).format(format);
} // 返回一个随机的日期字符串。


function datetime() {
  var format = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : 'YYYY-MM-DD HH:mm:ss';
  return date(format);
}

function time() {
  var format = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : 'HH:mm:ss';
  return date(format);
}

function now(unit, format) {
  // now(unit) now(format)
  if (arguments.length === 1) {
    // now(format)
    if (!/year|month|day|hour|minute|second|week/.test(unit)) {
      format = unit;
      unit = '';
    }
  }

  unit = (unit || '').toLowerCase();
  format = format || 'yyyy-MM-dd HH:mm:ss';
  var date = new Date();
  /* jshint -W086 */
  // 参考自 http://momentjs.cn/docs/#/manipulating/start-of/

  switch (unit) {
    case 'year':
      date.setMonth(0);

    case 'month':
      date.setDate(1);

    case 'week':
    case 'day':
      date.setHours(0);

    case 'hour':
      date.setMinutes(0);

    case 'minute':
      date.setSeconds(0);

    case 'second':
      date.setMilliseconds(0);
  }

  switch (unit) {
    case 'week':
      date.setDate(date.getDate() - date.getDay());
  }

  return dayjs(date).format(format);
}

function _randomDate() {
  var min = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : new Date(0);
  var max = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : new Date();
  return new Date(integer(min.getTime(), max.getTime()));
}

function timestamp(min, max) {
  if (min instanceof Date && max instanceof Date) return _randomDate(min, max).getTime();
  return _randomDate().getTime();
}

/*
    随机生成一个 18 位身份证。

    [身份证](http://baike.baidu.com/view/1697.htm#4)
        地址码 6 + 出生日期码 8 + 顺序码 3 + 校验码 1
    [《中华人民共和国行政区划代码》国家标准(GB/T2260)](http://zhidao.baidu.com/question/1954561.html)
*/

function id() {
  var rank = ['7', '9', '10', '5', '8', '4', '2', '1', '6', '3', '7', '9', '10', '5', '8', '4', '2'];
  var last = ['1', '0', 'X', '9', '8', '7', '6', '5', '4', '3', '2'];
  var id = sample(DICT).id + date('YYYYMMDD') + string$1('number', 3);
  var sum = id.split('').reduce(function (a, b, i) {
    return a + parseInt(b, 10) * parseInt(rank[i], 10);
  }, 0);
  id += last[sum % 11];
  return id;
}

/*
        随机生成一个 GUID。

        http://www.broofa.com/2008/09/javascript-uuid-function/
        [UUID 规范](http://www.ietf.org/rfc/rfc4122.txt)
            UUIDs (Universally Unique IDentifier)
            GUIDs (Globally Unique IDentifier)
            The formal definition of the UUID string representation is provided by the following ABNF [7]:
                UUID                   = time-low "-" time-mid "-"
                                       time-high-and-version "-"
                                       clock-seq-and-reserved
                                       clock-seq-low "-" node
                time-low               = 4hexOctet
                time-mid               = 2hexOctet
                time-high-and-version  = 2hexOctet
                clock-seq-and-reserved = hexOctet
                clock-seq-low          = hexOctet
                node                   = 6hexOctet
                hexOctet               = hexDigit hexDigit
                hexDigit =
                    "0" / "1" / "2" / "3" / "4" / "5" / "6" / "7" / "8" / "9" /
                    "a" / "b" / "c" / "d" / "e" / "f" /
                    "A" / "B" / "C" / "D" / "E" / "F"

        https://github.com/victorquinn/chancejs/blob/develop/chance.js#L1349

        ! KonghaYao : GUID 为微软对 GUID 的一种实现，在此实现的 guid 是对 uuid 的字符串转换

        UUID格式：xxxxxxxx-xxxx-xxxx-xxxxxxxxxxxxxxxx(8-4-4-16)
        GUID格式：xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx (8-4-4-4-12)
    */
function guid() {
  return v4().replace(/(?<=[^-]{4}-[^-]{4}-[^-]{4}-[^-]{4})/, '-');
}

/*
    ## Miscellaneous
*/
/*
    生成一个全局的自增整数。
    类似自增主键（auto increment primary key）。
*/

var key = 0;
var increment = decode(function (step) {
  return key += +step || 1;
}, ['number']);
var inc = increment; // Dice

function d4() {
  return natural(1, 4);
}

function d6() {
  return natural(1, 6);
}

function d8() {
  return natural(1, 8);
}

function d12() {
  return natural(1, 12);
}

function d20() {
  return natural(1, 20);
}

function d100() {
  return natural(1, 100);
}

var sentence = decode(function () {
  var min = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : 12;
  var max = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 18;
  var len = random$1(min, max);
  return faker.lorem.sentence(len);
}, ['number', 'number']); // 随机生成一个单词。

var word = decode(function () {
  var min = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : 3;
  var max = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 10;
  var len = random$1(min, max);
  return faker.lorem.word(len);
}, ['number', 'number']); // 随机生成一段文本。

var paragraph = decode(function () {
  var min = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : 3;
  var max = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 7;
  var len = random$1(min, max);
  return faker.lorem.paragraph(len);
}, ['number', 'number']); // 随机生成一句标题，其中每个单词的首字母大写。

var title = decode(function () {
  var min = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : 3;
  var max = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 7;
  var len = random$1(min, max);
  return times(len, function () {
    return capitalize(word());
  }).join('');
}, ['number', 'number']);

var cparagraph = decode(function () {
  var min = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : 3;
  var max = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 7;
  var len = random$1(min, max);
  return times(len, function () {
    return csentence();
  }).join('\n');
}, ['number', 'number']); // 随机生成一个中文句子。

var csentence = decode(function () {
  var min = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : 12;
  var max = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 18;
  var len = random$1(min, max);
  return times(len, function () {
    return cword();
  }).join('') + '。';
}, ['number', 'number']); // 随机生成一句中文标题。

var ctitle = decode(function () {
  var min = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : 3;
  var max = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 7;
  var len = random$1(min, max);
  return times(len, function () {
    return cword();
  }).join('');
}, ['number', 'number']); // 最常用的 500 个汉字 http://baike.baidu.com/view/568436.htm

var DICT_HANZI = '的一是在不了有和人这中大为上个国我以要他时来用们生到作地于出就分对成会可主发年动同工也能下过子说产种面而方后多定行学法所民得经十三之进着等部度家电力里如水化高自二理起小物现实加量都两体制机当使点从业本去把性好应开它合还因由其些然前外天政四日那社义事平形相全表间样与关各重新线内数正心反你明看原又么利比或但质气第向道命此变条只没结解问意建月公无系军很情者最立代想已通并提直题党程展五果料象员革位入常文总次品式活设及管特件长求老头基资边流路级少图山统接知较将组见计别她手角期根论运农指几九区强放决西被干做必战先回则任取据处队南给色光门即保治北造百规热领七海口东导器压志世金增争济阶油思术极交受联什认六共权收证改清己美再采转更单风切打白教速花带安场身车例真务具万每目至达走积示议声报斗完类八离华名确才科张信马节话米整空元况今集温传土许步群广石记需段研界拉林律叫且究观越织装影算低持音众书布复容儿须际商非验连断深难近矿千周委素技备半办青省列习响约支般史感劳便团往酸历市克何除消构府称太准精值号率族维划选标写存候毛亲快效斯院查江型眼王按格养易置派层片始却专状育厂京识适属圆包火住调满县局照参红细引听该铁价严龙飞'; // 随机生成一个或多个汉字。

function _cword() {
  var len = 1;
  var pool = DICT_HANZI;
  var min;
  var max;

  for (var _len = arguments.length, args = new Array(_len), _key = 0; _key < _len; _key++) {
    args[_key] = arguments[_key];
  }

  switch (args.length) {
    case 0:
      // ()
      break;

    case 1:
      // ( pool )
      if (typeof args[0] === 'string') {
        pool = args[0];
      } else {
        // ( length )
        len = args[0];
      }

      break;

    case 2:
      // ( pool, length )
      if (typeof args[0] === 'string') {
        pool = args[0];
        len = args[1];
      } else {
        // ( min, max )
        min = args[0];
        max = args[1];
        len = natural(min, max);
      }

      break;

    case 3:
      // (pool,min,max)
      pool = args[0];
      min = args[1];
      max = args[2];
      len = natural(min, max);
      break;
  }

  return sampleSize$1(pool, len).join('');
}

var cword = decode(_cword, ['number', 'number', 'number']);

var protocols = ["http", "ftp", "gopher", "mailto", "mid", "cid", "news", "nntp", "prospero", "telnet", "rlogin", "tn3270", "wais"];
var tlds = ["com", "net", "org", "edu", "gov", "int", "mil", "cn", "com.cn", "net.cn", "gov.cn", "org.cn", "中国", "中国互联.公司", "中国互联.网络", "tel", "biz", "cc", "tv", "info", "name", "hk", "mobi", "asia", "cd", "travel", "pro", "museum", "coop", "aero", "ad", "ae", "af", "ag", "ai", "al", "am", "an", "ao", "aq", "ar", "as", "at", "au", "aw", "az", "ba", "bb", "bd", "be", "bf", "bg", "bh", "bi", "bj", "bm", "bn", "bo", "br", "bs", "bt", "bv", "bw", "by", "bz", "ca", "cc", "cf", "cg", "ch", "ci", "ck", "cl", "cm", "cn", "co", "cq", "cr", "cu", "cv", "cx", "cy", "cz", "de", "dj", "dk", "dm", "do", "dz", "ec", "ee", "eg", "eh", "es", "et", "ev", "fi", "fj", "fk", "fm", "fo", "fr", "ga", "gb", "gd", "ge", "gf", "gh", "gi", "gl", "gm", "gn", "gp", "gr", "gt", "gu", "gw", "gy", "hk", "hm", "hn", "hr", "ht", "hu", "id", "ie", "il", "in", "io", "iq", "ir", "is", "it", "jm", "jo", "jp", "ke", "kg", "kh", "ki", "km", "kn", "kp", "kr", "kw", "ky", "kz", "la", "lb", "lc", "li", "lk", "lr", "ls", "lt", "lu", "lv", "ly", "ma", "mc", "md", "mg", "mh", "ml", "mm", "mn", "mo", "mp", "mq", "mr", "ms", "mt", "mv", "mw", "mx", "my", "mz", "na", "nc", "ne", "nf", "ng", "ni", "nl", "no", "np", "nr", "nt", "nu", "nz", "om", "qa", "pa", "pe", "pf", "pg", "ph", "pk", "pl", "pm", "pn", "pr", "pt", "pw", "py", "re", "ro", "ru", "rw", "sa", "sb", "sc", "sd", "se", "sg", "sh", "si", "sj", "sk", "sl", "sm", "sn", "so", "sr", "st", "su", "sy", "sz", "tc", "td", "tf", "tg", "th", "tj", "tk", "tm", "tn", "to", "tp", "tr", "tt", "tv", "tw", "tz", "ua", "ug", "uk", "us", "uy", "va", "vc", "ve", "vg", "vn", "vu", "wf", "ws", "ye", "yu", "za", "zm", "zr", "zw"];

/*
    随机生成一个 URL。

    [URL 规范](http://www.w3.org/Addressing/URL/url-spec.txt)
        http                    Hypertext Transfer Protocol
        ftp                     File Transfer protocol
        gopher                  The Gopher protocol
        mailto                  Electronic mail address
        mid                     Message identifiers for electronic mail
        cid                     Content identifiers for MIME body part
        news                    Usenet news
        nntp                    Usenet news for local NNTP access only
        prospero                Access using the prospero protocols
        telnet rlogin tn3270    Reference to interactive sessions
        wais                    Wide Area Information Servers
*/

function url(Protocol, host) {
  return "".concat(Protocol || protocol(), "://").concat(host || domain(), "/").concat(word());
} // 随机生成一个 URL 协议。

function protocol() {
  return sample(protocols);
} // 随机生成一个域名。

function domain(Tld) {
  return "".concat(word(), ".").concat(Tld || tld());
}
/*
    随机生成一个顶级域名。
    国际顶级域名 international top-level domain-names, iTLDs
    国家顶级域名 national top-level domainnames, nTLDs
    [域名后缀大全](http://www.163ns.com/zixun/post/4417.html)
*/

function tld() {
  // Top Level Domain
  return sample(tlds);
} // 随机生成一个邮件地址。

function email(domain) {
  return "".concat(character('lower'), ".").concat(word(), "@").concat(domain || word() + '.' + tld());
} // 随机生成一个 IP 地址。

function ip() {
  // 这里不能用 Array(4).fill fill 是向数组注入同一个值
  return _toConsumableArray(Array(4)).map(function () {
    return natural(0, 255);
  }).join('.');
}

var _adSize$1 = ["300x250", "250x250", "240x400", "336x280", "180x150", "720x300", "468x60", "234x60", "88x31", "120x90", "120x60", "120x240", "125x125", "728x90", "160x600", "120x600", "300x600"];
var _screenSize$1 = ["320x200", "320x240", "640x480", "800x480", "800x480", "1024x600", "1024x768", "1280x800", "1440x900", "1920x1200", "2560x1600"];
var _videoSize$1 = ["720x480", "768x576", "1280x720", "1920x1080"];
var size = {
  _adSize: _adSize$1,
  _screenSize: _screenSize$1,
  _videoSize: _videoSize$1
};

var Addvocate = "#ff6138";
var Adobe = "#ff0000";
var Aim = "#fcd20b";
var Amazon = "#e47911";
var Android = "#a4c639";
var AOL = "#0060a3";
var Atlassian = "#003366";
var Behance = "#053eff";
var bitly = "#ee6123";
var Blogger = "#fc4f08";
var Boeing = "#0039a6";
var Carbonmade = "#613854";
var Cheddar = "#ff7243";
var Delicious = "#205cc0";
var Dell = "#3287c1";
var Designmoo = "#e54a4f";
var Deviantart = "#4e6252";
var Devour = "#fd0001";
var DEWALT = "#febd17";
var Dribbble = "#ea4c89";
var Dropbox = "#3d9ae8";
var Drupal = "#0c76ab";
var Dunked = "#2a323a";
var eBay = "#89c507";
var Ember = "#f05e1b";
var Engadget = "#00bdf6";
var Envato = "#528036";
var Etsy = "#eb6d20";
var Evernote = "#5ba525";
var Facebook = "#3b5998";
var Firefox = "#e66000";
var Forrst = "#5b9a68";
var Foursquare = "#25a0ca";
var Garmin = "#007cc3";
var GetGlue = "#2d75a2";
var Gimmebar = "#f70078";
var GitHub = "#171515";
var Grooveshark = "#f77f00";
var Groupon = "#82b548";
var HelloWallet = "#0085ca";
var HootSuite = "#003366";
var Houzz = "#73ba37";
var HTML5 = "#ec6231";
var IKEA = "#ffcc33";
var IMDb = "#f3ce13";
var Instagram = "#3f729b";
var Intel = "#0071c5";
var Intuit = "#365ebf";
var Kickstarter = "#76cc1e";
var kippt = "#e03500";
var Kodery = "#00af81";
var LastFM = "#c3000d";
var LinkedIn = "#0e76a8";
var Livestream = "#cf0005";
var Lumo = "#576396";
var Mixpanel = "#a086d3";
var Meetup = "#e51937";
var Nokia = "#183693";
var NVIDIA = "#76b900";
var Opera = "#cc0f16";
var Path = "#e41f11";
var Pinboard = "#0000e6";
var Pinterest = "#c8232c";
var PlayStation = "#665cbe";
var Pocket = "#ee4056";
var Prezi = "#318bff";
var Pusha = "#0f71b4";
var Quora = "#a82400";
var Rdio = "#008fd5";
var Readability = "#9c0000";
var Resource = "#7eb400";
var Rockpack = "#0ba6ab";
var Roon = "#62b0d9";
var RSS = "#ee802f";
var Salesforce = "#1798c1";
var Samsung = "#0c4da2";
var Shopify = "#96bf48";
var Skype = "#00aff0";
var Snagajob = "#f47a20";
var Softonic = "#008ace";
var SoundCloud = "#ff7700";
var Spotify = "#81b71a";
var Sprint = "#fee100";
var Squarespace = "#121212";
var StackOverflow = "#ef8236";
var Staples = "#cc0000";
var Stripe = "#008cdd";
var StudyBlue = "#00afe1";
var StumbleUpon = "#f74425";
var Technorati = "#40a800";
var Treehouse = "#5cb868";
var Trulia = "#5eab1f";
var Tumblr = "#34526f";
var Twitter = "#00acee";
var TYPO3 = "#ff8700";
var Ubuntu = "#dd4814";
var Ustream = "#3388ff";
var Verizon = "#ef1d1d";
var Vimeo = "#86c9ef";
var Vine = "#00a478";
var Virb = "#06afd8";
var Wooga = "#5b009c";
var Wunderlist = "#2b88d9";
var XBOX = "#9bc848";
var XING = "#126567";
var Yandex = "#ffcc00";
var Yelp = "#c41200";
var YouTube = "#c4302b";
var Zalongo = "#5498dc";
var Zendesk = "#78a300";
var Zerply = "#9dcc7a";
var Zootool = "#5e8b1d";
var _brandColors = {
  "4ormat": "#fb0a2a",
  "500px": "#02adea",
  "About.me (blue)": "#00405d",
  "About.me (yellow)": "#ffcc33",
  Addvocate: Addvocate,
  Adobe: Adobe,
  Aim: Aim,
  Amazon: Amazon,
  Android: Android,
  "Angie's List": "#7fbb00",
  AOL: AOL,
  Atlassian: Atlassian,
  Behance: Behance,
  "Big Cartel": "#97b538",
  bitly: bitly,
  Blogger: Blogger,
  Boeing: Boeing,
  "Booking.com": "#003580",
  Carbonmade: Carbonmade,
  Cheddar: Cheddar,
  "Code School": "#3d4944",
  Delicious: Delicious,
  Dell: Dell,
  Designmoo: Designmoo,
  Deviantart: Deviantart,
  "Designer News": "#2d72da",
  Devour: Devour,
  DEWALT: DEWALT,
  "Disqus (blue)": "#59a3fc",
  "Disqus (orange)": "#db7132",
  Dribbble: Dribbble,
  Dropbox: Dropbox,
  Drupal: Drupal,
  Dunked: Dunked,
  eBay: eBay,
  Ember: Ember,
  Engadget: Engadget,
  Envato: Envato,
  Etsy: Etsy,
  Evernote: Evernote,
  "Fab.com": "#dd0017",
  Facebook: Facebook,
  Firefox: Firefox,
  "Flickr (blue)": "#0063dc",
  "Flickr (pink)": "#ff0084",
  Forrst: Forrst,
  Foursquare: Foursquare,
  Garmin: Garmin,
  GetGlue: GetGlue,
  Gimmebar: Gimmebar,
  GitHub: GitHub,
  "Google Blue": "#0140ca",
  "Google Green": "#16a61e",
  "Google Red": "#dd1812",
  "Google Yellow": "#fcca03",
  "Google+": "#dd4b39",
  Grooveshark: Grooveshark,
  Groupon: Groupon,
  "Hacker News": "#ff6600",
  HelloWallet: HelloWallet,
  "Heroku (light)": "#c7c5e6",
  "Heroku (dark)": "#6567a5",
  HootSuite: HootSuite,
  Houzz: Houzz,
  HTML5: HTML5,
  IKEA: IKEA,
  IMDb: IMDb,
  Instagram: Instagram,
  Intel: Intel,
  Intuit: Intuit,
  Kickstarter: Kickstarter,
  kippt: kippt,
  Kodery: Kodery,
  LastFM: LastFM,
  LinkedIn: LinkedIn,
  Livestream: Livestream,
  Lumo: Lumo,
  Mixpanel: Mixpanel,
  Meetup: Meetup,
  Nokia: Nokia,
  NVIDIA: NVIDIA,
  Opera: Opera,
  Path: Path,
  "PayPal (dark)": "#1e477a",
  "PayPal (light)": "#3b7bbf",
  Pinboard: Pinboard,
  Pinterest: Pinterest,
  PlayStation: PlayStation,
  Pocket: Pocket,
  Prezi: Prezi,
  Pusha: Pusha,
  Quora: Quora,
  "QUOTE.fm": "#66ceff",
  Rdio: Rdio,
  Readability: Readability,
  "Red Hat": "#cc0000",
  Resource: Resource,
  Rockpack: Rockpack,
  Roon: Roon,
  RSS: RSS,
  Salesforce: Salesforce,
  Samsung: Samsung,
  Shopify: Shopify,
  Skype: Skype,
  Snagajob: Snagajob,
  Softonic: Softonic,
  SoundCloud: SoundCloud,
  "Space Box": "#f86960",
  Spotify: Spotify,
  Sprint: Sprint,
  Squarespace: Squarespace,
  StackOverflow: StackOverflow,
  Staples: Staples,
  "Status Chart": "#d7584f",
  Stripe: Stripe,
  StudyBlue: StudyBlue,
  StumbleUpon: StumbleUpon,
  "T-Mobile": "#ea0a8e",
  Technorati: Technorati,
  "The Next Web": "#ef4423",
  Treehouse: Treehouse,
  Trulia: Trulia,
  Tumblr: Tumblr,
  "Twitch.tv": "#6441a5",
  Twitter: Twitter,
  TYPO3: TYPO3,
  Ubuntu: Ubuntu,
  Ustream: Ustream,
  Verizon: Verizon,
  Vimeo: Vimeo,
  Vine: Vine,
  Virb: Virb,
  "Virgin Media": "#cc0000",
  Wooga: Wooga,
  "WordPress (blue)": "#21759b",
  "WordPress (orange)": "#d54e21",
  "WordPress (grey)": "#464646",
  Wunderlist: Wunderlist,
  XBOX: XBOX,
  XING: XING,
  "Yahoo!": "#720e9e",
  Yandex: Yandex,
  Yelp: Yelp,
  YouTube: YouTube,
  Zalongo: Zalongo,
  Zendesk: Zendesk,
  Zerply: Zerply,
  Zootool: Zootool
};

var _adSize = size._adSize,
    _screenSize = size._screenSize,
    _videoSize = size._videoSize;

var _brandNames = Object.keys(_brandColors);
/*
    生成一个随机的图片地址。

    使用 api 替代图片源
        http://fpoimg.com/
    参考自
        http://rensanning.iteye.com/blog/1933310
        http://code.tutsplus.com/articles/the-top-8-placeholders-for-web-designers--net-19485
*/


function image() {
  for (var _len = arguments.length, args = new Array(_len), _key = 0; _key < _len; _key++) {
    args[_key] = arguments[_key];
  }

  var size;
  var background;
  var foreground;
  var format;
  var text;

  switch (arguments.length) {
    case 3:
      size = args[0];
      background = args[1];
      text = args[2];
      break;

    case 4:
      size = args[0];
      background = args[1];
      foreground = args[2];
      text = args[3];
      break;

    default:
      size = args[0];
      background = args[1];
      foreground = args[2];
      format = args[3];
      text = args[4];
  } // Random.image()


  if (!size) size = sample(_adSize); // 去除前面的 # 号

  foreground = (foreground || hex()).replace(/^#/, '');
  background = (background || hex()).replace(/^#/, ''); // http://dummyimage.com/600x400/cc00cc/470047.png&text=hello

  return "http://dummyimage.com/".concat(size).concat(background ? '/' + background : '').concat(foreground ? '/' + foreground : '').concat(format ? '.' + format : '').concat(text ? '&text=' + text : '');
}
/*
    生成一段随机的 Base64 图片编码。

    https://github.com/imsky/holder
    Holder renders image placeholders entirely on the client side.

    dataImageHolder: function(size) {
        return 'holder.js/' + size
    },
*/


function dataImage(size, text) {
  var canvas;

  if (typeof document !== 'undefined') {
    canvas = document.createElement('canvas');
  } else {
    /*
        https://github.com/Automattic/node-canvas
            npm install canvas --save
        安装问题：
        * http://stackoverflow.com/questions/22953206/gulp-issues-with-cario-install-command-not-found-when-trying-to-installing-canva
        * https://github.com/Automattic/node-canvas/issues/415
        * https://github.com/Automattic/node-canvas/wiki/_pages
         PS：node-canvas 的安装过程实在是太繁琐了，所以不放入 package.json 的 dependencies。
     */
    var Canvas = module.require('canvas');

    canvas = new Canvas();
  }

  var ctx = canvas && canvas.getContext && canvas.getContext('2d');
  if (!canvas || !ctx) return '';
  if (!size) size = sample(_adSize);
  text = text !== undefined ? text : size;

  var _size$split = size.split('x'),
      _size$split2 = _slicedToArray(_size$split, 2),
      w = _size$split2[0],
      h = _size$split2[1];

  var width = parseInt(w, 10);
  var height = parseInt(h, 10);
  var background = chroma.random().hex();
  var foreground = '#FFF';
  var textHeight = 14;
  var font = 'sans-serif';
  canvas.width = width;
  canvas.height = height;
  ctx.textAlign = 'center';
  ctx.textBaseline = 'middle';
  ctx.fillStyle = background;
  ctx.fillRect(0, 0, width, height);
  ctx.fillStyle = foreground;
  ctx.font = 'bold ' + textHeight + 'px ' + font;
  ctx.fillText(text, width / 2, height / 2, width);
  return canvas.toDataURL('image/png');
}

/*
    ## Address
*/
var REGION = ['东北', '华北', '华东', '华中', '华南', '西南', '西北'];

function region() {
  return sample(REGION);
} // 随机生成一个（中国）省（或直辖市、自治区、特别行政区）。


function province() {
  return sample(DICT).name;
} // 随机生成一个（中国）市。


var city = decode(function (prefix) {
  var province = sample(DICT);
  var city = sample(province.children);
  return prefix ? [province.name, city.name].join(' ') : city.name;
}, ['boolean']); // 随机生成一个（中国）县。

var county = decode(function (prefix) {
  var province = sample(DICT);
  var city = sample(province.children);
  var county = city.children && city.children.length ? sample(city.children) : {
    name: '-'
  };
  return prefix ? [province.name, city.name, county.name].join(' ') : county.name;
}, ['boolean']); // 随机生成一个邮政编码（六位数字）。

var zip = decode(function (len) {
  var zip = '';

  for (var i = 0; i < (len || 6); i++) {
    zip += natural(0, 9);
  }

  return zip;
}, ['number']); //  function address() {}

// 用于生成 Blob 和 File 型数据的工具
function genFile(anyData) {
  return new File([anyData !== null && anyData !== void 0 ? anyData : paragraph()], word());
}
function genBlob(anyData) {
  return new Blob([anyData !== null && anyData !== void 0 ? anyData : paragraph()]);
}

/*
    ## Mock.Random

    工具类，用于生成各种随机数据。
*/
var pick = sample;
var shuffle = shuffle$1;

var Random = /*#__PURE__*/Object.freeze({
  __proto__: null,
  pick: pick,
  shuffle: shuffle,
  integer: integer,
  isNumber: isNumber,
  random: random$1,
  natural: natural,
  int: integer,
  float: _float,
  boolean: _boolean$1,
  bool: _boolean$1,
  character: character,
  char: character,
  capitalize: capitalize,
  upper: upper,
  lower: lower,
  string: string$1,
  str: string$1,
  range: range,
  _goldenRatioColor: _goldenRatioColor,
  color: color,
  hex: hex,
  rgb: rgb,
  rgba: rgba,
  hsl: hsl,
  first: first,
  last: last,
  name: name,
  cfirst: cfirst,
  clast: clast,
  cname: cname,
  d4: d4,
  d6: d6,
  d8: d8,
  d12: d12,
  d20: d20,
  d100: d100,
  guid: guid,
  uuid: v4,
  id: id,
  increment: increment,
  inc: inc,
  paragraph: paragraph,
  word: word,
  sentence: sentence,
  title: title,
  ctitle: ctitle,
  csentence: csentence,
  cparagraph: cparagraph,
  cword: cword,
  url: url,
  protocol: protocol,
  domain: domain,
  tld: tld,
  email: email,
  ip: ip,
  time: time,
  datetime: datetime,
  date: date,
  now: now,
  timestamp: timestamp,
  _adSize: _adSize,
  _screenSize: _screenSize,
  _videoSize: _videoSize,
  image: image,
  img: image,
  _brandColors: _brandColors,
  _brandNames: _brandNames,
  dataImage: dataImage,
  region: region,
  province: province,
  city: city,
  county: county,
  zip: zip,
  genFile: genFile,
  genBlob: genBlob
});

// 占位符即是字符串函数的意思
function placeholder(resultString, context) {
  var delayTrigger = false;
  var result = resultString.replace(Constant.RE_PLACEHOLDER, function (all, _, key, params) {
    // 路径触发函数
    if (key.includes('/')) {
      if (delayTrigger === false) {
        // 延迟到最后加载
        delayTrigger = true;
      }

      return all;
    } // 解析占位符的参数


    if (key in Random) {
      var args = params ? params.split(/\s*,\s*/).map(function (param) {
        // 递归解析参数中的占位符
        if (Constant.RE_PLACEHOLDER.test(param)) {
          return placeholder(param, context);
        }

        return param;
      }) : []; //! 这里传入字符串给所有的 Random 函数

      return Random[key].apply(context.currentContext, args);
    }

    return all;
  });

  if (delayTrigger) {
    console.log('使用虚拟');
    var vir = new VirtualValue(result, context);
    context.delay.path.push(vir);
    return vir;
  } else {
    return result;
  }
}

var string = function string(options) {
  // options 需要传递，所以不能直接解构
  var rule = options.rule,
      context = options.context,
      template = options.template;
  var range = rule.range,
      count = rule.count;
  var result = template;

  if (template.length) {
    //  'foo': '★', count 为 undefined 时
    // 'star|1-5': '★',
    result = count === undefined ? template : times(count, function () {
      return template;
    }).join(''); // PlaceHolder 指的是在值的位置上又出现了 @语法兼容的方法
    // 'email|1-10': '@EMAIL, ',

    result = placeholder(result, context);
  } else {
    // 'ASCII|1-10': '',
    // 'ASCII': '',
    result = range.length ? string$1(count) : template;
  }

  return result;
};

var Handle = /*#__PURE__*/Object.freeze({
  __proto__: null,
  array: array,
  boolean: _boolean,
  number: number$1,
  object: object,
  regexp: regexp,
  string: string
});

var pathHandler = function pathHandler(result, delay) {
  delay.forEach(function (vir) {
    vir.value = vir.value.replace(Constant.RE_PLACEHOLDER, function (all, _, key) {
      if (key.includes('/')) {
        var path = key.split('/');

        if (path[0] === '') {
          // 绝对定位
          return get(result, path.slice(1));
        } else {
          // 相对定位
          var cur = _toConsumableArray(vir.context.path); // ! 矫正定位问题


          cur.pop();
          var findParent = true;
          path.forEach(function (i) {
            if (findParent && i === '..') {
              cur.pop();
            } else {
              findParent = false;
              cur.push(i);
            }
          });
          return get(result, cur);
        }
      }

      return all;
    });
    vir.replaceToReal();
  });
};
var functionHandler = function functionHandler(result, func) {
  func.forEach(function (vir) {
    var path = vir.context.path;
    var thisArg = path.length ? get(result, path) : result;
    vir.value = vir.value.call(thisArg, result, vir.context);
    vir.replaceToReal();
  });
};

var createContext = function createContext(obj) {
  return _objectSpread2({
    path: [],
    templatePath: [],
    currentContext: {},
    delay: {
      "function": [],
      path: []
    },
    templateCurrentContext: {},
    templateRoot: ''
  }, obj);
};
/** 模板对象解析器，所有的函数的解析入口 */


function gen(template) {
  var key = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : '';
  var context = arguments.length > 2 ? arguments[2] : undefined;
  var type = getType(template);
  var func = Handle[type];

  if (func) {
    if (context === undefined) {
      context = createContext({
        templateRoot: template
      });
    }

    var data = func({
      type: type,
      template: template,
      name: key,
      parsedName: key ? ('' + key).replace(Constant.RE_KEY, '$1') : key,
      rule: parser(key),
      context: context
    });
    return data;
  } else {
    return template;
  }
}
function genInput(template) {
  var context = createContext({
    templateRoot: template
  });
  var result = gen(template, '', context);
  pathHandler(result, context.delay.path);
  functionHandler(result, context.delay["function"]);
  return result;
}

var defaultExport = _objectSpread2({
  gen: gen,
  placeholder: placeholder
}, Handle);

function mock() {
  var template;

  for (var _len = arguments.length, args = new Array(_len), _key = 0; _key < _len; _key++) {
    args[_key] = arguments[_key];
  }

  switch (args.length) {
    case 1:
      // Mock.mock(template)
      template = args[0];
      var data = genInput(template); //debugger 专用

      console.log(template, data);
      return data;
    // 2 和 3 switch 穿透

    case 2:
      template = args[1];
      break;

    case 3:
      template = args[2];
      break;
  }

  return null;
}

// global require, module, window
/* !
    Mock - 模拟请求 & 模拟数据
    https://github.com/nuysoft/Mock
    墨智 mozhi.gyy@taobao.com nuysoft@gmail.com

    此版本为 esm 版本
    KonghaYao 修改于 2021/6/11
    https://github.com/KonghaYao/
*/

var Mock = {
  Handler: defaultExport,
  Random: Random,
  Util: Util,
  heredoc: heredoc,
  mock: mock
}; // //ifdef browser

export { Mock as default };
