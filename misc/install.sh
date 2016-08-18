#!/bin/sh

if [[ $USER != "root" ]] 
then
echo 
echo + You can only run the Isis install program as root 
echo 
exit 
fi

if [[ -d "/usr/local/oldisis" ]] 
then
echo 
echo + Please rm -rf /usr/local/oldisis 
echo + before running the Isis install program. 
echo + Note this folder contains backups from your previous update 
echo + of Isis that you may need to save before proceeding. 
echo 
exit 
fi

echo
echo - Welcome to the Isis install/update program
cd /usr/local

if [[ -d "/usr/local/isis" ]] 
then
echo - Moving /usr/local/isis to /usr/local/oldisis 
mv isis oldisis 
fi

if [[ -e "isis.tgz" ]] 
then
echo - Removing old Isis distribution archive file
rm -f isis.tgz
fi

echo - Downloading latest Isis distribution into /usr/local
wget --dot-style=binary http://www.medialabeurope.org/isis/downloads/isis.tgz

if [[ ! -e "isis.tgz" ]] 
then
echo + There was a problem downloading the Isis distribution.
echo + Please check that your network connection is functioning properly.
exit 
fi

echo - Unpacking the new Isis distribution
tar -zxf isis.tgz

echo - Removing archive file
rm -f isis.tgz

profdone=`grep ISIS /etc/profile`

if [[ $profdone ]]
then
echo - Isis definitions already exist in the global environment
else
echo - Adding Isis to the global environment for all users
cd /usr/local/isis/misc
cat isis.sh >> /etc/profile
cat isis.sh >> /etc/bash.bashrc
cat isis.csh >> /etc/csh.cshrc
cat isis.emacs >> /etc/emacs/site-start.d/50isis.el
fi

/usr/local/isis/misc/build.sh

echo 
echo - Your old installation is backed up at /usr/local/oldisis
echo - Please rm -rf /usr/local/oldisis if it is not needed.
echo


