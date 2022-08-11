Name:           dregarnuhr
Version:        1.0.0
Release:        1
Summary:        Rearrange Ascendence of a Bookworm epubs into Chronological Order

License:        GPLv3+
URL:            https://github.com/talisein/dregarnuhr
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc-c++
BuildRequires:  meson
BuildRequires:  ninja-build
BuildRequires:  cmake
BuildRequires:  openssl-devel
BuildRequires:  brotli-devel
BuildRequires:  zlib-devel
BuildRequires:  cpp-httplib-devel
BuildRequires:  date-devel
BuildRequires:  libjpeg-turbo-devel
BuildRequires:  miniz-devel
BuildRequires:  outcome-devel
BuildRequires:  libxml++50-devel


%description
Rearrange Ascendence of a Bookworm epubs into Chronological Order

%prep
%autosetup

%build
%meson
%meson_build

%install
%meson_install

%check
%meson_test

%files
%{_bindir}/dregarnuhr

%license LICENSE
%doc README.md



%changelog
* Wed Aug 10 2022 Andrew Potter <agpotter@gmail.com> - v1.0.0-1
- Initial version of package
