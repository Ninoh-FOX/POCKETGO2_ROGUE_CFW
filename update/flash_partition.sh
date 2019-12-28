#!/bin/sh

NEW_START=32768
NEW_END=819200
NEW_SIZE=$((${NEW_END} - ${NEW_START}))

SYSTEM_MOUNTPOINT=/mnt/_system_update

SYSTEM_IMAGE=${SYSTEM_MOUNTPOINT}/system.bin
KERNEL_DEST=${SYSTEM_MOUNTPOINT}/fs/vmlinuz.bin
ROOTFS_DEST=${SYSTEM_MOUNTPOINT}/fs/rootfs.squashfs
MODULES_FS_DEST=${SYSTEM_MOUNTPOINT}/fs/modules.squashfs

SYSTEM_DEVICE=/dev/mmcblk0
SYSTEM_PART_NUM=1

error_quit() {
	umount ${SYSTEM_MOUNTPOINT}/fs
	umount ${SYSTEM_MOUNTPOINT}
	rm ${SYSTEM_IMAGE}
	exit 1
}

if [ ! -f "$KERNEL" -o ! -f "$ROOTFS" -o ! -f "$MODULES_FS" \
	-o ! -f "$BOOTLOADER" -o ! -f "${KERNEL}.sha1" -o ! -f "${ROOTFS}.sha1" \
	-o ! -f "${MODULES_FS}.sha1" -o ! -f "${BOOTLOADER}.sha1" ] ; then
	DIALOGRC="/tmp/dialog_err.rc" \
		dialog --msgbox 'ERROR!\n\nUnable to generate image: required files are missing.' 9 34
	error_quit
fi

# Create a 400 MiB tmpfs. It won't be a problem even if the available
# amount of RAM is < 400 MiB.
mount none -t tmpfs -o size=400M "$SYSTEM_MOUNTPOINT"

# Create a ~386 MiB sparse file, format it to FAT32
dd if=/dev/zero of="${SYSTEM_MOUNTPOINT}/system.bin" \
	bs=1b count=0 seek=${NEW_SIZE} >/dev/null 2>&1
mkfs.vfat -F32 "${SYSTEM_MOUNTPOINT}/system.bin"

# Mount it, and copy the OS inside
mkdir "${SYSTEM_MOUNTPOINT}/fs"
mount -o loop "${SYSTEM_MOUNTPOINT}/system.bin" "${SYSTEM_MOUNTPOINT}/fs"

if [ "$BAR" ] ; then
	echo 'Adding kernel to flash image...'
	$BAR -w 54 -0 ' ' -n -o "$KERNEL_DEST" "$KERNEL"

	echo 'Adding root filesystem to flash image...'
	$BAR -w 54 -0 ' ' -n -o "$ROOTFS_DEST" "$ROOTFS"

	echo 'Adding modules filesystem to flash image...'
	$BAR -w 54 -0 ' ' -n -o "$MODULES_FS_DEST" "$MODULES_FS"
else
	echo 'Adding kernel to flash image...'
	cp "$KERNEL" "$KERNEL_DEST"

	echo 'Adding root filesystem to flash image...'
	cp "$ROOTFS" "$ROOTFS_DEST"

	echo 'Adding modules filesystem to flash image...'
	cp "$MODULES_FS" "$MODULES_FS_DEST"
fi

cp "${KERNEL}.sha1" "${KERNEL_DEST}.sha1"
cp "${ROOTFS}.sha1" "${ROOTFS_DEST}.sha1"
cp "${MODULES_FS}.sha1" "${MODULES_FS_DEST}.sha1"

echo ''
echo 'Verifying checksum of kernel...'
if [ "$BAR" ] ; then
	SHA1=`$BAR -w 54 -0 ' ' -n "$KERNEL_DEST" | sha1sum | cut -d' ' -f1`
else
	SHA1=`sha1sum "$KERNEL_DEST" | cut -d' ' -f1`
fi

if [ "$SHA1" != "`cat ${KERNEL_DEST}.sha1`" ] ; then
	DIALOGRC="/tmp/dialog_err.rc" \
		dialog --msgbox 'ERROR!\n\nUpdated kernel is corrupted!' 9 34
	error_quit
fi

echo 'Verifying checksum of root filesystem...'
if [ "$BAR" ] ; then
	SHA1=`$BAR -w 54 -0 ' ' -n "$ROOTFS_DEST" | sha1sum | cut -d' ' -f1`
else
	SHA1=`sha1sum "$ROOTFS_DEST" | cut -d' ' -f1`
fi

