{
  "targets": [
    {
      "target_name": "ssl_stat",
      "sources": [ "src/ssl_stat.cc"],
      "link_settings": {
          "libraries": [
              "-lcurl"
          ]
      },
    }
  ]
}