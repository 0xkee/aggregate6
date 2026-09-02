{ lib, stdenv, fetchurl }:

stdenv.mkDerivation rec {
  pname = "aggregate6";
  version = "0.2.1";

  src = fetchurl {
    url = "https://github.com/0xkee/aggregate6/releases/download/v${version}/${pname}-${version}.tar.gz";
    sha256 = "sha256-M9zDAn5KZSWuV8TsIYB5SwArgace8DUVd2zPbaqhtPM=";
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
    description = "Fast CIDR prefix aggregation tool";
    longDescription = ''
      Reads IPv4/IPv6 prefixes from stdin, aggregates them using a Patricia trie,
      and outputs the minimal covering set. Written in C99 with zero external
      dependencies.
    '';
    homepage = "https://github.com/0xkee/aggregate6";
    license = licenses.gpl3Plus;
    maintainers = [ ];
    platforms = platforms.unix;
    mainProgram = "aggregate6";
  };
}
