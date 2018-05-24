/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

const Module = require('../../build/bindings/irohalib.js')

/**
 * Get message of std::exception by Emscripten raw ptr.
 * Link: https://groups.google.com/d/msg/emscripten-discuss/ZRhTbpeF6SI/5bPSlpQyAwAJ
 * @param {number} ptr
 */
function whatEmscriptenException(ptr) {
  // 8 should be the vtable offset for the what function
  const fPtr = Module.HEAPU32[(Module.HEAPU32[(ptr) >> 2] + 8) >> 2]
  const message = Module.AsciiToString(Module.dynCall_ii(fPtr, ptr))

  return message
}

/**
 * Wrap class object into Proxy to catch errors from bulders' methods.
 * WARNING! We assume that wrapped object has just methods and constructors.
 * @param {*} obj Any non-primitive object
 * @returns {Proxy|*} 
 */
function wrap(obj) {
  if (obj === Object(obj)) {
    return new Proxy(obj, {
      // Control invocation all properties (which are methods)
      get(target, propKey, receiver) {
        const calledProperty = Reflect.get(...arguments)

        if (typeof propKey !== 'symbol' // Don't wrap built-in symbols!
              && typeof calledProperty === 'function') {
          // console.log(
          //   `[DEBUG] Reading method`, propKey,`of target`, target.constructor.name
          // )

          // Returned function is an emscripten exceptions catcher
          return (...args) => {
            try {
              // console.log('[DEBUG] Called callback with args: ', args)

              return wrap(calledProperty.apply(target, args))
            } catch(e) {
              throw typeof e === 'number' ?
                      new Error(whatEmscriptenException(e)) : e
            }
          }
        } else {
          // console.log(`[DEBUG] Target property`, propKey, `not a function!`)

          return calledProperty
        }
      },

      // Control builders constructors to enable "get" traps
      construct(target, args) {
          // console.log(`Construct new "${target}"`)
          return wrap(new target(...args))
      }
    })
  } else {
    return obj
  }
}

module.exports = Object.entries(Module)
.filter(([propKey, prop]) => prop && prop.hasOwnProperty('argCount'))
.map(([propKey, prop]) => [propKey, wrap(prop)])
.reduce((obj, [k, v]) => Object.assign(obj, { [k]: v }), {})
