# POCKETGO2 ROGUE CFW

Oficial firmware for Pocketgo 2

<p align="center"><img class="center" src =https://raw.githubusercontent.com/Ninoh-FOX/POCKETGO2_ROGUE_CFW/master/logos/logo_gcwzero_clut224.png></p>

# update firmware 1.0.1: <br>
https://github.com/Ninoh-FOX/POCKETGO2-ROGUE-CFW/releases/tag/1.0,1

### 1.0.1:<br>

1. Updated the file system, partition and expansion scripts again.
2. Gmenu2x analog stick control is removed
3. Adjusted the battery.
4. Update of the Stock Clock application with a new redesign. thanks to Rafa Vico (https://github.com/RafaVico)
5. Text editor, corrected opk buttons. thanks to Rafa Vico (https://github.com/RafaVico)
6. Added the hardware tester POCKETGO 2. thanks to Rafa Vico (https://github.com/RafaVico)

# features:

1. Support to read the second sdcard in fat32, exFAT, ntfs, ext4 (recommended) and ext3 format.

2. Added key combinations with the power button:
+ POWER + VOL + or VOL-: Adjust the brightness of the screen.
+ POWER + SELECT: Close the current application.
+ POWER + START: Restart the console.
+ POWER + B: The analog stick will work as DPAD.
+ POWER + A: Change the aspect ratio with the screen in Hardware mode.
+ POWER + R1: Mouse emulation (Stick is movement and the L2 and R2 buttons the buttons)
+ POWER + DPAD up / dowm: Adjust the sharpness.
+ POWER + X: Take a screenshot.
    
3. In Gmenu2x the power button can turn the screen off or on.

4. You can change the cpu to maximum or minimum in gmenu2x for the opk, the same to change the name, description, icon and file filter.

# Installation:

For a correct installation, the first time it is advisable to flash the sd_image in an sdcard with Win32DiskImager for example, too you need format the sdcard two times with SD Formatter before of flasher the sdcard.

https://sourceforge.net/projects/win32diskimager/

https://www.sdcard.org/downloads/formatter/

You can also use flasher.opk for the same, but it takes much longer and being a console that has the SD1 (TF1) of the system at hand because I do not see it really necessary, even so, it is in the release. If you would like to use it, it is advisable to put it in the apps folder of the SD2 (TF2).

For future updates you will only need to use the update.opk, which is used just like any application. You can use it in both the apps folder of SD1 (TF1) or SD2 (TF2).
