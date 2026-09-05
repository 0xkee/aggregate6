{ lib, stdenv, fetchurl }:

stdenv.mkDerivation rec {
  pname = "aggregate6";
  version = "0.3.0";

  src = fetchurl {
    url = "https://github.com/0xkee/aggregate6/releases/download/v${version}/${pname}-${version}.tar.gz";
    sha256 = "sha256-Nlhk8OvfWRa7HB9pIeon2RInxAwGioSWKdbW12+2FqQ=";
  };

  # Zero external dependencies — only libc
  buildInputs = [ ];
  nativeBuildInputs = [ ];

  makeFlags = [
    "PREFIX=${placeholder "out"}"
  ];

  doCheck = true;
  checkTarget = "test";

  meta = with lib; {
    description = "Fast multi-threaded IPv4/IPv6 CIDR prefix aggregation tool";
    longDescription = ''
      Reads bulk IPv4/IPv6 prefix lists from stdin or files, aggregates them
      using a multi-threaded Patricia trie, and outputs the minimal covering
      set. Written in C99 with zero external dependencies.
    '';
    homepage = "https://github.com/0xkee/aggregate6";
    license = licenses.gpl3Only;
    maintainers = [ ];
    platforms = platforms.unix;
    mainProgram = "aggregate6";
  };
}
