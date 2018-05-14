const test = require('tape')
const iroha = require('../../bindings/irohalib.js')

const publicKey = '407e57f50ca48969b08ba948171bb2435e035d82cec417e18e4a38f5fb113f83'
const privateKey = '1d7e0a32ee0affeb4d22acd73c2c6fb6bd58e266c8c2ce4fa0ffe3dd6a253ffb'
const randomKey = 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
const incorrectInputLength = 'aaaaaaaaaaaaaaaa'

test('ModelCrypto tests', function (t) {
  t.plan(8)

  let crypto = new iroha.ModelCrypto()

  t.throws(() => crypto.convertFromExisting(randomKey, randomKey), /Provided keypair is not correct/, 'Should throw "Provided keypair is not correct"')
  t.throws(() => crypto.convertFromExisting(incorrectInputLength, incorrectInputLength), /input string has incorrect length/, 'Should throw "input string has incorrect length"')

  let keypair = crypto.convertFromExisting(publicKey, privateKey)
  t.equal(keypair.publicKey().hex(), publicKey, 'Should be the same as public key was passed to convertFromExisting')
  t.equal(keypair.privateKey().hex(), privateKey, 'Should be the same as private key was passed to convertFromExisting')

  let newKeypair = crypto.generateKeypair()
  t.equal(newKeypair.publicKey().hex().length, publicKey.length, 'Size of generated public key should be the same as size of predefined public key')
  t.equal(newKeypair.privateKey().hex().length, privateKey.length, 'Size of generated private key should be the same as size of predefined private key')

  t.throws(() => crypto.fromPrivateKey(incorrectInputLength), /input string has incorrect length/, 'Should throw "input string has incorrect length"')
  t.equal(crypto.fromPrivateKey(privateKey).publicKey().hex(), publicKey)

  t.end()
})

const accountId = 'admin@test'
const assetId = 'coin#test'

