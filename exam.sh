#!/bin/bash
mkdir mydir
chmod a+rwx mydir
touch myfile
echo 2023 > myfile
mv moveme mydir/
cp copyme mydir/copied
cat readme
gcc bad.c 2> err.txt
mkdir gen
cd gen/
a=1
n=10
if [ $# -eq 1 ]
then
    n=$1
fi
while [ $a -le $n ]
do
    touch "$a.txt"
    let a++
done

