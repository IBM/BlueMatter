echo "*******"
env | grep GFE
echo "Building BlueMatter from :" $GFESOURCEROOT " To: " $GFEBUILDROOT
rm -rf $GFEBUILDROOT/include/BlueMatter;
mkdir -p $GFEBUILDROOT/include/BlueMatter;
cp $GFESOURCEROOT/BlueMatter/*/include/*.hpp $GFEBUILDROOT/include/BlueMatter/
cp $GFESOURCEROOT/BlueMatter/*/include/*.h   $GFEBUILDROOT/include/BlueMatter/
#cp */include/*.hpp $GFEBUILDROOT/include/BlueMatter/
#cp */include/*.h   $GFEBUILDROOT/include/BlueMatter/
echo "*******"

