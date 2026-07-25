# Ascent

A points-based "crash" game built with [Felgo](https://felgo.com) and Qt 6.

A balloon rises and a multiplier climbs from 1.00x. It pops at a random — but
pre-committed — point. Cash out before it pops and your stake is multiplied.
Wait too long and you lose it.

No real money. Points only.

> **Status:** work in progress. Built as a challenge project,
> developed in the open. See the commit history for progress.

## Features

- Continuous rounds with a betting window, so you can watch the ones you sit out
- Provably fair crash points: the seed hash is shown before the round and
  revealed after, so any round can be verified
- Auto cash out at a target multiplier
- Persistent balance, round history and statistics

## Built with

- Felgo SDK 4.3.1
- Qt 6.8.3
- CMake
- Qt Test for the game logic

## Platforms

macOS and iOS from a single codebase.

## Building

Instructions follow once the project skeleton lands.

## Tutorial

A step-by-step tutorial explaining how to build this game is written alongside
the code in QDoc. See `doc/`.
