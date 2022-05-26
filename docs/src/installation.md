# CBMC installation

## MacOS

On MacOS, we recommend installing with [brew](https://brew.sh/). Install with

```bash
brew tap aws/tap
brew install cbmc cbmc-viewer cbmc-starter-kit litani universal-ctags
```

The [brew home page](https://brew.sh/) gives instructions for
installing `brew`. The first command taps the AWS repository that
contains some of the brew packages, and the second command installs the
packages. Installing `ctags` is recommended but optional, see
[below](#installation-notes).

## Ubuntu

On Ubuntu, we recommend installing by downloading the `cbmc` and `litani`
installation packages for your operating system from the
[CBMC release page](https://github.com/diffblue/cbmc/releases/latest) and the
[Litani release page](https://github.com/awslabs/aws-build-accumulator/releases/latest).
Install with
```bash
sudo apt install python3 python3-pip *cbmc*.deb *litani*.deb universal-ctags
python3 -m pip install cbmc-viewer cbmc-starter-kit
```

The [python download page](https://www.python.org/downloads/)
gives instructions for installing python and pip.
Installing `ctags` is recommended but optional, see
[below](#installation-notes). The `pip` installation packages for
`cbmc-viewer` and `cbmc-starter-kit` can be used on any machine with
`python3` including MacOS.

## Windows

On Windows, we recommend using the [Windows Subsystem for Linux](
https://docs.microsoft.com/en-us/windows/wsl/) (WSL) and using
the Ubuntu instructions above.  CBMC and CBMC viewer
will run natively on Windows, but Litani and the CBMC starter kit
that depends on Litani are not supported on Windows.
To install natively on Windows (without using WSL),
download the Windows installation package from the
[CBMC release page](https://github.com/diffblue/cbmc/releases/latest) and
run
```bash
python3 -m pip install cbmc-viewer
msiexec /i cbmc*.msi
PATH="C:\Program Files\cbmc\bin";%PATH%
```

## Installation notes

If you have difficulty installing these tools, please let us know
by submitting a
[GitHub issue](https://github.com/model-checking/cbmc-training/issues).

The installation of `ctags` is optional, but without `ctags`, `cbmc-viewer`
will fail to link some symbols appearing in error traces to their
definitions in the source code.
The [ctags](https://en.wikipedia.org/wiki/Ctags) tool has a long history.
The original ctags was replaced by
[exuberant ctags](http://ctags.sourceforge.net/) which was replaced by
[universal ctags](https://github.com/universal-ctags/ctags).
Any version will work better than none, but we recommend universal ctags.