test('ModelQueryBuilder tests', function (t) {
  t.plan(50)

  let queryBuilder = new iroha.ModelQueryBuilder()
  const time = (new Date()).getTime()

  // Tests for concrete query
  t.comment('Basic QueryBuilder tests')
  t.throws(() => queryBuilder.build(), /Missing concrete query/, 'Should throw Missing concrete query')
  t.throws(() => queryBuilder.creatorAccountId(accountId).build(), /Missing concrete query/, 'Should throw Missing concrete query')
  t.throws(() => queryBuilder.creatorAccountId(accountId).createdTime(time).build(), /Missing concrete query/, 'Should throw Missing concrete query')
  t.throws(() => queryBuilder.creatorAccountId(accountId).createdTime(time).queryCounter(1).build(), /Missing concrete query/, 'Should throw Missing concrete query')
  t.throws(() => queryBuilder.creatorAccountId('').createdTime(time).queryCounter(1).getAccount(accountId).build(), /Wrongly formed creator_account_id, passed value: ''/, 'Should throw Wrongly formed creator_account_id')
  t.throws(() => queryBuilder.creatorAccountId(accountId).createdTime(0).queryCounter(1).getAccount(accountId).build(), /bad timestamp: too old, timestamp: 0, now:/, 'Should throw bad timestamp: too old')
  t.throws(() => queryBuilder.creatorAccountId(accountId).createdTime(time).queryCounter(0).getAccount(accountId).build(), /Counter should be > 0, passed value: 0/, 'Should throw Counter should be > 0')

  // Query with valid queryCounter, creatorAccountId and createdTime
  let correctQuery = queryBuilder.creatorAccountId(accountId).createdTime(time).queryCounter(1)

  // getAccount() tests
  t.comment('Testing getAccount()')
  t.throws(() => correctQuery.getAccount(), /Error: Illegal number of arguments/, 'Should throw Illegal number of arguments')
  t.throws(() => correctQuery.getAccount('').build(), /Wrongly formed account_id, passed value: ''/, 'Should throw Wrongly formed account_id,')
  t.throws(() => correctQuery.getAccount('@@@').build(), /Wrongly formed account_id, passed value: '@@@'/, 'Should throw Wrongly formed account_id,')
  t.doesNotThrow(() => correctQuery.getAccount(accountId).build(), null, 'Should not throw any exceptions')

  // getSignatories() tests
  t.comment('Testing getSignatories()')
  t.throws(() => correctQuery.getSignatories(), /Error: Illegal number of arguments/, 'Should throw Illegal number of arguments')
  t.throws(() => correctQuery.getSignatories('').build(), /Wrongly formed account_id, passed value: ''/, 'Should throw Wrongly formed account_id,')
  t.throws(() => correctQuery.getSignatories('@@@').build(), /Wrongly formed account_id, passed value: '@@@'/, 'Should throw Wrongly formed account_id,')
  t.doesNotThrow(() => correctQuery.getSignatories(accountId).build(), null, 'Should not throw any exceptions')

  // getAccountTransactions() tests
  t.comment('Testing getAccountTransactions()')
  t.throws(() => correctQuery.getAccountTransactions(), /Error: Illegal number of arguments/, 'Should throw Illegal number of arguments')
  t.throws(() => correctQuery.getAccountTransactions('').build(), /Wrongly formed account_id, passed value: ''/, 'Should throw Wrongly formed account_id,')
  t.throws(() => correctQuery.getAccountTransactions('@@@').build(), /Wrongly formed account_id, passed value: '@@@'/, 'Should throw Wrongly formed account_id,')
  t.doesNotThrow(() => correctQuery.getAccountTransactions(accountId).build(), null, 'Should not throw any exceptions')

  // getAccountAssetTransactions() tests
  t.comment('Testing getAccountAssetTransactions()')
  t.throws(() => correctQuery.getAccountAssetTransactions(), /Error: Illegal number of arguments/, 'Should throw Illegal number of arguments')
  t.throws(() => correctQuery.getAccountAssetTransactions(''), /Error: Illegal number of arguments/, 'Should throw Illegal number of arguments')
  t.throws(() => correctQuery.getAccountAssetTransactions('', assetId).build(), /Wrongly formed account_id, passed value: ''/, 'Should throw Wrongly formed account_id,')
  t.throws(() => correctQuery.getAccountAssetTransactions('@@@', assetId).build(), /Wrongly formed account_id, passed value: '@@@'/, 'Should throw Wrongly formed account_id,')
  t.throws(() => correctQuery.getAccountAssetTransactions(accountId, '').build(), /Wrongly formed asset_id, passed value: ''/, 'Should throw Wrongly formed asset_id,')
  t.throws(() => correctQuery.getAccountAssetTransactions(accountId, '@@@').build(), /Wrongly formed asset_id, passed value: '@@@'/, 'Should throw Wrongly formed asset_id,')
  t.doesNotThrow(() => correctQuery.getAccountAssetTransactions(accountId, assetId).build(), null, 'Should not throw any exceptions')

  // getAccountAssets() tests
  t.comment('Testing getAccountAssets()')
  t.throws(() => correctQuery.getAccountAssets(), /Error: Illegal number of arguments/, 'Should throw Illegal number of arguments')
  t.throws(() => correctQuery.getAccountAssets(''), /Error: Illegal number of arguments/, 'Should throw Illegal number of arguments')
  t.throws(() => correctQuery.getAccountAssets('', assetId).build(), /Wrongly formed account_id, passed value: ''/, 'Should throw Wrongly formed account_id,')
  t.throws(() => correctQuery.getAccountAssets('@@@', assetId).build(), /Wrongly formed account_id, passed value: '@@@'/, 'Should throw Wrongly formed account_id,')
  t.throws(() => correctQuery.getAccountAssets(accountId, '').build(), /Wrongly formed asset_id, passed value: ''/, 'Should throw Wrongly formed asset_id,')
  t.throws(() => correctQuery.getAccountAssets(accountId, '@@@').build(), /Wrongly formed asset_id, passed value: '@@@'/, 'Should throw Wrongly formed asset_id,')
  t.doesNotThrow(() => correctQuery.getAccountAssets(accountId, assetId).build(), null, 'Should not throw any exceptions')

  // getRoles() tests
  t.comment('Testing getRoles()')
  t.doesNotThrow(() => correctQuery.getRoles().build(), null, 'Should not throw any exceptions')

  // getAssetInfo() tests
  t.comment('Testing getAssetInfo()')
  t.throws(() => correctQuery.getAssetInfo(), /Error: Illegal number of arguments/, 'Should throw Illegal number of arguments')
  t.throws(() => correctQuery.getAssetInfo('').build(), /Wrongly formed asset_id, passed value: ''/, 'Should throw Wrongly formed asset_id,')
  t.throws(() => correctQuery.getAssetInfo('@@@').build(), /Wrongly formed asset_id, passed value: '@@@'/, 'Should throw Wrongly formed asset_id,')
  t.doesNotThrow(() => correctQuery.getAssetInfo(assetId).build(), null, 'Should not throw any exceptions')

  // getRolePermissions() tests
  t.comment('Testing getRolePermissions()')
  t.throws(() => correctQuery.getRolePermissions(), /Error: Illegal number of arguments/, 'Should throw Illegal number of arguments')
  t.throws(() => correctQuery.getRolePermissions('').build(), /Wrongly formed role_id, passed value: ''/, 'Should throw Wrongly formed role_id,')
  t.throws(() => correctQuery.getRolePermissions('@@@').build(), /Wrongly formed role_id, passed value: '@@@'/, 'Should throw Wrongly formed role_id,')
  t.doesNotThrow(() => correctQuery.getRolePermissions('role').build(), null, 'Should not throw any exceptions')

  // getTransactions() tests
  t.comment('Testing getTransactions()')
  t.throws(() => correctQuery.getTransactions(), /Error: Illegal number of arguments/, 'Should throw Illegal number of arguments')
  t.throws(() => correctQuery.getTransactions(''), /argument 2 of type 'std::vector< shared_model::crypto::Hash >/, 'Should throw ...argument 2 of type...')

  let hv = new iroha.HashVector()
  hv.add(new iroha.Hash('11111111111111111111111111111111'))
  hv.add(new iroha.Hash('22222222222222222222222222222222'))
  let emptyHv = new iroha.HashVector()

  t.throws(() => correctQuery.getTransactions(emptyHv), /Hash set should contain at least one hash/, 'Should throw Hash set should contain at least one hash')
  t.doesNotThrow(() => correctQuery.getTransactions(hv), null, 'Should not throw any exceptions')

  // getAccountDetail() tests
  t.comment('Testing getAccountDetail()')
  t.throws(() => correctQuery.getAccountDetail(), /Error: Illegal number of arguments/, 'Should throw Illegal number of arguments')
  t.throws(() => correctQuery.getAccountDetail('').build(), /Wrongly formed account_id, passed value: ''/, 'Should throw Wrongly formed account_id,')
  t.throws(() => correctQuery.getAccountDetail('@@@').build(), /Wrongly formed account_id, passed value: '@@@'/, 'Should throw Wrongly formed account_id,')
  t.doesNotThrow(() => correctQuery.getAccountDetail(accountId).build(), null, 'Should not throw any exceptions')

  t.end()
})

console.log(iroha)
