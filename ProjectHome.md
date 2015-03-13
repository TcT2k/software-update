This aims at being a simple but powerful universal software update system to be included into your own projects.

# Easy To Use #
  1. Package the SoftwareUpdate binary with your application
  1. Create a config file describing your application version
  1. Host a config file describing a potential update on HTTP
  1. Call the SoftwareUpdate binary from your application on startup or just call it manually

# How To #
Include a _SoftwareUpdate.conf_ like this with your application:
```
[Software]
Id=SampleApp
Title=SampleApplication
Version=1.0.0.1000
PubKey=A1A2A3A4A5B1
UpdateURL=http://myserver/NewSoftwareUpdate.conf
```

Upload a _NewSoftwareUpdate.conf_ like the following on your softwares website:
```
[Software]
Version=1.0.1.1001

[Windows]
DownloadURL=http://myserver/SampleAppSetup.exe
Signtature=AaBsdSDaSdasSDErERFf

[Macintosh]
DownloadURL=http://myserver/SampleApp.pkg
```