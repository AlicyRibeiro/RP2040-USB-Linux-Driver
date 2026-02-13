savedcmd_pico_usb_driver.mod := printf '%s\n'   pico_usb_driver.o | awk '!x[$$0]++ { print("./"$$0) }' > pico_usb_driver.mod
