# hwinspect

`hwinspect` is a C++ utility for inspecting hardware on OpenBSD and looking up
devices in the [OpenBSD Device Support Database](https://obsd-device-support.vercel.app/).

The goal is to make it easier to identify the hardware in an OpenBSD system
and, where known, the retail or rebranded products that share the same hardware
identifiers.

## Current Features

- Reads and parses `usbdevs -v` output
- Identifies USB devices by vendor ID and product ID
- Reports the driver currently attached by OpenBSD
- Queries the OpenBSD Device Support Database API for matching devices
- Displays names from the OpenBSD device database separately from names
  reported by the hardware
- Displays known retail or rebranded product names when available
- Performs device database lookups concurrently

Example output:

    /dev/usb0  addr 02  0x0bda:0xf179  Realtek  802.11n  urtwn0
    OpenBSD: Realtek RTL8188FTV (urtwn)

The first line contains information reported by the local system. The
`OpenBSD:` line contains information from the OpenBSD Device Support Database.

## Why?

The names reported by hardware or used in OpenBSD's device tables do not
necessarily correspond to the product names users encounter when buying
hardware. A device may identify itself with a generic name, and multiple
products may share the same vendor and product IDs.

The OpenBSD Device Support Database records the device information found in
OpenBSD's sources along with known retail and rebranded product names.
`hwinspect` connects hardware detected on a running OpenBSD system with that
information.

## Building

`hwinspect` is currently developed on OpenBSD using C++20.

Dependencies:

- curl / libcurl
- nlohmann-json

On OpenBSD these can be installed with packages.

Build with:

    make

## Testing

The USB parser has a small test program using captured `usbdevs` output:

    make test

## How It Works

`hwinspect` runs `usbdevs -v` and parses its output into a normalized
representation of each USB device.

It then queries the OpenBSD Device Support Database API using the device's bus,
vendor ID, and product ID. JSON responses are parsed with nlohmann/json and
converted into C++ data structures before being displayed.

Database lookups for detected devices are performed concurrently.

## Planned Work

- PCI device support using saved `pcidump` output
- Improved output and command-line options
- Additional tests and error handling

## Related Project

The device data and API are provided by the
[OpenBSD Device Support Database](https://github.com/jakemsr/obsd-device-support).
