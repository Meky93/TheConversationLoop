# ECE 309 Project 2: The Conversation Loop

This project implements the core memory and streaming components of a C++ LLM harness.

## Overview

The project contains:

- `Message` — stores a message role and text.
- `Conversation` — a manually managed growable array of messages.
- `SentinelScanner` — detects `<|end_conversation|>` across streamed chunks.
- `Harness` — provided code that manages the conversation loop.
- `ScriptedModelClient` and `ReplayModelClient` — provided deterministic model clients.

The `Conversation` class demonstrates dynamic memory management and the Rule of Five. It performs deep copies, pointer-stealing moves, and automatic capacity growth. The `SentinelScanner` handles responses that arrive in arbitrary-sized chunks without printing the sentinel.

## Building

This project uses CMake and requires C++17.

```bash
cmake -S . -B build
cmake --build build
