# Procedural Terrain Generation Engine

The engine procedurally generates a terrain of different biomes depending on different heights. The engine is still under development.


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




