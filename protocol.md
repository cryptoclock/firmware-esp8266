
# Cryptoclock protocol v0.9.1

## Handshake

Server: 
```json
{"type": "welcome", "text": "Welcome client ..."}
```
Device:
```json
{"type": "hello", "model": "DPB0200", "uuid": "0a115964-659f-d095-b4f8-c2d468447ed9", "version": "1.0.0", "firmwareChecksum": "1df795aed8236272eb6f2ad3c5a489499c4a88597727a602fd0a03408564ce34"}
```

## Configuration
Device:
```json
{"type": "parameter", "name": "brightness", "value": 3}
{"type": "parameter", "name": "clock_interval", "value": 30}
{"type": "parameter", "name": "clock_mode", "value": 1}
{"type": "parameter", "name": "ntpserver", "value": "0.xnet.pool.ntp.org"}
{"type": "parameter", "name": "rotate_display", "value": 0}
{"type": "parameter", "name": "ticker_url", "value": "wss://ticker.cryptoclock.net:443/"}
{"type": "parameter", "name": "timezone", "value": 2}
{"type": "parameter", "name": "update_url", "value": "https://ticker.cryptoclock.net:443/"}
{"type": "diagnostics", "lastResetReason": {"cpu0": "RTCWDT_RTC_RESET", "cpu1": "EXT_CPU_RESET"}}
{"type": "parameters", "parameters": [ {"name": "brightness", "value": 3}, ... ]}
{"type": "fonts", "names": ["default","5x7"...]}
```

## Device commands

### tick
Data are then received as "tick" commands, associating format from "layout" command by "id" field.
```json
{"type":"tick","data":[{"id":"(UUID)","value":"6456.9"},...]}
```

### allTimeHigh
Highest ever value for given item can be provided by server in similar fashion:
```json 
{"type": "allTimeHigh", data: [{id: "(UUID)", "value": 99999.9},...] }
```

### message
Display rotating message on given screen
```json 
{"type": "message", "text": "Hello there!", "target": "(UUID)"}
```

### staticMessage
Display static message
```json 
{"type": "staticMessage", "text": "Hello there!", "durationSecs": 60.0, "target": "(UUID)"}
```

### imageBegin, imageChunk, imageEnd (only for ESP32 devices with RGB display)
Display an image, format must be 24-bit .bmp image, with same size as display image is split into X-bytes chunks (default 1024), each individually base64-encoded

```json
{"type": "imageBegin", "size": 6282, "chunk_size": 1024 }
{"type": "imageChunk", "offset":0, "chunk":"Qk2KGAAAAAAAA..."}
{"type": "imageChunk", "offset":1024, "chunk":"Qk2KGAAAAAAAA..."}
..
{"type": "imageEnd", action: "display", "durationSecs": 10.0}
```

Or store it on device's filesystem for use as logo
```json 
{"type": "imageEnd", action: "store", "filename": "logo.bmp"}
```

### countdown
Displays countdown, from specified number of seconds to 0.

```json
{"type": "countdown", "timeSecs": 60.0 }
```

### sound (only for devices with attached buzzer)
Plays a melody specified in RTTL format
```json
{"type": "sound", "melodyRTTL": "StarwarsI:d=16,o=5,b=100:4e,4e,4e,8c,p,g,4e,8c,p,g,4e,4p,4b,4b,4b,8c6,p,g,4d#,8c,p,g,4e,8p"}
```

### setTemplate (only for ESP32 devices with RGB display)
Templates are collections of data display formats indexed from 0.

Each format contains display position(x,y) and attributes such as color or text alignment.

Formats can be divided into _horizontal_/_vertical_ pairs; appropriate format is then selected based on physical device orientation.

Default templates are provided for 1-4 displayed items (see [data_templates.hpp](main/data_templates.hpp)),

Custom template can be set by server using _setTemplate_ command.

