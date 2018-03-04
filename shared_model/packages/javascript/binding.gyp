{
  'variables': {
    'iroha_home_dir': '../../../',
    'protobuf_install_dir': '<!(echo $PROTOBUF_INSTALL_DIR)'
  },
  'targets': [
    {
      'target_name': 'shared_model',
      'type': 'none',
      'actions': [
        {
          'action_name': 'configure',
          'message': 'Generate CMake build configuration for shared_model...',
          'inputs': [
            '<(iroha_home_dir)/shared_model/bindings/CMakeLists.txt'
          ],
          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/Makefile',
          ],
          'action': [
            'cmake', 
            '-H<(iroha_home_dir)', 
            '-B<(SHARED_INTERMEDIATE_DIR)', 
            '-DSWIG_NODE=ON', 
            '-DENABLE_LIBS_PACKAGING=OFF',
            '-DSHARED_MODEL_DISABLE_COMPATIBILITY=ON', 
            '-DCMAKE_BUILD_TYPE=Release',

            # TODO: Maybe this is not necessary
            '-DCMAKE_POSITION_INDEPENDENT_CODE=ON',

            '-Dprotobuf_INCLUDE_DIR=<(protobuf_install_dir)/include',
            '-Dprotobuf_LIBRARY=<(protobuf_install_dir)/lib',
            '-Dprotoc_EXECUTABLE=<(protobuf_install_dir)/bin/protoc',
          ],
        },
        {
          'action_name': 'build',
          'message': 'Build shared_model libraries by CMake...',
          'inputs': [
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/Makefile',
          ],
          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/bindingsJAVASCRIPT_wrap.cxx',
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/libirohanode.a',
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/libbindings.a'
          ],
          'action': [
            'cmake', 
            '--build', '<(SHARED_INTERMEDIATE_DIR)',
            '--target', 'irohanode',
            '--',
            '-j<!(echo "$(getconf _NPROCESSORS_ONLN)")'
          ]
        },
      ],
      ###
      # Copy all necessary static libs to PRODUCT_DIR, so we ensure their existence!
      ###
      'copies': [
        {
          'files': [
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/libirohanode.a',
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/libbindings.a',
            '<(SHARED_INTERMEDIATE_DIR)/schema/libschema.a',
            '<(SHARED_INTERMEDIATE_DIR)/libs/generator/libgenerator.a',
            '<(SHARED_INTERMEDIATE_DIR)/libs/amount/libiroha_amount.a',
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/validators/libshared_model_stateless_validation.a',
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/cryptography/ed25519_sha3_impl/libshared_model_ed25519_sha3.a',
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/cryptography/ed25519_sha3_impl/internal/libcryptography.a',
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/cryptography/ed25519_sha3_impl/internal/libhash.a',

            '<(protobuf_install_dir)/lib/libprotobuf.a',
            '<(iroha_home_dir)/external/src/hyperledger_ed25519-build/libed25519.a'
          ],
          'destination': '<(PRODUCT_DIR)'
        }
      ]
    },
    {
      'target_name': '<(module_name)',
      'dependencies': [ 'shared_model' ],
      'include_dirs': [
        '<(iroha_home_dir)/shared_model',
        '<(iroha_home_dir)/libs',
        '<(iroha_home_dir)/external/src/martinmoene_optional/include',
        '<(iroha_home_dir)/irohad',
        '<(iroha_home_dir)/schema',
        '<(protobuf_install_dir)/include',
      ],
      'sources': [
        '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/bindingsJAVASCRIPT_wrap.cxx'
      ],
      'cflags_cc': ['-std=c++14', '-fexceptions', '-DDISABLE_BACKWARD'],
      'cflags_cc!': ['-fno-rtti'],
      'libraries': [
        '-L<(PRODUCT_DIR)',
        
        '-lirohanode', # Library contains SWIG runtime
        '-lbindings',
        '-lgenerator',
        '-lhash',
        '-liroha_amount',
        '-lschema',
        '-lshared_model_ed25519_sha3',
        '-lshared_model_stateless_validation',
        '-lcryptography',

        # Third-party libraries
        '-led25519',
        '-lprotobuf'
      ],
      'conditions': [
        [ 'OS == "mac"', {
            'xcode_settings': {
              'GCC_ENABLE_CPP_RTTI': 'YES',
              'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
              'OTHER_CFLAGS': ['-std=c++14', '-DDISABLE_BACKWARD']
            }
          }
        ]
      ]
    },
    {
      'target_name': 'action_after_build',
      'type': 'none',
      'dependencies': [ '<(module_name)' ],
      'copies': [
        {
          'files': [ '<(PRODUCT_DIR)/<(module_name).node' ],
          'destination': '<(module_path)'
        }
      ]
    }
  ]
}
