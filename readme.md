# LaBattleships

LaBattleships is a 2 player battleships style game that is played on 2 LaFortuna devices

## Install

To play LaBattleships you will need 2 LaFortunas.

Inside `laBattleships.h` you wil find:

```c
#define PLAYER 0
```

Make sure you set one device to `PLAYER 0` and the second to `PLAYER 1`.

You can compile and flash the firmware then by running `make`

If you are on linux you may have to use sudo.

## Wiring

The devices need to communicate and they do so via [USART](https://en.wikipedia.org/wiki/Universal_synchronous_and_asynchronous_receiver-transmitter). To connect them you will need to connect the following pins as written on the back of the device:

| Device 1 | Device 2 |
| -------- | -------- |
| RXD1     | TXD1     |
| TXD1     | RXD1     |
| XCK1     | XCK1     |

## Contributors

- [Will Kelly](https://github.com/wkelly1)
- [Stefan Wiecek](https://github.com/stefanwiecek)

## Credits

- Peter Dannegger’s - Rotary encoder and switch debouncing
- Steve Gunn’s - Display driver
