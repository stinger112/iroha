/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

const test = require('tape')
const {
  ModelCrypto,
  hashTransaction,
  signTransaction,
  validateTransaction
} = require('../index')
const cmd = require('../pb/commands_pb')
const {
  Transaction
} = require('../pb/transaction_pb')

function validAddPeer () {
  let command = new cmd.Command()
  command.setAddPeer(
    // Create addPeer
    (() => {
      let addPeer = new cmd.AddPeer()
      addPeer.setPeer(
        // Create Peer
        (() => {
          let peer = new cmd.Peer()
          peer.setAddress('127.0.0.1:50500')
          peer.setPeerKey('vd1YQE0TFeDrJ5AsXXyOsGAsFiOPAFdz30BrwZEwiSk=')
          return peer
        })()
      )
      return addPeer
    })()
  )
  return command
}

function createUnsignedTx (commands) {
  let tx = new Transaction()
  tx.setPayload(
    // Create Payload
    (() => {
      let payload = new Transaction.Payload()
      payload.setReducedPayload(
        // Create ReducedPayload
        (() => {
          let reducedPayload = new Transaction.Payload.ReducedPayload()
          reducedPayload.setCreatorAccountId('admin@test')
          reducedPayload.setCreatedTime(Date.now())
          reducedPayload.setQuorum(1)
          reducedPayload.addCommands(commands)
          return reducedPayload
        })()
      )
      return payload
    })()
  )

  return tx
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
