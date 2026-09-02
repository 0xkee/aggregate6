# NixOS / Nix Flake — aggregate6

Nix derivation and flake for installing aggregate6 on NixOS or any system
with the Nix package manager.

## Using as a Flake

### One-shot run

```bash
nix run github:0xkee/aggregate6?dir=dist/nix
```

### Install to profile

```bash
nix profile install github:0xkee/aggregate6?dir=dist/nix
```

### Add to your flake.nix

```nix
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    aggregate6.url = "github:0xkee/aggregate6?dir=dist/nix";
  };

  outputs = { self, nixpkgs, aggregate6, ... }: {
    # Use the overlay
    nixosConfigurations.myhost = nixpkgs.lib.nixosSystem {
      modules = [
        {
          nixpkgs.overlays = [ aggregate6.overlays.default ];
          environment.systemPackages = [ pkgs.aggregate6 ];
        }
      ];
    };
  };
}
```

## Using as an Overlay

In `configuration.nix` (without flakes):

```nix
nixpkgs.overlays = [
  (import (builtins.fetchGit {
    url = "https://github.com/0xkee/aggregate6";
    ref = "master";
  } + "/dist/nix")).overlays.default
];
```

Then add to `environment.systemPackages`:

```nix
environment.systemPackages = [ pkgs.aggregate6 ];
```

## Standalone nix-build

```bash
nix-build dist/nix/default.nix
./result/bin/aggregate6 --help
```

## Development shell

```bash
nix develop github:0xkee/aggregate6?dir=dist/nix
```

## Files

| File | Description |
|------|-------------|
| `default.nix` | Standalone derivation (usable with and without flakes) |
| `flake.nix` | Nix flake with overlay support |
