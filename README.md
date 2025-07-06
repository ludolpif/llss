# Purpose and status
Libre Live Streaming Software (LLSS) is my attempt to mockup a Streaming software, to make live brodcasts on platforms like Twitch.tv.

This software will never be production-ready while I am the (sole) developer.

# Building
## Linux
I currently use Debian trixie (testing) for that. It will generate a binary with low system dependancy but needs `GLIBC_2.38` symbols which is not a good choice to distribute currently. This envrionnement is great for now to explore recent things around `rust`, `bevy`, and latest audio/video software stacks.
```sh
git clone https://github.com/ludolpif/llss
cd llss
sudo ./build-dep-system.sh
./build-dep-user.sh
cargo build
cargo run

cat Makefile # act as cheat sheet for non-rust developpers
```

# Contributing
See [CONTRIBUTING.md](CONTRIBUTING.md)
