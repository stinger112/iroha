# iroha-js
Modern version of JavaScript library for Iroha. https://github.com/hyperledger/iroha

## Build

### Prerequisities

1. CMake (>=3.7)

2. Protobuf (>3.5)

**If you want to publish self-contained NPM package, you MUST build Protobuf statically as described below:**
```
git clone --depth 1 --branch v3.5.1 https://github.com/google/protobuf
cd protobuf
cmake -Hcmake/ -Bbuild -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_SHARED_LIBS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_BUILD_TYPE=Release
sudo cmake --build build/ --target install -- -j"$(nproc)"
```
Also, you can set `-DCMAKE_INSTALL_PREFIX=<some_directory>` to customize install directory.

3. SWIG (patched, =3.0.12)

**If you have already installed SWIG, you MUST install patched version instead.**

```
git clone --depth 1 --branch rel-3.0.12 https://github.com/swig/swig
cd swig
./autogen.sh
./configure --without-pcre
git apply ${IROHA_HOME_DIR}/patch/add-nodejs8-support-to-swig.patch
make
sudo make install
```

### For Mac users

To build **iroha-lib** on Mac the following dependencies should be installed:

```
brew install node cmake # Common dependencies
brew install autoconf automake # SWIG dependencies
brew install protobuf boost # Iroha dependencies
```

---

This NPM package is in deep pre-alfa phase, so if you have any troubles, please contact maintainers from *package.json*.