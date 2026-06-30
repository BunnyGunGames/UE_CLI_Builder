# UE CLI Builder

Unreal Engine **5.5** C++ sample project for experimenting with **CLI-driven workflows** and AI-assisted development (Cursor + command-line builds). Spawns animated cube blocks from JSON data and lets you add more at runtime with a left-click.

## What it does

- Loads block positions from `Data/data.json` at startup
- Spawns engine cube meshes with staggered sine-wave bobbing animation
- **Left-click** spawns additional cubes at random non-overlapping positions near the cluster
- Uses **Enhanced Input** for mouse binding (no Blueprint required for the core loop)

## Requirements

- Unreal Engine **5.5** (see `EngineAssociation` in `CursorCPP.uproject`)
- Windows recommended (project targets DX12; `sync-build-files.bat` is Windows-only)
- Visual Studio 2022 with C++ game development workload (or Rider)

## Project layout

```
.
├── CursorCPP.uproject
├── sync-build-files.bat       # Copy local build/ output to UnrealProjects path
├── Config/                    # Default engine/input/game settings
├── Data/
│   └── data.json              # Initial cube spawn positions
└── Source/CursorCPP/
    ├── CoolSpawner.cpp/.h     # JSON load, spawn, animation, click-to-add
    └── CursorCPP.*            # Game module entry
```

## Getting started

### Open in Unreal Editor

1. Clone the repo (see **Clone notes** below if full clone fails).
2. Right-click `CursorCPP.uproject` → **Generate Visual Studio project files** (or open directly in UE 5.5).
3. Build and run the editor target.

### CLI build (Windows)

From a **Developer Command Prompt for VS** with UE 5.5 on your PATH:

```bat
"C:\Program Files\Epic Games\UE_5.5\Engine\Build\BatchFiles\Build.bat" ^
  CursorCPPEditor Win64 Development ^
  -project="C:\path\to\UE_CLI_Builder\CursorCPP.uproject"
```

After building, optionally sync artifacts:

```bat
sync-build-files.bat
```

That copies the local `build\` folder to `C:\UnrealProjects\UE_CLI_Builder`.

### Edit spawn data

Edit `Data/data.json` — each entry has `box`, `x`, `y`, `z` (Unreal units):

```json
{ "box": 1, "x": 0, "y": 0, "z": 0 }
```

Place an `ACoolSpawner` actor in your level (or use the default Open World template map).

## Clone notes

The git history includes large **Visual Studio `.vs/` cache files** (~60 MB pack), so a full clone can be slow or fail on flaky connections.

If `git clone` dies with `invalid index-pack output` or `tmp_pack_*` errors:

```bash
# Remove failed partial clone
rm -rf UE_CLI_Builder

# Shallow clone (enough for normal dev)
git clone --depth 1 git@github.com:BunnyGunGames/UE_CLI_Builder.git

# Optional: full history later
cd UE_CLI_Builder && git fetch --unshallow
```

Also check `df -h` for free disk space (need ~200 MB headroom) and retry on a stable network.

## License

MIT
