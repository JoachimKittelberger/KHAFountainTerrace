; overwrite setting from platformio.ini

; use OTA
[env:esp32dev]
upload_protocol = espota
upload_port = 192.168.1.1
upload_flags =
    --port=3232
    --auth=12345678

