echo "*******"
env | grep GFE
echo "Building BGFG from :" $GFESOURCEROOT " To: " $GFEBUILDROOT

rm -rf $GFEBUILDROOT/include/pk
mkdir -p $GFEBUILDROOT/include/pk
cp  $GFESOURCEROOT/bgfe/pk/*/include/*.hpp $GFEBUILDROOT/include/pk/

rm -rf $GFEBUILDROOT/include/fft3d
mkdir -p $GFEBUILDROOT/include/fft3d
cp  $GFESOURCEROOT/bgfe/fft3d/include/*.hpp $GFEBUILDROOT/include/fft3d/

rm -rf $GFEBUILDROOT/include/shmem
mkdir -p $GFEBUILDROOT/include/shmem
cp  $GFESOURCEROOT/bgfe/shmem/include/*.hpp $GFEBUILDROOT/include/shmem/

rm -rf $GFEBUILDROOT/include/PhasedPipeline
mkdir -p $GFEBUILDROOT/include/PhasedPipeline
cp  $GFESOURCEROOT/bgfe/PhasedPipeline/include/*.hpp $GFEBUILDROOT/include/PhasedPipeline/

echo "*******"


