#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "client_api.hpp"

using namespace emscripten;
using namespace shared_model::bindings;

EMSCRIPTEN_BINDINGS(client_api) {
  function("validateTransaction", &validateTransaction);
  function("validateQuery", &validateQuery);

  function("signTransaction", &signTransaction);
  function("signQuery", &signQuery);
  
  function("hashTransaction", &hashTransaction);
  function("hashQuery", &hashQuery);
}
