{
  'targets': [
    {
      'target_name': 'shared_model',
      'type': 'none',
      'actions': [
        {
          'action_name': 'build',
          'inputs': [
            'scripts/prebuild.sh',
          ],
          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/bindingsJAVASCRIPT_wrap.cxx',
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/libbindings.a',
            '<(SHARED_INTERMEDIATE_DIR)/shared_model/bindings/irohanode.so'
          ],
          'action': [
            'sh', 'scripts/prebuild.sh', '<(SHARED_INTERMEDIATE_DIR)', '<(module_path)'
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
        '../../../shared_model',
        '../../../libs',
        '../../../external/src/martinmoene_optional/include',
        '../../../irohad',
        '../../../schema'
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
