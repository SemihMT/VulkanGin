# VulkanGin

Welcome to the **VulkanGin** repository! VulkanGin is a first-person voxel world editor inspired by Minecraft. It allows users to place and break blocks, and fly around an infinitely generated world. The project is developed using C++ and Vulkan, providing a robust and efficient framework for rendering and interacting with vast voxel landscapes.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
## Overview

VulkanGin is a voxel world editor that offers an immersive first-person experience. The world is procedurally generated using simplex noise and is divided into chunks of 16x16x16 blocks. As you navigate the world, chunks are dynamically loaded and unloaded to ensure seamless exploration.

https://github.com/user-attachments/assets/08a172bf-f377-4436-ba01-1247d96e0eb7


## Features

- **First-Person Controls**: Navigate the world with fluid first-person controls.
- **Voxel Editing**: Place and break blocks to shape the world around you.
- **Infinite World Generation**: Explore an endlessly generated world.
- **Dynamic Chunk Management**: Chunks are loaded and unloaded dynamically based on your position.
- **Simplex Noise Generation**: Procedurally generate terrain using simplex noise for natural-looking landscapes.
- **Efficient Rendering**: Utilize Vulkan for high-performance rendering.

## Installation

### Prerequisites

- C++20 compatible compiler
- CMake
- Vulkan SDK

### Building VulkanGin
1. Clone the repository:
```bash
git clone https://github.com/SemihMT/VulkanGin.git
cd VulkanGin
```

If Visual Studio 2022 or later is installed, I'd recommend opening the cloned repo using VS and building from there.
If not, plain CMake should do the trick.

## Usage

### Controls

- **W/A/S/D**: Move forward/left/backward/right
- **Space**: Fly up
- **Shift**: Fly down
- **L-Ctrl**: Speed up
- **Mouse**: Look around
- **Left Click**: Break block
- **Right Click**: Place block
- **Esc**: Exit the application

