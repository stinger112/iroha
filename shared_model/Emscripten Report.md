0. Устанавливаем переменные среды нужные для кросскомпиляции

```
source /opt/emsdk/emsdk_set_env.sh
```




1. Скопировать хидеры **Boost 1.65.1** в *${EMSCRIPTEN_ROOT_PATH}/system/include*

```
# TODO: Can we use system/local to store headers|libs|share|etc?
```




2. Сборка Protobuf

```
# TODO: We can use CMAKE_INSTALL_PREFIX:PATH=$EMSCRIPTEN/system to install Protobuf to Emscripten directory 
emcmake cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -Dprotobuf_BUILD_TESTS=OFF \
    -DCMAKE_CXX_FLAGS="-O3" \
    -Hcmake \
    -Bbuild

# TODO: Can we use install target for just libprotobuf?
cmake --build build --target libprotobuf
```


## Compilation problems

### The first problem (target libprotobuf):
Problems with macro GOOGLE_FALLTHROUGH_INTENDED and clang::fallthrough
https://github.com/google/protobuf/issues/4122

#### Solution:
Patch file */root/dev/protobuf/src/google/protobuf/stubs/port.h*: comment out the definition of GOOGLE_FALLTHROUGH_INTENDED

```
#TODO: Can we can specify some compiler options to fix GOOGLE_FALLTHROUGH_INTENDED problem?

# LEGACY COMMAND DON'T USE IT
# emcmake cmake \
#    -DCMAKE_BUILD_TYPE=Release \
#    -Dprotobuf_BUILD_TESTS=OFF \
#    -Dprotobuf_BUILD_SHARED_LIBS=OFF \
#    -DCMAKE_CXX_FLAGS="-Wno-implicit-fallthrough" \ ?
#    -Hcmake \
#    -Bbuild
```

### The second problem (target libprotoc):

Error log:
```
[ 41%] Generating /root/dev/protobuf/src/google/protobuf/compiler/js/well_known_types_embed.cc
/bin/sh: 1: "/opt/emsdk/node/8.9.1_64bit/bin/node": not found
CMakeFiles/libprotoc.dir/build.make:64: recipe for target '/root/dev/protobuf/src/google/protobuf/compiler/js/well_known_types_embed.cc' failed
make[2]: *** [/root/dev/protobuf/src/google/protobuf/compiler/js/well_known_types_embed.cc] Error 127
make[2]: *** Deleting file '/root/dev/protobuf/src/google/protobuf/compiler/js/well_known_types_embed.cc'
CMakeFiles/Makefile2:217: recipe for target 'CMakeFiles/libprotoc.dir/all' failed
make[1]: *** [CMakeFiles/libprotoc.dir/all] Error 2
Makefile:129: recipe for target 'all' failed
make: *** [all] Error 2

```

#### Solution 
Not found yet!


## Install Protobuf
- Move content of *~/dev/protobuf/build/lib/cmake/protobuf* to */opt/emsdk/emscripten/1.37.39/system/share/protobuf/cmake*
Install script do this:
```
-- Installing: /usr/local/lib/pkgconfig/protobuf.pc
-- Installing: /usr/local/lib/pkgconfig/protobuf-lite.pc
	
-- Installing: /usr/local/lib/cmake/protobuf/protobuf-targets.cmake
-- Installing: /usr/local/lib/cmake/protobuf/protobuf-targets-release.cmake
-- Up-to-date: /usr/local/lib/cmake/protobuf
-- Installing: /usr/local/lib/cmake/protobuf/protobuf-config.cmake
-- Installing: /usr/local/lib/cmake/protobuf/protobuf-config-version.cmake
-- Installing: /usr/local/lib/cmake/protobuf/protobuf-module.cmake
-- Installing: /usr/local/lib/cmake/protobuf/protobuf-options.cmake
```

- Move *~/dev/protobuf/build/libprotobuf.a* to *$EMSCRIPTEN/system/lib*

- Move protobuf headers to *$EMSCRIPTEN/system/include*




3. Сборка iroha-ed25519

```
emcmake cmake -H. \
              -Bbuild \
              -DTESTING=OFF \
              -DBUILD=STATIC \
              -DCMAKE_BUILD_TYPE=Release \
              -DCMAKE_CXX_FLAGS="-O3" \
              -DCMAKE_INSTALL_PREFIX:PATH=$EMSCRIPTEN/system

cmake --build build --target install
```

