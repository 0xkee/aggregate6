Name:           aggregate6
Version:        0.1.0
Release:        1%{?dist}
Summary:        Fast CIDR prefix aggregation tool

License:        GPL-3.0-or-later
URL:            https://github.com/0xkee/aggregate6
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc
BuildRequires:  make

%description
Reads IPv4/IPv6 prefixes from stdin, aggregates them,
outputs minimal covering set. Written in C99 with zero
external dependencies.

%prep
%setup -q

%build
make %{?_smp_mflags} CC=gcc CFLAGS="%{optflags}"

%install
make install PREFIX=/usr DESTDIR=%{buildroot}

%check
make test

%files
%license LICENSE
%{_bindir}/aggregate6

%changelog
* Tue Sep 02 2025 0xkee <0xkee@users.noreply.github.com> - 0.1.0-1
- Initial release
