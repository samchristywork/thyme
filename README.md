## Overview

## Features

## Setup

## Usage

```
Usage: thyme [options] command [args...]

Run a command and record its output

Options:
  -h, --help               Show this help message
  -v, --version            Show version and license information
  -o, --stdout <file>      The file to write stdout to
  -e, --stderr <file>      The file to write stderr to
  -t, --timeout <seconds>  The maximum time to run the command for

Examples:
  thyme -o stdout.txt -e stderr.txt -t 10 sleep 5
  thyme bash -c "ls"
```

## Limitations

## Dependencies

```
```

## License

This work is licensed under the GNU General Public License version 3 (GPLv3).

[<img src="https://s-christy.com/status-banner-service/GPLv3_Logo.svg" width="150" />](https://www.gnu.org/licenses/gpl-3.0.en.html)
