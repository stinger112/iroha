/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

const Module = require('./lib/irohalib')
const { Transaction } = require('./pb/transaction_pb')

/**
 * Get message of std::exception from C++ raw pointer.
 * Link: https://groups.google.com/d/msg/emscripten-discuss/ZRhTbpeF6SI/5bPSlpQyAwAJ
 * @param {number} ptr
 * @return {string} Exception's message
 */
function whatEmscriptenException (ptr) {
  // 8 should be the vtable offset for the what function
  const fPtr = Module.HEAPU32[(Module.HEAPU32[(ptr) >> 2] + 8) >> 2]
  return Module.AsciiToString(Module.dynCall_ii(fPtr, ptr))
}

/**
 * Wrap class object into Proxy to catch errors from bulders' methods.
 * WARNING! We assume that wrapped object has just methods and constructors.
 * @param {*} obj Any non-primitive object
 * @returns {Proxy|*}
 */
function wrap (obj) {
  if (obj === Object(obj)) {
    return new Proxy(obj, {
      // Wrap constructors to enable "get" traps on new objects
      construct (target, args) {
        return wrap(new target(...args))
      },

      // Wrap every object's method
      get (target, propKey, receiver) {
        const calledProperty = Reflect.get(...arguments)

        if (typeof propKey !== 'symbol' && // Don't wrap built-in symbols!
          typeof calledProperty === 'function') {
          // Returned function is an Emscripten exceptions catcher
          return (...args) => {
            try {
              return wrap(calledProperty.apply(target, args))
            } catch (e) {
              throw typeof e === 'number'
                ? new Error(whatEmscriptenException(e)) : e
            }
          }
        } else {
          return calledProperty
        }
      }
    })
  } else {
    return obj
  }
}

// Export proxyfied embinded classes
module.exports = Object.entries(Module)
  .filter(([propKey, prop]) => prop && prop.hasOwnProperty('argCount'))
  .map(([propKey, prop]) => [propKey, wrap(prop)])
  .reduce((obj, [k, v]) => Object.assign(obj, { [k]: v }), {})

// Export embinded enums
module.exports.Role = Module.Role
module.exports.Grantable = Module.Grantable

/**
 * Export compiled C++ module itself for direct access.
 */
module.exports.__Module = Module

/**
 * Conversion between native and custom types.
 * @param {Blob|ByteVector} blob object from bindings
 * @return {Uint8Array}
 */
function blob2array (blob) {
  let bytes = new Uint8Array(blob.size())
  for (let i = 0; i < blob.size(); ++i) {
    bytes[i] = blob.get(i)
  }
  return bytes
}

/**
 * Get transaction and serialize it to ByteVector value.
 * @param {Transaction} tx
 * @return {ByteVector}
 */
function serialize (tx) {
  return tx.serializeBinary().reduce((bv, currentByte) => {
    bv.push_back(currentByte)
    return bv
  }, new Module.ByteVector())
}

/**
 * Get ByteVector and parse it to Transaction object.
 * @param {ByteVector} vec
 * @return {Transaction}
 */
function deserialize (vec) {
  return Transaction.deserializeBinary(blob2array(vec))
}

/**
 * Get hash of transaction's proto object.
 * @param {Transaction} tx
 * @return {Uint8Array}
 */
module.exports.hashTransaction = function (tx) {
  try {
    return blob2array(Module.hashTransaction(
      serialize(tx)
    ))
  } catch (e) {
    throw typeof e === 'number'
      ? new Error(whatEmscriptenException(e)) : e
  }
}

/**
 * Add signature to a transaction's signature list.
 * @param {Transaction} tx
 * @param keys
 * @return {Transaction}
 */
module.exports.signTransaction = function (tx, keys) {
  try {
    return deserialize(
      Module.signTransaction(
        serialize(tx), keys
      )
    )
  } catch (e) {
    throw typeof e === 'number'
      ? new Error(whatEmscriptenException(e)) : e
  }
}

/**
 * Check correctness of transaction's proto object.
 * If validation is unsuccessful exception returns.
 * @param {Transaction} tx
 * @return {undefined}
 */
module.exports.validateTransaction = function (tx) {
  try {
    return Module.validateTransaction(serialize(tx))
  } catch (e) {
    throw typeof e === 'number'
      ? new Error(whatEmscriptenException(e)) : e
  }
}
