{
  description = "aggregate6 - Fast multi-threaded IPv4/IPv6 CIDR prefix aggregation tool";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    let
      overlay = final: prev: {
        aggregate6 = final.callPackage ./default.nix { };
      };
    in
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [ overlay ];
        };
      in {
        packages = {
          default = pkgs.aggregate6;
          aggregate6 = pkgs.aggregate6;
        };
      }
    ) // {
      overlays.default = overlay;
    };
}
