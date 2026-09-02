{ lib, stdenv, fetchFromGitHub }:

stdenv.mkDerivation rec {
  pname = "aggregate6";
  version = "0.1.0";

  src = fetchFromGitHub {
    owner = "0xkee";
    repo = "aggregate6";
    rev = "v${version}";
    sha256 = lib.fakeSha256;
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
