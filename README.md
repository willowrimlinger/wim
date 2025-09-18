# wim
A very incomplete clone of vim.

## Build

### Ubuntu

```bash
sudo apt-get update
sudo apt-get install make libncurses-dev
make
```

## Run

wim is lean and lightweight, so it does not currently have the ability to create
new files or open empty files.

Please give it a hand and create a file with some text.

```bash
echo some text > file.txt
```

Then, run wim:

```bash
./wim file.txt
```
