lineTracker	
First tests of a lineTracker for the Money Never Sleeps (MNS) project 

Get the device id  
lsusb 
lsusb -s 003:005 -v | egrep "Width|Height"
v4l2-ctl -d 1 -c brightness=15
v4l2-ctl -d 1 -c contrast=150
muimota@muimota-HP-ENVY-15-Notebook-PC:~$ v4l2-ctl -d 1 -c contrast=160

