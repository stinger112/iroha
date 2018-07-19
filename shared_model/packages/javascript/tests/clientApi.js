/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

const test = require('tape')
const iroha = require('../index.js')
const {
  ByteVector,
  ModelCrypto
} = require('../index.js')
const cmd = require('../pb/commands_pb')
const {
  Transaction
} = require('../pb/block_pb')

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
  }, new ByteVector())
}

/**
 * Get ByteVector and parse it to Transaction object.
 * @param {ByteVector} vec
 * @return {Transaction}
 */
function deserialize (vec) {
  return Transaction.deserializeBinary(blob2array(vec))
}

function validAddPeer () {
  let peer = new cmd.Peer()
  peer.setAddress('127.0.0.1:50500')
  peer.setPeerKey('vd1YQE0TFeDrJ5AsXXyOsGAsFiOPAFdz30BrwZEwiSk=')

  let addPeer = new cmd.AddPeer()
  addPeer.setPeer(peer)

  let command = new cmd.Command()
  command.setAddPeer(addPeer)

  return command
}

function defaultPayload (commands) {
  let reducedPayload = new Transaction.Payload.ReducedPayload()

  reducedPayload.setCreatorAccountId('admin@test')
  reducedPayload.setCreatedTime(Date.now())
  reducedPayload.setQuorum(1)

  reducedPayload.addCommands(commands)

  return reducedPayload
}

function createUnsignedTx (commands) {
  let tx = new Transaction()
  let payload = new Transaction.Payload()
  payload.setReducedPayload(
    defaultPayload(commands)
  )
  tx.setPayload(payload)

  return tx
}

function hashTransaction (tx) {
  return blob2array(iroha.hashTransaction(
    serialize(tx)
  ))
}

function signTransaction (tx, keys) {
  return deserialize(
    iroha.signTransaction(
      serialize(tx), keys
    )
  )
}

function validateTransaction (tx) {
  let error = iroha.validateTransaction(serialize(tx))

  if (error === '') {
    return true
  } else {
    throw new Error(error)
  }
}

test('Client API tests', function (t) {
  t.plan(6)

  const keys = (new ModelCrypto()).generateKeypair()
  const correctTx = createUnsignedTx(validAddPeer())

  t.comment('Test hashTransaction()')
  t.equals(hashTransaction(correctTx).length, 32, 'Should have a length equal to 32')

  t.comment('Test signTransaction()')
  t.equals(correctTx.getSignaturesList().length, 0, 'Should be an empty list')
  t.equals(signTransaction(correctTx, keys).getSignaturesList().length, 1, 'Should appears one signature in the list')

  t.comment('Test validateTransaction()')
  t.throws(() => validateTransaction(
    signTransaction(createUnsignedTx(), keys) // Create and sign invalid transaction (command missing)
  ), /Undefined: \[\[command is undefined ]]/, 'Transaction should contain at least one command')
  t.throws(() => validateTransaction(correctTx), /Signature: \[\[Signatures cannot be empty ]]/, 'Transaction must be signed')
  t.doesNotThrow(() => validateTransaction(signTransaction(correctTx, keys)), null, 'Validator should return no errors')

  t.end()
})