Затем переместить файлы:
```
-- Install configuration: "Release"
-- Installing: /usr/local/lib/static/libed25519.a
-- Installing: /usr/local/include/ed25519.h
-- Up-to-date: /usr/local/include
-- Installing: /usr/local/include/ed25519
-- Installing: /usr/local/include/ed25519/ed25519.h
-- Installing: /usr/local/include/ed25519/ed25519
-- Installing: /usr/local/include/ed25519/ed25519/errcode.h
-- Installing: /usr/local/include/ed25519/ed25519/ed25519.h
-- Installing: /usr/local/include/ed25519/ed25519/crypto_types.h
-- Installing: /usr/local/include/ed25519/ed25519/randombytes.h
-- Installing: /usr/local/include/ed25519/ed25519/crypto_verify.h
-- Installing: /usr/local/include/ed25519/ed25519/sha256.h
-- Installing: /usr/local/include/ed25519/ed25519/sha512.h
-- Installing: /usr/local/share/ed25519/cmake/ed25519Config.cmake
-- Installing: /usr/local/share/ed25519/cmake/ed25519Config-release.cmake
```

В директорию *${EMSCRIPTEN_ROOT_PATH}/system/include*




4. **OPTIONAL** Install Node.js headers

..Нужна дополнительная информация...




##############################################################################################

# Compile target **bindings** to bytecode

```
# TODO: We can use add_compile_options() or target_compile_definitions() CMake's commands
# TODO: We can add optimization -Ox
emcmake cmake -H. \
              -Bbuild \
              -DTESTING=OFF \
              -DCMAKE_BUILD_TYPE=Release \
              -DCMAKE_CXX_FLAGS="-O0 -std=c++14" \
              -DCMAKE_PREFIX_PATH=$EMSCRIPTEN/system \
              -DEMSCRIPTEN=ON

cmake --build build --target bindings
```

Then you need compile bytecode to javascript:

```
# TODO: We can add optimization -Ox same as on bytecode compilation
emcc libbindings.bc -o irohalib.js
```



# Create C++ glue code to interact with JS

## Embind

```sh
cd /opt/iroha/shared_model/bindings

# TODO: We can add optimization -Ox same as on bytecode compilation
emcc -c -std=c++14 -I/opt/iroha/shared_model -I/opt/iroha/libs model_crypto_embind.cpp -o model_crypto_embind.bc
emcc -c -std=c++14 -I/opt/iroha/shared_model -I/opt/iroha/libs -I/opt/iroha/irohad -I/opt/iroha/schema model_query_builder_embind.cpp -o model_query_builder_embind.bc
emcc --bind -s DISABLE_EXCEPTION_CATCHING=0 ../build/bindings/libbindings.bc model_crypto_embind.bc model_query_builder_embind.bc -o irohalib.js
```

**Build with additional dev arguments**

```sh
emcc -c -std=c++14 -c -Wno-deprecated-declarations -I/opt/iroha/shared_model -I/opt/iroha/libs model_crypto_embind.cpp -o model_crypto_embind.bc
emcc -c -std=c++14 -c -Wno-deprecated-declarations -I/opt/iroha/shared_model -I/opt/iroha/libs -I/opt/iroha/irohad -I/opt/iroha/schema model_query_builder_embind.cpp -o model_query_builder_embind.bc
emcc --bind -s DISABLE_EXCEPTION_CATCHING=0 -s EXTRA_EXPORTED_RUNTIME_METHODS='["AsciiToString"]' ../build/bindings/libbindings.bc model_crypto_embind.bc model_query_builder_embind.bc -o irohalib.js
```

## WebIDL

```sh
python $EMSCRIPTEN/tools/webidl_binder.py bindings.idl glue
```




# Debug Node.js app remotely

```sh
node --inspect-brk=0.0.0.0:9229 test.js
```




# Отчёт по средствам описания биндингов (интерфейсов для JS)

## Embind

### Pros:
1. Сильная интеграция с плюсовым кодом. Работает автофолдинг.
2. Type Inference. При изменении плюсового кода (например сигнатур) сразу будет видна ошибка или биндинг перестроится автоматически.
3. Ошибки показывает компилятор

### Cons:
1. Дополнительные телодвижения для Resource Management: нужно использовать `smart_pointer` или JS функции `delete()`/`deleteLater()`
2. Синтаксис менее декларативный чем в WebIDL

## WebIDL

### Pros
1. Более декларативный синтаксис
2. Широкая поддержка как W3C стандарта




# Problems

## Exceptions:
Uncaught number instead exception 
https://groups.google.com/forum/#!topic/emscripten-discuss/ZRhTbpeF6SI
Need to add in compilation process
```
-s EXTRA_EXPORTED_RUNTIME_METHODS='["AsciiToString"]'
```
Function to get text message
```
function whatEmscriptenException(ptr) {
    // 8 should be the vtable offset for the what function
    var fPtr = iroha.HEAPU32[(iroha.HEAPU32[(ptr) >> 2] + 8) >> 2];
    console.info(iroha.AsciiToString(iroha.dynCall_ii(fPtr, ptr)));
}
```