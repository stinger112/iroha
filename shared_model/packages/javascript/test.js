const test = require('tape')
const iroha = require('../../build/bindings/irohalib.js')

const publicKey = '407e57f50ca48969b08ba948171bb2435e035d82cec417e18e4a38f5fb113f83'
const privateKey = '1d7e0a32ee0affeb4d22acd73c2c6fb6bd58e266c8c2ce4fa0ffe3dd6a253ffb'
const randomKey = 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
const incorrectInputLength = 'aaaaaaaaaaaaaaaa'


const crypto = new iroha.ModelCrypto()
const queryBuilder = new iroha.ModelQueryBuilder()

const accountId = 'admin@test'
const assetId = 'coin#test'

// Test createdTime
const time = (new Date()).getTime()
const counter = 1
// Нужны тесты с получением точного конечного hash от собранного объекта!!
// Нужны тесты на переполнение unsigned long
// var correctQuery = queryBuilder.creatorAccountId(accountId).createdTime(time).queryCounter(counter)
// console.log(correctQuery.getAccount(accountId).build().hash().toString())


test('Integration tests', function (t) {
  t.plan(10)

  let keypair = crypto.convertFromExisting(publicKey, privateKey)
  let queryPrototype = queryBuilder
                      .creatorAccountId(accountId)
                      .createdTime(time)
                      .queryCounter(counter)
  let unsignedQuery = queryPrototype.getAccount(accountId).build()

  
  t.comment('UnsignedWrapperQuery methods availability tests')
  t.ok(unsignedQuery.hash().toString().match(/^Hash: \[\w{64}\s\]$/), 'Should be in format "Hash: []"')
  t.ok(unsignedQuery.signAndAddSignature(keypair) instanceof iroha.Query, 'Should have type Query')

  let signedQuery = unsignedQuery.signAndAddSignature(keypair)
  
  t.comment('Basic Query tests')
  t.equal(signedQuery.creatorAccountId(), accountId, 'Should be the same as accountId')
  t.equal(parseInt(signedQuery.queryCounter()), counter, 'Should be the same as queryCounter')
  t.equal(parseInt(signedQuery.createdTime()), time, 'Should be the same as createdTime')
  t.ok(signedQuery.blob() instanceof iroha.Blob, 'Should have type Blob')
  t.ok(signedQuery.payload() instanceof iroha.Blob, 'Should have type Blob')
  t.equal(signedQuery.signatures(), null, 'Should be the same as signatures')
  t.equal(signedQuery.addSignature(), true, 'Should be the same as addSignature')
  t.equal(signedQuery.clearSignatures(), true, 'Should be the same as clearSignatures')
})
