#!/usr/bin/env bash
set -e
script=$(readlink -f "$0")
route=$(dirname "$script")

pkg_name=dispatch
target_version=$1
target_install_prefix=$2
working_dir=install
pkg_date=$(date "+%Y%m%d%H%M%S")
echo "target version: ${target_version}"
if [  "${target_version}"x = ""x ]; then
    target_version=$(cat ${route}/../version)
    echo "No version number is provided, so we use value in 'proj_root/version' as the target version: ${target_version}"
  else
   echo ${target_version} > ${route}/../version
   echo "Version is ${target_version} and it has been written into 'proj_root/version'"
fi
if [ "${target_install_prefix}" == "" ]; then
    target_install_prefix=/usr/local
fi
echo "deb install prefix is ${target_install_prefix}"

mkdir -p ${route}/../${working_dir}
mkdir -p ${route}/../${working_dir}/DEBIAN
mkdir -p ${route}/../${working_dir}${target_install_prefix}

project_install_path=${route}/../${working_dir}${target_install_prefix}

chmod 775 ${route}/../${working_dir}/DEBIAN
cd ${route}/../${working_dir}/DEBIAN
touch control
chmod 775 control
(cat << EOF
Package: dispatch
Version: ${target_version}
Section: x11
Priority: optional
Depends:
Suggests:
Architecture: amd64
Maintainer: oyoung
CopyRight: commercial Provider: oyoung.
Description: Modern C++ Dispatch Tool
EOF
) > control

touch preinst
chmod 775 preinst
(cat << EOF
#!/bin/bash
rm -rf ${target_install_prefix}
EOF
) > preinst

touch postinst
chmod 775 postinst
(cat << EOF
#!/bin/bash
session_user=\`echo \${SUDO_USER:-\$USER}\`
uid=\`id -un \${session_user}\`
gid=\`id -gn \${session_user}\`
EOF
) > postinst

touch prerm
chmod 775 prerm

(cat << EOF
#!/bin/bash
EOF
) > prerm

touch postrm
chmod 775 postrm
(cat << EOF
#!/bin/bash
EOF
) > postrm

mkdir -p ${route}/../build
cd ${route}/../build

cmake -DCMAKE_INSTALL_PREFIX=${project_install_path}  ..

make install

### make package
cd ${route}/..

dpkg -b ${working_dir} dispatch_${target_version}_amd64.deb || exit 7
echo "pack dispatch into deb finished."

rm -rf ${route}/../${working_dir}
