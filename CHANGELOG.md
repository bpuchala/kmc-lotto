# Changelog

All notable changes to `kmc-lotto` will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [v1.0a1] - 2024-02-08

Changes are described beginning from https://github.com/jonaskaufman/kmc-lotto/commit/711a1a74243c5591ec48b5f314d01bf55f2df82b

### Changed

- Change build to use cmake
- Add EngineType template parameter, with default value std::mt19937_64, to lotto::EventSelectorBase and derived classes
- Make lotto::EventSelectorBase::random_generator a shared_ptr
- Add total_rate accessor to lotto::RejectionFreeEventSelector
- Add missing include of <memory> in lotto/random.hpp