if [ "$SHA1" != "`cat ${ROOTFS_DEST}.sha1`" ] ; then
	DIALOGRC="/tmp/dialog_err.rc" \
		dialog --msgbox 'ERROR!\n\nUpdated RootFS is corrupted!' 9 34
	error_quit
fi

echo 'Verifying checksum of modules filesystem...'
if [ "$BAR" ] ; then
	SHA1=`$BAR -w 54 -0 ' ' -n "$MODULES_FS_DEST" | sha1sum | cut -d' ' -f1`
else
	SHA1=`sha1sum "$MODULES_FS_DEST" | cut -d' ' -f1`
fi

if [ "$SHA1" != "`cat ${MODULES_FS_DEST}.sha1`" ] ; then
	DIALOGRC="/tmp/dialog_err.rc" \
		dialog --msgbox 'ERROR!\n\nUpdated modules filesystem is corrupted!' 9 34
	error_quit
fi

echo 'Verifying checksum of bootloader...'
if [ "$BAR" ] ; then
	SHA1=`$BAR -w 54 -0 ' ' -n "$BOOTLOADER" | sha1sum | cut -d' ' -f1`
else
	SHA1=`sha1sum "$BOOTLOADER" | cut -d' ' -f1`
fi

if [ "$SHA1" != "`cat ${BOOTLOADER}.sha1`" ] ; then
	DIALOGRC="/tmp/dialog_err.rc" \
		dialog --msgbox 'ERROR!\n\nBootloader is corrupted!' 9 34
	error_quit
fi

umount "${SYSTEM_MOUNTPOINT}/fs"

# Shrink the image
echo ''
python trimfat.py "${SYSTEM_IMAGE}"

echo ''
echo 'Calculating checksum of flash image...'
if [ "$BAR" ] ; then
	IMAGE_SHA1=`$BAR -w 54 -0 ' ' -n "${SYSTEM_IMAGE}" | sha1sum | cut -d' ' -f1`
else
	IMAGE_SHA1=`sha1sum "${SYSTEM_IMAGE}" | cut -d' ' -f1`
fi

# Flash the image!
echo 'Flashing the new system partition...'
if [ "$BAR" ] ; then
	$BAR -w 54 -0 ' ' -n "${SYSTEM_IMAGE}" | dd of=${SYSTEM_DEVICE} \
		bs=1b seek=${NEW_START} conv=notrunc 2>/dev/null
else
	dd if="${SYSTEM_IMAGE}" of=${SYSTEM_DEVICE} \
		bs=1b seek=${NEW_START} conv=notrunc 2>/dev/null
fi

echo 'Flushing write cache... '
sync
echo 3 > /proc/sys/vm/drop_caches

echo ''
echo 'Verifying checksum of flashed partition...'
SIZE_BYTES=`ls -l ${SYSTEM_IMAGE} | awk '{print $5}'`
if [ "$BAR" ] ; then
	SHA1=`head -c $((${NEW_START} * 512 + ${SIZE_BYTES})) ${SYSTEM_DEVICE} \
		| tail -c +$((${NEW_START} * 512 + 1)) \
		| $BAR -w 54 -0 ' ' -n -b 512 -s ${SIZE_BYTES} \
		| sha1sum | cut -d' ' -f1 `
else
	SHA1=`head -c $((${NEW_START} * 512 + ${SIZE_BYTES})) ${SYSTEM_DEVICE} \
		| tail -c +$((${NEW_START} * 512 + 1)) \
		| sha1sum | cut -d' ' -f1 `
fi

if [ "$SHA1" != "$IMAGE_SHA1" ] ; then
	DIALOGRC="/tmp/dialog_err.rc" \
		dialog --msgbox 'ERROR!\n\nFlashed image is corrupted!' 9 34
	echo "SHA1: $SHA1"
	echo "IMAGE_SHA1: $IMAGE_SHA1"
	error_quit
fi

echo 'Writing new partition table...'
echo ${NEW_START},${NEW_SIZE} | sfdisk --no-reread -uS -N \
	${SYSTEM_PART_NUM} ${SYSTEM_DEVICE} >/dev/null 2>&1
sync

echo 'Writing new bootloader...'
dd if="$BOOTLOADER" of=${SYSTEM_DEVICE} bs=512 seek=1 \
	count=16 conv=notrunc 2>/dev/null
sync

dialog --msgbox 'Update complete!\nThe system will now restart.\n\n
If for some reason the system fails to boot, try to press the
following keys while powering on the device:\n
    -X to boot the last working kernel,\n
    -Y to boot the last working rootfs.\n\n
Note that pressing both keys during the power-on sequence will load the very
same Operating System (kernel + rootfs) you had before upgrading.' 16 0
reboot
