Name: keepass
Summary: KeePassX Cross Platform Password Manager
Version: 0.2.0
Release: 1
License: GPL
Group: Security
Source: keepass-0.2.0.tar.gz


BuildRoot: /home/tarek/Desktop/KeePassX-RPM-Build
Packager: Tarek Saidi
Distribution: KeePassX
Prefix: /usr/local
Url: http://keepass.berlios.de

Vendor: Tarek Saidi


%description
KeePassX is a free/open-source password manager or safe which helps you to manage your passwords in a secure way. You can put all your passwords in one database, which is locked with one master key or a key-disk. So you only have to remember one single master password or insert the key-disk to unlock the whole database. The databases are encrypted using the best and most secure encryption algorithms currently known (AES and Twofish).

%prep
rm -rf $RPM_BUILD_ROOT 
mkdir $RPM_BUILD_ROOT

%setup -q

%build
qmake PREFIX=$RPM_BUILD_ROOT%{prefix}
make

%install
make install

cd $RPM_BUILD_ROOT

find . -type d -fprint $RPM_BUILD_DIR/file.list.%{name}.dirs
find . -type f -fprint $RPM_BUILD_DIR/file.list.%{name}.files.tmp
sed '/\/man\//s/$/.gz/g' $RPM_BUILD_DIR/file.list.%{name}.files.tmp > $RPM_BUILD_DIR/file.list.%{name}.files
find . -type l -fprint $RPM_BUILD_DIR/file.list.%{name}.libs
sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' $RPM_BUILD_DIR/file.list.%{name}.dirs > $RPM_BUILD_DIR/file.list.%{name}
sed 's,^\.,\%attr(-\,root\,root) ,' $RPM_BUILD_DIR/file.list.%{name}.files >> $RPM_BUILD_DIR/file.list.%{name}
sed 's,^\.,\%attr(-\,root\,root) ,' $RPM_BUILD_DIR/file.list.%{name}.libs >> $RPM_BUILD_DIR/file.list.%{name}

%clean
rm -rf $RPM_BUILD_ROOT
rm -rf $RPM_BUILD_DIR/file.list.%{name}
rm -rf $RPM_BUILD_DIR/file.list.%{name}.libs
rm -rf $RPM_BUILD_DIR/file.list.%{name}.files
rm -rf $RPM_BUILD_DIR/file.list.%{name}.files.tmp
rm -rf $RPM_BUILD_DIR/file.list.%{name}.dirs

%files -f ../file.list.%{name}

%defattr(-,root,root,0755)
