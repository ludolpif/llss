# How to contribute

This projet is in early stage. Please get in touch before writing too much code.

# Notes

## vim development environnement configuration

On a debian trixie:
- make sure that `sudo ./build-dep-system.sh` and `./build-dep-user.sh` have been ran
- check `rust-analyser` availability:
```
$ rust-analyzer --version
rust-analyzer 1.81.0 (eeb90cd 2024-09-04)
```
- install some system-wide vim packages:
```sh
sudo apt install vim vim-ale vim-syntastic
```
- set current use `~/.vimrc` with:
```vim
runtime! defaults.vim
runtime! debian.vim
if &diff
  syntax off
else
  set mouse=
end
set background=dark

filetype plugin indent on

" vim-ale, usage: https://github.com/dense-analysis/ale?tab=readme-ov-file#usage
packadd! ale
let g:ale_linters = {'rust': ['analyzer']}
let g:ale_virtualtext_cursor = 'current'
let g:ale_set_highlights = 0
let g:ale_sign_column_always = 1
" You should not turn this setting on if you wish to use ALE as a completion
" source for other completion plugins, like Deoplete.
let g:ale_completion_enabled = 1
set omnifunc=ale#completion#OmniFunc
"let g:ale_completion_autoimport = 0

nmap <silent> <C-k> <Plug>(ale_previous_wrap)
nmap <silent> <C-j> <Plug>(ale_next_wrap)
EOT
```
- check, init and use this configuration:
    - `vim src/main.rs`
    - `:set filetype` should return `=rust`
    - `:ALEInfo` should contain : `Enabled Linters: ['analyzer']`
    - mangle the end of some struct defined in devmode crate
    - press `Ctrl+X Ctrl+O` to force `ale` to generate a first omni-completion
    - use `Ctrl+N` to choose a completion proposition (then return)
    - now omnicompletion should trigger itself automatically
    - note: code errors are checked only at file save
    - just `:w` and see virtual text where the gutter indicate it.

## Project very start

Read: https://www.rust-lang.org/learn/get-started

```sh
apt install git cargo
git init hello-bevy
cargo init hello-bevy
cd hello-bevy
cat src/main.rs
cat Cargo.toml

echo /target > .gitignore

editor build-deps-systemwide.sh
editor build-deps-userwide.sh

editor Makefile # optionnal, small hint for non-rust developers
make build-dep

cargo add bevy

# for developing, not releasing
cargo add bevy -F dynamic_linking

# from: https://bevyengine.org/learn/quick-start/getting-started/setup/#compile-with-performance-optimizations
editor Cargo.toml

# from: https://bevyengine.org/learn/quick-start/getting-started/setup/#alternative-linkers
mkdir .cargo
editor .cargo/config.toml

# from: https://bevyengine.org/learn/quick-start/getting-started/apps/
editor src/main.rs

make # shows an empty window and a console "Hello, World!"

editor README.md
editor CONTRIBUTING.md

git status
git add .
git commit -m "Project skeleton from learnings done in hello_bevy."
```
