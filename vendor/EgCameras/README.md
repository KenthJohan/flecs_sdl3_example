##




## slcan

### Check for device name
```
sudo dmesg
```

### Check for CAN device permission
```
ls -l /dev/ttyACM*
```

### Change permission
```
sudo chmod a+rw /dev/ttyACM0
```

### Setup CAN device
Just run slcand with the proper arguments for your bus speed, and a new CAN device should show up on your system. Don't forget to bring the interface up with ifconfig after running slcand! Now you can use any of the standard Linux CAN utilities to interact with the bus. Make sure that you specify the right TTY port, which you can check with the dmesg command after plugging in your CANable.


```
sudo slcand -o -c -s0 /dev/ttyACM0 can0
```




### can-utils
The CANable provides a socketCAN-compatible interface that can be brought up with slcand. This allows you to use all standard Linux CAN utilities like candump, cansniffer, and even wireshark. Bus speed is specified with the "-s" parameter where:

    -s0 = 10k
    -s1 = 20k
    -s2 = 50k
    -s3 = 100k
    -s4 = 125k
    -s5 = 250k
    -s6 = 500k
    -s7 = 750k
    -s8 = 1M

```bash

sudo ifconfig can0 up
sudo ifconfig can0 txqueuelen 1000
cansend can0 999#DEADBEEF   # Send a frame to 0x999 with payload 0xdeadbeef
candump can0                # Show all traffic received by can0
canbusload can0 500000      # Calculate bus loading percentage on can0 
cansniffer can0             # Display top-style view of can traffic
cangen can0 -D 11223344DEADBEEF -L 8    # Generate fixed-data CAN messages
```




### Interfaces
```bash
ip -details link show can5
```









## candlelight
https://github.com/candle-usb/candleLight_fw
https://canable.io/getting-started.html#cantactapp-windows

```bash
sudo code /etc/udev/rules.d/99-candlelight.rules
```

#### 99-candlelight.rules
```
SUBSYSTEM=="net", ATTRS{idVendor}=="1d50", ATTRS{idProduct}=="606f", ATTRS{serial}=="0047005F4D4D501320383834", NAME="can5"
```

### Automatically configure can bus on 
https://github.com/woj76/gs_usb_leonardo/blob/master/80-can.network

Put this file in: `/etc/systemd/network/` and then run `$ sudo systemctl enable systemd-networkd` to enable `systemd-networkd`
Now start `systemd-networkd`: `sudo systemctl start systemd-networkd`
Credit: https://github.com/linux-can/can-utils/issues/68#issuecomment-584505426
#### /etc/systemd/network/80-can.network
```
# For Ubuntu 20.04

[Match]
Name=can*

[CAN]
BitRate=500000

# Put this file in: "/etc/systemd/network/" and then run "sudo systemctl enable systemd-networkd" to enable systemd-networkd
# Now start systemd-networkd: "sudo systemctl start systemd-networkd"
# Credit: https://github.com/linux-can/can-utils/issues/68#issuecomment-584505426
```



### Enable can interface
```bash
sudo ip link set can5 up type can bitrate 500000
```



## Code
https://github.com/linux-can/can-utils/blob/master/candump.c







## command to determine ports of a device (like /dev/ttyUSB0)

Below is a quick and dirty script which walks through devices in /sys looking for USB devices with a ID_SERIAL attribute. Typically only real USB devices will have this attribute, and so we can filter with it. If we don't, you'll see a lot of things in the list that aren't physical devices.
https://unix.stackexchange.com/questions/144029/command-to-determine-ports-of-a-device-like-dev-ttyusb0

```bash
cd /usr/local/bin/
sudo touch lsusbdev
sudo chmod +x lsusbdev
sudo nano lsusbdev
```

```bash
#!/bin/bash

for sysdevpath in $(find /sys/bus/usb/devices/usb*/ -name dev); do
    (
        syspath="${sysdevpath%/dev}"
        devname="$(udevadm info -q name -p $syspath)"
        [[ "$devname" == "bus/"* ]] && exit
        eval "$(udevadm info -q property --export -p $syspath)"
        [[ -z "$ID_SERIAL" ]] && exit
        echo "/dev/$devname - $ID_SERIAL"
    )
done

```


## References
* https://www.youtube.com/watch?v=GdnKQclBvP4
* https://github.com/makerbase-motor/MKS-SERVO57D/blob/master/Example%20Code/Arduino%20Example%20Code/arduino_11_Serial_Speed%20Mode/arduino_11_speedMode/arduino_11_speedMode.ino
* https://www.kernel.org/doc/html/next/networking/can.html



## Issues
https://www.aliexpress.com/item/1005005455241016.html
* CANable V2.0 Pro S<br>
candlelight firmware does not work. It hangs after sending 3 CAN frames.