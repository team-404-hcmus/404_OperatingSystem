VMDIR="VM/img"
OSNAME="404OS"
BUILDDIR="bin"
KERNELDIR="kernel"
#mv bootloader/$BUILDDIR/bootx64.efi VM/img/main.efi
if test ! -f "$VMDIR/$OSNAME.img"; then
printf "Create Image\n"
dd if=/dev/zero of="$VMDIR/$OSNAME.img" bs=512 count=93750
mformat -i "$VMDIR/$OSNAME.img" -f 1440 ::
fi
	printf "Create ::/EFI directory\n"
	mmd -o -i "$VMDIR/$OSNAME.img" ::/EFI
	printf "Create ::/EFI/BOOT directory\n"
	mmd -o -i "$VMDIR/$OSNAME.img" ::/EFI/BOOT
	printf "Create ::/404OS directory\n"
	mmd -o -i "$VMDIR/$OSNAME.img" ::/404OS
	printf "Copy bootloader/$BUILDDIR/bootx64.efi  => ::/EFI/BOOT\n"
	mcopy -o -i "$VMDIR/$OSNAME.img" "bootloader/$BUILDDIR/bootx64.efi" ::/EFI/BOOT
	printf "Copy $VMDIR/startup.nsh  => ::\n"
	mcopy -o -i "$VMDIR/$OSNAME.img" "$VMDIR/startup.nsh" ::
for filename in "$KERNELDIR/$BUILDDIR"/*; do
	printf "Copy $filename  => ::/404OS\n"
        mcopy -o -i "$VMDIR/$OSNAME.img" "$filename" ::404OS/
done