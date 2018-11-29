make all
sudo rmmod chrdrv
sudo insmod chrdrv.ko
#insert to udev to remove nod permission modification
sudo chmod 666 /dev/chrdrv
echo "Running user space poll app to wait until resource is ready."
echo "In another terminal write some data like \"\$echo test > /dev/chrdrv\""
./polling.o
