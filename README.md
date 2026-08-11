# open-mufc

An open source firmware implementation for the [Multipurpose UFC][mufc-url].

> [!IMPORTANT]
> **Not affiliated with Multipurpose UFC.** This is an independent, community-driven
> project and is not endorsed by, sponsored by, or associated with the makers of the
> Multipurpose UFC. All product names and trademarks are the property of their
> respective owners.

> [!WARNING]
> **Use at your own risk.** This firmware is provided "as is", without warranty of any
> kind. Flashing it to your hardware may void warranties or damage your device. You are
> solely responsible for any consequences of using this software.

## Supported aircraft

The firmware supports [DCS][dcs-bios-url] and [Falcon BMS][fbac-url] by compiling the firmware using the `env:dcs` and `env:bms` environments respectively.

### DCS

| Aircraft | Status | Comment |
| --- | :---: | --- |
| AJS-37 | 🚧 | In progress, rudimentary CK 37 support added |
| F-16C | ✅ | Supports DED, CMDS and (some) lights |
| F/A-18C | ❌ | Planned, not yet implemented |

### Flacon BMS

| Aircraft | Status | Comment |
| --- | :---: | --- |
| F-15 | ❌ | Planned, not yet implemented |
| F-16 | ❌ | Planned, not yet implemented |

## License

Distributed under the `GPL 3.0` license. See [LICENSE][license-url] for more information.

[license-url]: https://github.com/tobier/open-mufc/blob/master/LICENSE

[dcs-bios-url]: https://github.com/DCS-Skunkworks/dcs-bios
[fbac-url]: https://github.com/Bacon8tor/FalconBMSArduinoConnector

[mufc-url]: https://www.multipurpose-ufc.com/