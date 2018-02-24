{
  'variables': {
    'IROHA_HOME': '../../../'
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
            '<(IROHA_HOME)/shared_model/bindings/CMakeLists.txt'
          ],
          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/Makefile',
          ],
          'action': [
            'cmake', 
            '-H<(IROHA_HOME)', 
            '-B<(SHARED_INTERMEDIATE_DIR)', 
            '-DSWIG_NODE=ON', 
            '-DSHARED_MODEL_DISABLE_COMPATIBILITY=ON', 
            '-DCMAKE_BUILD_TYPE=Release', 
            '-DSWIG_EXECUTABLE=/opt/swig/bin/swig'
          ]
        },
        {
          'action_name': 'build',
          'message': 'Build shared_model libraries by CMake...',
          'inputs': [
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/Makefile',
          ],
          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/bindingsJAVASCRIPT_wrap.cxx',
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/libbindings.a',
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/irohanode.so'
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
      'copies': [
        {
          'files': [
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/irohanode.so'
          ],
          'destination': '<(module_path)'
        }
      ]
    },
    {
      'target_name': '<(module_name)',
      'dependencies': [ 'shared_model' ],
      'include_dirs': [
        '<(IROHA_HOME)/shared_model',
        '<(IROHA_HOME)/libs',
        '<(IROHA_HOME)/external/src/martinmoene_optional/include',
        '<(IROHA_HOME)/irohad',
        '<(IROHA_HOME)/schema'
      ],
      'sources': [
        '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/bindingsJAVASCRIPT_wrap.cxx'
      ],
      'cflags_cc': ['-std=c++14', '-fexceptions', '-DDISABLE_BACKWARD'],
      'cflags_cc!': ['-fno-rtti'],
      'conditions': [
        [
          'OS != "mac"',
          {
            'libraries': [
              '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/libbindings.a',
              '<(module_path)/irohanode.so'
            ]
          }
        ],
        [
          'OS == "mac"',
          {
            'libraries': [
              '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/libbindings.a',
              '<(module_path)/irohanode.dylib'
            ],
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
