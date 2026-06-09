{
  description = "OpenGL C Project Environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }: let
    supportedSystems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
    forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
  in {
    devShells = forAllSystems (system: let
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      default = pkgs.mkShell {
        packages = with pkgs; [
          clang
          cmake
          gnumake
          pkg-config
          glfw
          cglm
        ] ++ (if pkgs.stdenv.isLinux then [
          libGL
        ] else if pkgs.stdenv.isDarwin then [
        ] else []);
      };
    });
  };
}