#include <emscripten/bind.h>

// #include "cryptography/hash.hpp"
#include "model_crypto.hpp"

using namespace emscripten;
using namespace shared_model::bindings;
using namespace shared_model::crypto;

EMSCRIPTEN_BINDINGS(model_crypto)
{
  // TODO: Can we create wrapper for std::exception?
  // class_<std::exception>("IrohaException")
  // .constructor<std::exception>();
  // .function("what", &std::invalid_argument::what);

  // class_<std::invalid_argument>("InvalidArgumentException")
  // .constructor<std::string>()
  // .function("what", &std::invalid_argument::what, allow_raw_pointers());

  class_<Blob>("Blob")
  .constructor<std::string>()
  .function("fromHexString", &Blob::fromHexString)
  .function("blob", &Blob::blob)
  .function("hex", &Blob::hex)
  .function("toString", &Blob::toString);

  class_<Signed, base<Blob>>("Signed")
  .constructor<const std::string&>()
  .function("toString", &Signed::toString);

  class_<PublicKey, base<Blob>>("PublicKey")
  .constructor<const std::string&>()
  .function("toString", &PublicKey::toString);

  class_<PrivateKey, base<Blob>>("PrivateKey")
  .constructor<const std::string&>()
  .function("toString", &PrivateKey::toString);

  class_<Keypair>("Keypair")
  .constructor<PublicKey, PrivateKey>()
  .function("publicKey", &Keypair::publicKey)
  .function("privateKey", &Keypair::privateKey);

  /**
   * Top level ModelCrypto class
   **/
  class_<ModelCrypto>("ModelCrypto")
  .constructor<>()
  .function("generateKeypair", &ModelCrypto::generateKeypair)
  .function("fromPrivateKey", &ModelCrypto::fromPrivateKey)
  .function("convertFromExisting", &ModelCrypto::convertFromExisting);
}
