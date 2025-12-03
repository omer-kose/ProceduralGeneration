# Procedural Terrain Generation Engine

<p align="center">
  <img src="https://github.com/omer-kose/ProceduralGeneration/assets/44121631/3542dce3-c602-4168-9824-49a2b78dd72b" width = "50%">
  <br>
  An example view of the program
</p>

The engine procedurally generates a terrain of different biomes depending on different heights.


## Development Environment

- The engine is written in C++
- OpenGL is used as the rendering API
- For GUI, ImGui is used

## The Directory Tree

```bash
├── README.md
├── External/Include
│   ├── External Libraries like GLFW, ImGui etc.
│   ├── Progen
│       └── Engine Code    
│
├── Shaders
│
└── ProceduralGeneration
    └── main.cpp
```

## Some Implementation Details

- Perlin Noise is used for the heightmap generation
- Each biome has a height range. Depending on height the corresponding biome is picked.
- The height values sampled from the noise map are undergone a non-linear function. This allows users to customize the height shape of the map with the curve editor GUI. 


## TODO's (This will always be updated (I hope))
- Textures for biomes
- Better lighting model
- A cubemap 
- A water shader
- Chunk system to create infinite worlds




