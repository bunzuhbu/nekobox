# NekoBox for PC
Qt based Desktop cross-platform GUI proxy utility, empowered by [Sing-box](https://github.com/SagerNet/sing-box)

Supports Windows 11/10/8/7 / Linux / MacOS out of the box.

### GitHub Releases (Portable ZIPs, Windows installers)

[![GitHub All Releases](https://img.shields.io/github/downloads/qr243vbi/nekobox/total?label=downloads-total&logo=github&style=flat-square)](https://github.com/qr243vbi/nekobox/releases)

### OBS repository
[NekoBox repository](https://software.opensuse.org//download.html?project=home%3Ajuzbun%3ANekoBox&package=nekobox) for various linux distributions.

## Supported protocols

- SOCKS
- HTTP(S)
- Shadowsocks
- Trojan
- VMess
- VLESS
- TUIC
- Hysteria
- Hysteria2
- AnyTLS
- Wireguard
- SSH
- Custom Outbound
- Custom Config
- Chaining outbounds
- Extra Core

## Subscription Formats

Various formats are supported, including share links, JSON array of outbounds and v2rayN link format as well as limited support for Shadowsocks and Clash formats.

## Credits

- [SagerNet/sing-box](https://github.com/SagerNet/sing-box)
- [Qv2ray](https://github.com/Qv2ray/Qv2ray)
- [Qt](https://www.qt.io/)
- [protorpc](https://github.com/chai2010/protorpc)
- [fkYAML](https://github.com/fktn-k/fkYAML)
- [quirc](https://github.com/dlbeer/quirc)
- [QHotkey](https://github.com/Skycoder42/QHotkey)
- [simple-protobuf](https://github.com/tonda-kriz/simple-protobuf)
- [quickjs](https://quickjs-ng.github.io/quickjs)
- [qrcodegen](https://www.nayuki.io/page/qr-code-generator-library)
- [Setup MinGW Github Action](https://github.com/bwoodsend/setup-winlibs-action)
- [Setup MSVC Github Action](https://github.com/ilammy/msvc-dev-cmd)
- [Setup Ninja Github Action](https://github.com/seanmiddleditch/gha-setup-ninja)
- [Cache Apt Pkgs Github Action](https://github.com/awalsh128/cache-apt-pkgs-action)
- [Setup Qt Github Action](https://github.com/jurplel/install-qt-action)
- [linuxdeploy](https://github.com/linuxdeploy/linuxdeploy)
- [throneproj](https://github.com/throneproj/Throne)
- [MinGW](https://www.mingw-w64.org)
- [MSVC](https://visualstudio.microsoft.com/)
- [go](https://go.dev/)
- [nekoray](https://github.com/MatsuriDayo/nekoray)
- [Open Build Service](https://openbuildservice.org/)
- [Github](https://github.com)
- [cv2pdb](https://github.com/rainers/cv2pdb)
- [cmake](https://gitlab.kitware.com/cmake/cmake)
- [ninja-build](https://ninja-build.org/)
- [codeclysm/extract](https://github.com/codeclysm/extract)
- and many others...

## FAQ
**Why does my Anti-Virus detect NekoBox as malware?** <br/>
NekoBox's built-in update functionallity downloads the new release, removes the old files and replaces them with the new ones, which is quite simliar to what malwares do, remove your files and replace them with an encrypted version of your files.
Also the `System DNS` feature will change your system's DNS settings, which is also considered a dangerous action by some Anti-Virus applications.

**I got the msvcp140.dll not found error on windows** <br/>
The "msvcp140.dll not found" error usually means that the Microsoft Visual C++ Redistributable is missing or corrupted. To fix this, try install or reinstall the Microsoft Visual C++ Redistributable from the official Microsoft website
[Official Microsoft website for Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170)

**Is setting the `SUID` bit really needed on Linux?** <br/>
No, it is not needed, but if `SUID` does not configured properly, the NekoBox will ask for administrator password to order to restart nekobox_core with administrator privilegies, if NekoBox is not already running as root. NekoBox will ask for password for once, and will not configure `SUID`.

**Why does my internet stop working after I force quit NekoBox?** <br/>
If NekoBox is force-quit while `System proxy` is enabled, the process ends immediately and NekoBox cannot reset the proxy. <br/>
Solution:
- Always close NekoBox normally.
- If you force quit by accident, open nekobox again, enable `System proxy`, then disable it- this will reset the settings.
