# Argon Browser

**A simpler browsing experience, built on Brave and Chromium.**

Argon is an independent browser project inspired by the minimal direction of
Brave Origin. This repository is a fork of
[Brave Core](https://github.com/brave/brave-core) and contains the source changes,
patches, and build tools used to develop Argon.

The name comes from **argon (Ar)**, the noble gas with atomic number **18**.
It reflects the project's intended character: quiet, restrained, and focused
on browsing.

[Source code](https://github.com/Argon-Browser/argon-core) ·
[Upstream documentation](https://github.com/brave/brave-browser/wiki) ·
[License](./LICENSE)

## Project status

Argon is in early development. The fork inherits Brave's code and build system;
Argon-specific branding, feature selection, packaging, and release workflows
are being established.

The goals below describe the intended direction, not a list of completed
features. Existing Brave identifiers, resources, and service integrations may
still appear in the source and in development builds. Forking this repository
alone does not select or validate a Brave Origin build configuration.

## Direction

- **Minimal interface:** keep everyday browsing at the center of the experience.
- **Privacy:** build on the privacy protections available in the upstream code.
- **Focused features:** evaluate optional integrations against the project's
  simpler browsing goals.
- **Maintainability:** keep changes focused so upstream fixes can be integrated
  and tested regularly.
- **Open development:** document changes and make the source available for
  inspection and contribution.

## Repository structure

Argon uses the build layout inherited from Brave:

| Component | Role |
| --- | --- |
| [Chromium](https://chromium.googlesource.com/chromium/src.git) | Browser foundation, checked out into `src/`. |
| [argon-core](https://github.com/Argon-Browser/argon-core) | This fork, checked out into `src/brave/`. |
| [adblock-rust](https://github.com/brave/adblock-rust) | Upstream ad-blocking engine used by Brave. |

Keep the directory name **`src/brave`**, even when the repository is named
`argon-core`. The inherited build tools expect that layout. Chromium and other
dependencies are fetched during initialization; this repository is not a
standalone checkout of the entire browser source tree.

## Development

The commands below follow the inherited Brave build workflow. They are a
starting point for development, not a validated Argon release recipe.

### Prerequisites

Install Git, the Node.js and pnpm versions required by
[`package.json`](./package.json), and the dependencies for your host platform:

- [Linux](https://github.com/brave/brave-browser/wiki/Linux-Development-Environment)
- [macOS](https://github.com/brave/brave-browser/wiki/macOS-Development-Environment)
- [Windows](https://github.com/brave/brave-browser/wiki/Windows-Development-Environment)

Upstream also documents
[Android](https://github.com/brave/brave-browser/wiki/Android-Development-Environment)
and [iOS](https://github.com/brave/brave-browser/wiki/iOS-Development-Environment).
These references describe Brave's platform support; Argon builds must be
validated separately.

Compiling a Chromium-based browser requires substantial disk space, memory,
and processing time. Check the platform requirements before initializing the
checkout, including when using a remote development environment.

### Clone and initialize

```bash
mkdir -p argon/src
git clone https://github.com/Argon-Browser/argon-core.git argon/src/brave
cd argon/src/brave
pnpm run init
```

Initialization installs JavaScript dependencies and downloads Chromium and the
other required projects. See the upstream
[build configuration guide](https://github.com/brave/brave-browser/wiki/Build-configuration)
for service-dependent settings and release configuration.

### Build and run

For the default component development build:

```bash
pnpm run build
pnpm start Component
```

Other inherited build configurations:

| Command | Configuration |
| --- | --- |
| `pnpm run build Debug` | Debug build. |
| `pnpm run build Static` | Statically linked build. |
| `pnpm run build Release` | Release build configuration. |

Running a release build does not by itself configure Argon branding, signing,
distribution, or automatic updates.

### Sync dependencies

Commit your work before updating the checkout or synchronizing dependencies.
From `src/brave`, run:

```bash
pnpm run sync
```

This uses the current checkout's configuration to synchronize dependencies and
apply patches. Incorporating new Brave changes into the Argon fork is a
separate Git integration and review step.

### Documentation

- [Source documentation](./docs/README.md)
- [Upstream developer wiki](https://github.com/brave/brave-browser/wiki)
- [Build troubleshooting](https://github.com/brave/brave-browser/wiki/Troubleshooting)
- [Rust development](./docs/rust.md)

Some inherited documentation still refers to Brave infrastructure and internal
resources. Access to those services is not provided by this fork.

## Contributing

Keep changes small, explain their purpose, and include the validation relevant
to the affected behavior. Submit Argon changes to this repository.

The inherited [contributing guidelines](./CONTRIBUTING.md) provide upstream
technical context; Brave-specific submission and contact instructions apply
to the upstream project.

For security-sensitive changes, consult Chromium's
[security rules](https://chromium.googlesource.com/chromium/src/+/refs/heads/main/docs/security/rules.md)
and [IPC review guidance](https://chromium.googlesource.com/chromium/src/+/HEAD/docs/security/ipc-reviews.md).
The inherited [security policy](./SECURITY.md) describes upstream reporting
procedures and should not be interpreted as an Argon-specific response service.

## Credits and license

Argon builds on the work of the Brave, Chromium, adblock-rust, and other
open-source contributors whose code is included in this project.

This repository retains the **Mozilla Public License 2.0**; see
[`LICENSE`](./LICENSE). Included third-party code retains its respective
licenses and copyright notices.

Argon is an independent project and is not affiliated with or endorsed by
Brave Software or Google. Brave and Chromium names are used to identify the
upstream projects.
