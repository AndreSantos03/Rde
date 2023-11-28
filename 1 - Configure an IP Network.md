#Configure an IP Network

1. Connect E0 from Tux43 and Tux44 to the *switch* port.
2. Switchig between the PC number 3 and 4, we run in bash:
```bash
ifconfig eth0 up
```

Given the different IPs for each computer, for TUX43 we run:
```bash
ifconfig eth0 172.16.40.1/24
```
And for Tux44 we run:
```bash
ifconfig eth0 172.16.40.254/24
```
By running:
```bash
route -n
```
We can see that each of the computers is now routing to the other one's IP, as intended!

3. By running *ipconfig* we can see each computers MAC. For TUX43 we get: *ether 00:21:5a:61:2f:13*. For TUX44 we get: *ether 00:21:5a:c3:78:76*!
   
4. In each computer, to ensure that the connection is now active, we *ping* the other computer to see if we are now able to send information!
```bash
ping 172.16.40.254 #for the Tux43
ping 172.16.40.1 #for the Tux44    
```
Each time 64 bytes of data are being sent periodically when doing either ping, so we know it's working!

5. We now run the *arp* ( Address Resolution Protocol ) in each of the computers.
```bash
arp -a
#(172.16.40.254) at ether 00:21:5a:c3:78:76 [ether] on eth0    for Tux43
#(172.16.40.1) at ether 00:21:5a:61:2f:13 [ether] on eth0    for Tux43
```
As expected, we get the other's IP and MAC.

6. In TUX43 now:
```bash
arp -d 172.16.40.254/24
arp -a # empty
```

7. Open WireShark in Tux43 e start capturing on the *eht0* channel.

8. Run *ping 172.16.40.254/24*.
9. Stop capturing results after some 10 seconds and the result is:
