{
  "targets":[
    {
      "target_name":"irohanodejs",
      "include_dirs":["../../shared_model", "../../libs", "../../external/src/martinmoene_optional/include", "../../irohad", "../../schema"],
      "libraries":["~/src/iroha/example/node/build/shared_model/bindings/libbindings.a", "~/src/iroha/example/node/build/shared_model/bindings/irohanode.so"],
      "sources":["./build/shared_model/bindings/bindingsJAVASCRIPT_wrap.cxx"],
      "cflags_cc":["-std=c++14", "-fexceptions"],
      "cflags_cc!":["-fno-rtti"]
    }
  ]
}
