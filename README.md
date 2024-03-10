![Banner](https://s-christy.com/sbs/status-banner.svg?icon=places/grass&hue=120&title=Thyme&description=A%20terminal%20based%20tool%20for%20monitoring%20long%20running%20commands)

## Overview

<p align="center">
  <img src="./assets/screenshot.png" width=500 />
</p>

`thyme` is a CLI program written in C++ for timing long-running commands and
monitoring their progress. Although there is no generalized way to determine how
long a command will run, the number of bytes that are written to `stdout` can be
a good proxy for some programs. Additionally, because the program writes the
`stdout` and `stderr` streams to files, you have the added benefit of being able
to save these streams, and monitor them while the command is running with `tail
-f`.

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
