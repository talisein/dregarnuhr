Name:           dregarnuhr
Version:        1.0.15
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
BuildRequires:  libpng-devel
BuildRequires:  libjpeg-turbo-devel
BuildRequires:  miniz-devel
BuildRequires:  outcome-devel
BuildRequires:  libxml++50-devel >= 5.0.2
BuildRequires:  boost-ext-ut-static
BuildRequires:  ctre-static
BuildRequires:  libmagicenum-static


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
* Wed Jun 7 2023 Andrew Potter <agpotter@gmail.com> - v1.0.15-1
- Add M3V1

* Tue May 30 2023 Andrew Potter <agpotter@gmail.com> - v1.0.14-1
- Add P5V4

* Sun Apr 23 2023 Andrew Potter <agpotter@gmail.com> - v1.0.13-1
- Add UFTSS1

* Sun Apr 23 2023 Andrew Potter <agpotter@gmail.com> - v1.0.12-1
- Add SSC1

* Wed Apr 12 2023 Andrew Potter <agpotter@gmail.com> - v1.0.11-1
- Fix missing slug bug

* Tue Apr 4 2023 Andrew Potter <agpotter@gmail.com> - v1.0.10-1
- Add M2V7 and P5V3

* Thu Jan 26 2023 Andrew Potter <agpotter@gmail.com> - v1.0.9-1
- P5V2

* Fri Dec 30 2022 Andrew Potter <agpotter@gmail.com> - v1.0.8-1
- Fix 1.0.7 bug that omits P4V8 from omnibus
- Move P3V5 'And So, the Future' to after 'Meanwhile at the Temple'
- Allow omnibus to be created without volume 1
- Consoliate bonus manga in TOC

* Wed Nov 30 2022 Andrew Potter <agpotter@gmail.com> - v1.0.7-1
- Nested table of contents

* Wed Nov 30 2022 Andrew Potter <agpotter@gmail.com> - v1.0.6-1
- Fix maps

* Wed Nov 30 2022 Andrew Potter <agpotter@gmail.com> - v1.0.5-1
- Add FB3 and P5V1

* Mon Oct 31 2022 Andrew Potter <agpotter@gmail.com> - v1.0.4-1
- Add P4V9 and M2V6

* Wed Aug 10 2022 Andrew Potter <agpotter@gmail.com> - v1.0.0-1
- Initial version of package
