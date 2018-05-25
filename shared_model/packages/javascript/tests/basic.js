const test = require('tape')
const iroha = { 
  ModelCrypto, 
  ModelQueryBuilder, 
  Hash, 
  HashVector 
} = require('../index.js')

const publicKey = '407e57f50ca48969b08ba948171bb2435e035d82cec417e18e4a38f5fb113f83'
const privateKey = '1d7e0a32ee0affeb4d22acd73c2c6fb6bd58e266c8c2ce4fa0ffe3dd6a253ffb'
const randomKey = 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
const incorrectInputLength = 'aaaaaaaaaaaaaaaa'

const crypto = new ModelCrypto()
const queryBuilder = new ModelQueryBuilder()

const accountId = 'admin@test'
const assetId = 'coin#test'

// Test createdTime
const time = Date.now()
const counter = 1
// TODO: Need tests to get hash of built object
// TODO: Need tests to unsigned long overflowing
// var correctQuery = queryBuilder.creatorAccountId(accountId).createdTime(time).queryCounter(counter)
// console.log(correctQuery.getAccount(accountId).build().hash().toString())

// test('Integration tests', function (t) {
//   t.plan(8)

//   let keypair = crypto.convertFromExisting(publicKey, privateKey)
//   let queryPrototype = queryBuilder
//                       .creatorAccountId(accountId)
//                       .createdTime(time)
//                       .queryCounter(counter)
//   let unsignedTx = queryPrototype.getAccount(accountId).build()

  
//   t.comment('UnsignedWrapperQuery methods availability tests')
//   t.ok(unsignedTx.hash().toString().match(/^Hash: \[\w{64}\s\]$/), 'Should be in format "Hash: []"')
//   t.ok(unsignedTx.signAndAddSignature(keypair) instanceof iroha.Query, 'Should have type Query')

//   let signedTx = unsignedTx.signAndAddSignature(keypair)
  
//   t.comment('Basic Query tests')
//   t.equal(signedTx.creatorAccountId(), accountId, 'Should be the same as accountId')
//   t.equal(parseInt(signedTx.queryCounter()), counter, 'Should be the same as queryCounter')
//   t.equal(parseInt(signedTx.createdTime()), time, 'Should be the same as createdTime')
//   t.ok(signedTx.blob() instanceof iroha.Blob, 'Should have type Blob')
//   t.ok(signedTx.payload() instanceof iroha.Blob, 'Should have type Blob')
//   // t.equal(signedTx.signatures(), null, 'Should be the same as signatures')
//   // t.equal(signedTx.addSignature(), true, 'Should be the same as addSignature')
//   t.equal(signedTx.clearSignatures(), true, 'Should be the same as clearSignatures')
// })

const adminAccountId = 'admin@test'
// const assetId = 'coin#test'
const testAccountId = 'test@test'

// test('ModelTransactionBuilder tests', function (t) {
//   t.plan(8)
  
//   let keypair = crypto.convertFromExisting(publicKey, privateKey)

//   const txBuilder = new iroha.ModelTransactionBuilder()
//   const futureTime = 2400000000000
//   const address = '0.0.0.0:50051'

//   // Transaction with valid creatorAccountId and createdTime
//   let txPrototype = txBuilder
//                     .creatorAccountId(adminAccountId)
//                     .createdTime(time)

//   let unsignedTx = txPrototype.addAssetQuantity(adminAccountId, assetId, '1000').build()
  
//   t.comment('UnsignedWrapperTransaction methods availability tests')
//   t.ok(unsignedTx.hash().toString().match(/^Hash: \[\w{64}\s\]$/), 'Should be in format "Hash: []"')
//   t.ok(unsignedTx.signAndAddSignature(keypair) instanceof iroha.Transaction, 'Should have type Query')

//   let signedTx = unsignedTx.signAndAddSignature(keypair)
  
//   t.comment('Basic Transaction tests')
//   t.equal(signedTx.creatorAccountId(), accountId, 'Should be the same as accountId')
//   t.equal(parseInt(signedTx.createdTime()), time, 'Should be the same as createdTime')
//   // t.equal(signedTx.commands(), [], 'Should be the same as createdTime')
//   t.ok(signedTx.blob() instanceof iroha.Blob, 'Should have type Blob')
//   t.ok(signedTx.payload() instanceof iroha.Blob, 'Should have type Blob')
//   // t.equal(signedTx.signatures(), null, 'Should be the same as signatures')
//   t.equal(signedTx.addSignature(new iroha.Signed('sometesttesttest'), new iroha.PublicKey(publicKey)), true, 'Should be the same as addSignature')
//   t.equal(signedTx.clearSignatures(), true, 'Should be the same as clearSignatures')

//   // let sv = new iroha.StringVector()
//   // sv.push_back('can_add_peer')
//   // sv.push_back('can_read_assets')
//   // txPrototype.createRole('new_user_role', sv).build()
// })

test('Basic unit tests', function (t) {
  t.plan(8)
  
  t.comment('Hash tests')
  t.throws(() => new Hash(), /Tried to invoke ctor of Hash with invalid number of parameters/, 'Should throw "Tried to invoke ctor of Hash with invalid number of parameters..."')
  t.ok(new Hash('') instanceof Hash, 'Should not throw any exceptions')

  const hash = new Hash('11111111111111111111111111111111')
  t.equals(hash.hex(), '3131313131313131313131313131313131313131313131313131313131313131', 'Should not throw any exceptions')
  // t.equals(hash.blob(), null, 'Should not throw any exceptions')
  t.equals(hash.toString(), 'Hash: [3131313131313131313131313131313131313131313131313131313131313131 ]', 'Should not throw any exceptions')
  // t.equals(hash.fromHexString(), null, 'Should not throw any exceptions')

  t.comment('HashVector tests')
  t.ok(new HashVector() instanceof HashVector, 'Should not throw any exceptions')

  let hv = new HashVector()
  t.doesNotThrow(() => hv.push_back(hash), null, 'Should not throw any exceptions')
  t.equals(hv.get(0).hex(), hash.hex(), 'Should be equal')
  t.equals(hv.size(), 1, 'Should be equal')

  t.end()
})


// TODO: Additional test
// t.comment('Correct query')
// var correctQuery2 = queryBuilder.creatorAccountId(accountId).createdTime((new Date()).getTime() + 10000).queryCounter(1)
// t.equal(correctQuery2.getAccount(accountId).build().hash().toString(), "aaaaaaaaaaa", 'Should be the same as private key was passed to convertFromExisting')
