# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
- This software is now versioned, starting at v0.1.0
- `main.cc` now uses `boost::program_options` to provide command-line arguments and options,
  primarily serial port device name, FT8 frequency, log file, and CW offset frequency are
  configurable options rather than hardcoded options

[Unreleased]: https://github.com/jdelacroix/qcx_fsk_cat/compare/v0.1.0...HEAD