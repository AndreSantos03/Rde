#Configure an IP Network

##Steps
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

![Alt text](/images/image.png)

##Questions

- **What are the commands required to configure this experience?**
    This experience requires *ipconfig* alone (displays te information about network interfaces, including their IP and MAC addresses), *ipconfig eth0 ADDRESS* (to enable a network communication throught a certain interface), *route -n* (to display the IP routing table), *ping* (to check the connectivety between the different TUX), *arp -a* (shows a a table of IP addresses and their corresponding MAC addresses) and *arp -d* (deletes entries in the ARP table).

- **What are the ARP packets and what are they used for?**
    ARP(Address Resolution Protocol) packets are packets that estabilish connection between an IP and a MAC address.

- **What are the MAC and IP addresses of ARP packets and why?**
    The ARP packets contain information about the IP of the source or TUX43(172.16.40.01) and of the destination or TUX 44(172.16.40.254), or the other way around. We also get the MAC address as a response from the other machine, which means TUX43 gets 00:21:5a:c3:78:76 and TUX44 gets 00:21:5a:61:2f:13.

- **What packets does the ping command generate?**
    At first, as we don't have the other machine's MAC address, the *ping* command sends ARP packets, which allows to create a connection and then *ping* sends ICMP(Internet Control Message Protocol) packets.

- **What are the MAC and IP addresses of the ping packets?**
    They are the ones of the machine itself that's sending the ICMP packet.

- **How to determine if a receiving Ethernet frame is ARP, IP, ICMP?**
    We can easily see in the Wireshark capture in the Protocol table, but we can also see in the frame itself as they're the first 2 bytes in the header.

- **How to determine the length of a receiving frame?**
    There's also a table called Length in Wireshark that tells us that!

- **What is the loopback interface and why is it important?**   
    It's a virtual interface that remains accessible as long as at least one of the switch's IP interfaces is operational, allowing us to regularly check to ensure that there are proper network configurations.