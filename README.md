Drawing-Machine-Firmware
========================

Here is the source code for the PIC184620 microcontroller. The firmware is stable and reliable. Circuit schematics, PCB layout and user manual will eventually be posted alongside the source code.

Raspberry Pi Configuration
========================
A Raspberry Pi model B+ with a 2014-06-20-wheezy-raspbian.img image was used.

1. To make use of the UART, two file must be modified.

a. Comment out the following line refering to ttyAMA0 in /etc/inittab with a leading #:

T0:23:respawn:/sbin/getty -L ttyAMA0 115200 vt100

b. delete entries refering to ttyAMAo in the /boot/cmdline file. After deleting, my file
consisted only of this line:

dwc_otg.lpm_enable=0 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline rootwait

2. The /etc/rc.local file includes only one of the two following commands:

/home/pi/vitrines/equitable-load-file | xargs /home/pi/vitrines/stream-block.sh

or

/home/pi/vitrines/equitable-load-file | xargs /home/pi/vitrines/stream2.sh

See the comments in the shells scripts themselves.

3. All other important programs reside in ~/vitrines



