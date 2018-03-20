# iroha-js

Official Iroha JavaScript Library. https://github.com/hyperledger/iroha

## Usage

You can use regular Node.js style to import **iroha-lib** package and related protobufs:

```javascript
const iroha = require('iroha-lib')

const blockTransaction = require('iroha-lib/pb/block_pb.js').Transaction
const endpointGrpc = require('iroha-lib/pb/endpoint_grpc_pb.js')

...

```

Watch usage in *example* folder.

## Build

You need this section if you want to build **iroha-lib** manually for publishing or if your architecture/OS not supported yet.

### Prerequisities

In order to build NPM package by `node-gyp` on your machine you need some global installed dependencies: 

1. CMake (>=3.7)

2. Protobuf (>3.5)

**WARNING! If you have already installed SWIG, you MUST install patched version instead using [this patch](https://github.com/swig/swig/pull/968.patch)**

Also, you can watch `install-dependencies.sh` script - it may help to install neccessary dependencies on your system.

#### For Mac users

To build **iroha-lib** on Mac the following dependencies should be installed:

```sh
brew install node cmake # Common dependencies
brew install autoconf automake # SWIG dependencies
brew install protobuf boost # Iroha dependencies
```

### Build process

1. Clone full Iroha repository

```sh
git clone https://github.com/hyperledger/iroha.git

```

2. Go to the NPM package directory and start build

```sh
cd iroha/shared_model/packages/javascript
npm install
```

That's pretty all.

---


This NPM package is in deep pre-alfa phase, so if you have any troubles, feel free to create a new issue or contact contributors from *package.json*.