Server:
```json
{"type": "setTemplate", "template": 
  [
    {
      "index": 0,
      "horizontal": {
        "value": {
          "x": 2, 
          "y": 2,
          "maxCharacters": 6,
          "textAlignment": "right",
          "font": "default",
          "color": "#FFFFFF"
        },
        "text": {
          "x": 50,
          "y": 2,
          "maxCharacters": 3,
          "textAlignment": "left",
          "font": "default",
          "color": "#FFFFFF"
        }
      },
      "vertical": {
        ...
      }
    },
    {
      "index": 1,
      ...
    }
  ]
}
```

### layout (only for ESP32 devices with RGB display)
When "layout" command is received, appropriate template is selected based on number of items in "data" field  and each item is associated with particular format by it's index position. 

server:
```json
{"type": "layout", "data": [
  {"index": 0, "id": "(UUID)", "displayName": "USD", "source": "/bitfinex/btc/usd"}, 
  {"index": 1, "id": "(UUID2)", "displayName": "CZK", "source": "/coinmate/btc/czk"}
]}
```

Optional "format" field can be used to override some (or all) of template's format attributes.

(Note: If the format doesn't have "horizontal" or "vertical" fields, it is applied to both.)

server:
```json
{"type": "layout", "data": [
  {"index": 0, "id": "(UUID)", "displayName": "USD", "source": "/bitfinex/btc/usd", 
    "format": {
      "text": { 
        "color": "#FF0000"
      } 
    } 
  },
  {"index": 1, "id": "(UUID2)", "displayName": "CZK", "source": "/coinmate/btc/czk",
    "format": { 
      "horizontal": { 
        "value": { 
          "font": "5x7",
          "color": "#00FF00"
        }
      }, 
      "vertical": {
        "value": {
          "font": "4x6",
          "color": "#00FF00"
        }
      }
    }
  }
]}
```

If "source" for given index is "_clock", time is displayed at given position instead of value/name

server:
```json
{"type": "layout", "data": [
  {"index": 0, "id": "(UUID)", "displayName": "USD", "source": "/bitfinex/btc/usd"}
  {"index": 1, "id": "(UUID2)", "displayName": "", "source": "_clock"}
]}
```

###  parameter
set value of a parameter
```json
{"type": "parameter", "name": "timezone", "value": 3}
```

### requestParameters
Server requests all stored parameter values
```json
{"type": "requestParameters"}
```

### setTimeout
Server can set some of device's timeouts (in case of server source sending values too infrequently)
```json
{"type": "setTimeout", "dataReceivedTimeoutSecs": 180.0 }
```

### heartbeat
server sending heartbeat to keep the connection alive
```json
{"type": "heartbeat"}
```

### OTP
Server sends otp to the device. Password has to be presented to user.
```json
{"type": "OTP", "password": 123456}
```

### OTP_ACK
Server notifies about successful OTP usage
```json
{"type": "OTP_ACK"}
```

### triggerUpdate
Notifies client to check for firmware updates
```json
{"type": "triggerUpdate"}
```

### triggerReset
Notifies client to restart the device
```json
{"type": "triggerReset"}
```

## Server commands

### heartbeat
client sending heartbeat to keep server informed it's still connected
```json
{"type": "heartbeat"}
```

### OTPRequest
 User presses the OTP button on device 
```json
{"type": "OTPRequest"}
```

### error
Returning error message for a command
```json
{"type": "error", "command": "setImage", "text": "Error, Image size must be 64x32..."}
```


## Example flows
### OTP (One-time password) exchange
* User presses the OTP button on device --
C: ```{"type": "OTPRequest"}```
S: ```{"type": "OTP", "password": 123456}```
*  OTP is displayed on device 
*  User types in OTP into server web interface to pair the device 
S: ```{"type": "OTP_ACK"}```
* OTP is not displayed on the device anymore

### Server requests all stored parameter values
S: ```{"type": "requestParameters"}```
C: ```{"type": "parameter" ...```

## Notes

For backward compatibility on client side ignore lines beginning with ;
```json
; ignored line
```
for connecting to plain ws/wss data feeds - updates value at index 0, with id and displayName set to ""
```json
12345.6
```
