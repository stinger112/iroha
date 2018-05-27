const test = require('tape')
const { ERROR_MESSAGES } = require('./helpers')
const iroha = { 
  Blob, 
  Hash, 
  HashVector
} = require('../index.js')

test('Common unit tests', function (t) {
  t.plan(12)
  
  const hexString = '11111111111111111111111111111111'

  t.comment('Blob tests')
  t.throws(() => new Blob(), ERROR_MESSAGES.INVALID_CONSTRUCTOR_PARAMETERS, 'Should throw illegal number of arguments')
  t.ok(new Blob('') instanceof Blob, 'Should have the same type as Blob')
  
  const blob = new Blob(hexString)
  t.equals(blob.hex(), '3131313131313131313131313131313131313131313131313131313131313131', 'Should be the same as hex representation')
  t.equals(blob.size(), hexString.length, 'Should have the same as length of hexString')
  // t.equals(blob.toString(), 'Hash: [3131313131313131313131313131313131313131313131313131313131313131 ]', 'Should be in the format of log')
  // t.equals(Blob.fromHexString(hexString).hex(), hexString, 'Should be the same as hexString')

  t.comment('Hash tests')
  t.throws(() => new Hash(), ERROR_MESSAGES.INVALID_CONSTRUCTOR_PARAMETERS, 'Should throw illegal number of arguments')
  t.ok(new Hash('') instanceof Hash, 'Should have the same type as Hash')

  const hash = new Hash(hexString)
  t.equals(hash.hex(), '3131313131313131313131313131313131313131313131313131313131313131', 'Should be the same as hex representation')
  t.equals(hash.toString(), 'Hash: [3131313131313131313131313131313131313131313131313131313131313131 ]', 'Should be in the format of log')
  // t.equals(hash.fromHexString(), null, 'Should not throw any exceptions')

  t.comment('HashVector tests')
  t.ok(new HashVector() instanceof HashVector, 'Should have the same type as HashVector')

  let hv = new HashVector()
  t.doesNotThrow(() => hv.push_back(hash), null, 'Should add new member to the vector')
  t.equals(hv.get(0).hex(), hash.hex(), 'Should return the first member of the vector')
  t.equals(hv.size(), 1, 'Should have size = 1')

  t.end()
})
