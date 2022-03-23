# CBMC installation

## MacOS

On MacOS, install the CBMC tools with
```bash
python3 -m pip install cbmc-viewer
brew install cbmc litani universal-ctags
```
The installation of `ctags` is optional, see below.

## Ubuntu

On Ubuntu, install the CBMC tools by downloading the installation packages
for your operating system from the
[CBMC release page](https://github.com/diffblue/cbmc/releases/latest) and the
[Litani release page](https://github.com/awslabs/aws-build-accumulator/releases/latest)
and running
```bash
python3 -m pip install cbmc-viewer
sudo apt install *cbmc*.deb *litani*.deb universal-ctags
```
The installation of `ctags` is optional, see below.

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

The installation of `ctags` is optional, but without ctags, `cbmc-viewer`
will fail to link some symbols appearing in error traces to their
definitions in the source code.
The [ctags](https://en.wikipedia.org/wiki/Ctags) tool has a long history.
The original ctags was resplaced by
[exhuberant ctags](http://ctags.sourceforge.net/) which was replaced by
[universal ctags](https://github.com/universal-ctags/ctags).
We recommend using universal ctags.
