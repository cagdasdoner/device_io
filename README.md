# device_io
device driver poll example

# Run
$ source run.sh

After script execution, a user space C program will be polling the device driver.
Need to provide data from console to break the poll like :
echo pollbreaker > /dev/chrdrv

