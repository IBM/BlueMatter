#! /bin/ksh

export subdir="$1";
export comdir="com/ibm/bluematter/";

echo $subdir;
echo $comdir;
rm -f $comdir/$subdir/*.class;
javac -O $comdir/$subdir/*.java;
jar cvf $subdir.jar $comdir/$subdir/*.class;
mv $subdir.jar ~/ 
