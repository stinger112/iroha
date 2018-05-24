// Error messages for SWIG-based Iroha Library
// You can export if you use SWIG
const ERROR_MESSAGES_SWIG = {
  ILLEGAL_NUMBER_OF_ARGUMENTS: /Error: Illegal number of arguments/,
  // ERROR_MESSAGES.WRONG_ARGUMENT_TYPE
  WRONG_ARGUMENT_TYPE: /argument \d of type '[\w:><]+'/,
  CANNOT_CONVERT_ARG: /argument \d of type '[\w:><]+'/,
  NUMBER_NOT_IN_RANGE: /argument \d of type '[\w:><]+'/
}

// Error messages for Emscripten-based Iroha Library
// You can export if you use Emscripten
const ERROR_MESSAGES_EMSCRIPTEN = {
  ILLEGAL_NUMBER_OF_ARGUMENTS: /function \w+.\w+ called with \d arguments, expected \d args!/,
  WRONG_ARGUMENT_TYPE: /Cannot pass .+ as a \w+/,
  CANNOT_CONVERT_ARG: /Cannot convert "\w+" to .+/,
  NUMBER_NOT_IN_RANGE: /Passing a number .+/
}

module.exports.ERROR_MESSAGES = ERROR_MESSAGES_EMSCRIPTEN