{
  "targets": [
    {
      "target_name": "dds_bridge",
      "sources": [ "src/native/addon.cpp" ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "src/native/dds"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').targets\"):node_addon_api"
      ],
      "libraries": [
        "<(module_root_dir)/src/native/dds/dds.lib"
      ],
      "msvs_settings": {
        "VCCLCompilerTool": {
          "DebugInformationFormat": "0"
        }
      }
    }
  ]
}
