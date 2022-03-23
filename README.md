## CBMC Training

This repository contains our documentation on how to use CBMC.

The documentation is in directory `docs` and is generated using the
tool `mdbook` from a collection of markdown files in the directory
`docs/src`.  The [`mdbook`](https://rust-lang.github.io/mdBook/)
tool is a standard tool for preparing documentation in the Rust community.

### Installing `mdbook`

On MacOS, run
```bash
brew install mdbook
```
On Linux and Windows, download the compressed binary from the
[mdbook release page](https://github.com/rust-lang/mdBook/releases/latest),
uncompress it, and put it in your search path.

### Building documentation

Build the documentation with the commands
```bash
cd docs
mdbook build
```
This will place the documentation in the directory `docs/book`.

View the documentation with
```bash
cd docs
mdbook serve --open
```
This will start up a web server rooted in `docs/book` and lets you
proofread your work before publication.

## Security

See [CONTRIBUTING](CONTRIBUTING.md#security-issue-notifications) for more information.

## License Summary

The documentation is made available under the Creative Commons Attribution-ShareAlike 4.0 International License. See the LICENSE file.

The sample code within this documentation is made available under the MIT-0 license. See the LICENSE-SAMPLECODE file.
