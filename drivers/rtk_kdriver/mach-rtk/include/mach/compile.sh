# step1 : jump to target folder
all:
	cd $1/drivers/rtk_kdriver/

# step2 : git hash
	echo \#define RTK_KDRIVER_GIT_VERSION \"rtk_kdriver_git_version=`git describe --always`\" > ./mach-rtk/include/mach/compile.h;
	echo \#define RTK_LINUX_GIT_VERSION \"rtk_linux_git_version=`cd ../../ && git describe --always`\" >> ./mach-rtk/include/mach/compile.h;
	echo \#define RTK_COMMON_GIT_VERSION \"rtk_common_git_version=`cd common && git describe --always`\" >> ./mach-rtk/include/mach/compile.h;
	cd -;