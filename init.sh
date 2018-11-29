sudo rmmod chrdrv
sudo insmod chrdrv.ko
#insert to udev to remove nod permission modif
sudo chmod 666 /dev/chrdrv
