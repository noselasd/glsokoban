%define DATADIR %{_prefix}/games/glsokoban-data/
Name:           glsokoban
Version:        0.0.1
Release:        0.fdr.1
Epoch:          0
Summary:        OpenGL based Sokoban game

Group:          Amusements/Games
License:        Public Domain
URL:            http://asgaard.homelinux.org/svn/glsokoban/
Source0:        glsokoban-0.0.1.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  /usr/include/GL/gl.h,SDL_image-devel,SDL-devel,freetype-devel

%description
The all famous sokoban. Get more levels from 
http://www.sourcecode.se/sokoban/

%prep
%setup -q -n glsokoban


%build

make CFLAGS+=-DMAINDIR=\\\"%DATADIR\\\"

%post
if [ -f /usr/bin/update-desktop-database ] ; then
	/usr/bin/update-desktop-database /usr/share/applications
fi

%postun
if [ -f /usr/bin/update-desktop-database ] ; then
        /usr/bin/update-desktop-database /usr/share/applications
fi

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/games/glsokoban-data/levels
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/games/glsokoban-data/textures
cp glsokoban $RPM_BUILD_ROOT/%{_prefix}/games
cp -a levels/*slc  $RPM_BUILD_ROOT/%{_prefix}/games/glsokoban-data/levels/
cp -a textures/*bmp $RPM_BUILD_ROOT/%{_prefix}/games/glsokoban-data/textures/
mkdir -p $RPM_BUILD_ROOT/%{_datadir}/applications/
cp fedora-glsokoban.desktop  $RPM_BUILD_ROOT/%{_datadir}/applications/fedora-glsokoban.desktop
cp LucidiaSR.font $RPM_BUILD_ROOT/%{_prefix}/games/glsokoban-data/

%clean
rm -rf $RPM_BUILD_ROOT

%files 
%doc README
%defattr(-,root,root,-)
%{_prefix}/games/*
%{_datadir}/applications/*
