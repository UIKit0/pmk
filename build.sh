#!/bin/sh

# Pretty make!
alias make=$(pwd)/tool/pretty_make.py

# Compile kernel
make -C kern/ all

# If build succeeded, continue
OUT=$?
if [ $OUT -eq 0 ];then
	echo ""
else
	exit
fi

echo "\n[3;32;40m***** Building RAM disk *****[0;37;49m"

# Create an the RAM disk
rm -rf ramdisk/.DS_Store
dot_clean -m ramdisk/
./tool/mkramdisk ramdisk

# Copy kernel, etc
echo "\n[3;32;40m***** Copying files *****[0;37;49m"

# Mount the disk image
hdiutil attach ./hdd.img

# Copy files
echo ""
cp -v kern/kernel_stripped.elf /Volumes/PMK/boot/kernel.elf
cp -v initrd.gz /Volumes/PMK/boot/initrd.gz

# Clean up some files
rm initrd.bin initrd.gz

# Clean up OS X's crap
rm -rf /Volumes/PMK/.fseventsd
rm -rf /Volumes/PMK/.Trashes

# Remove "dot underbar" resource forks
dot_clean -m /Volumes/PMK/

# Unmount the disk image
#hdiutil detach /Volumes/PMK/

# Determine which emulator to run
if [ "$1" == "qemu" ]; then
	echo "\n[3;32;40m***** Running QEMU *****[0;37;49m"
	qemu-system-i386 -hda hdd.img -m 256M -vga std -soundhw sb16 -net nic,model=e1000 -net user -cpu pentium3 -rtc base=utc -monitor stdio -s
elif [ "$1" == "bochs" ]; then
	echo "\n[3;32;40m***** Running Bochs *****[0;37;49m"
	bochs -f bochsrc.bxrc -q
elif [ "$1" == "vbox" ]; then
	echo "\n[3;32;40m***** Running VirtualBox *****[0;37;49m"
	VBoxManage controlvm PMK poweroff

	rm -rf hdd.vdi
	VBoxManage convertfromraw hdd.img hdd.vdi --format VHD
	VBoxManage internalcommands sethduuid hdd.vdi 51f1a393-1c6e-49dc-816f-f83c430f5fe5

	VBoxManage startvm PMR
fi
