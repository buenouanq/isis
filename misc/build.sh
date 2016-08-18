#!/bin/sh

if [[ $USER != "root" ]] 
then
echo 
echo + You can only run the Isis build program as root 
echo 
exit 
fi

. /usr/local/isis/misc/isis.sh
echo - Building Isis
cd ${ISIS_ROOT}/source
make clean install &> install.log

errors=`grep Error ${ISIS_ROOT}/source/install.log`

if [[ $errors ]]
then
echo
echo \* There were errors while building Isis
echo \* Please inspect ${ISIS_ROOT}/source/install.log
echo \* Verify you have properly installed the prerequisite software
echo \* Check the Isis web site Linux info for pointers
echo \* Run /usr/local/isis/misc/build.sh to attempt another build
echo
else
echo - Isis build successful
fi
