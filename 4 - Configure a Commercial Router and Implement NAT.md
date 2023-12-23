/ip address add address=172.16.1.29/24 interface=ether1
/ip address add address=172.16.21.254/24 interface=ether2

Connect cable from T4 to Console or RouterMT depending who we're talking to

route add default gw 172.16.20.254 #Tux3
route add default gw 172.16.21.254 #Tux2 e Tux4

tux 2 e rc: ip route add 172.16.20/24 via 172.16.21.253

route -n

in router:
/ip route add dst-address=0.0.0.0/0 gateway=172.16.1.254 (1 is room)
/ip route add dst-address=172.16.20.0/24 gateway =172.16.21.253
/ip route print


Tux3:
ping 172.16.21.1
ping 172.16.21.253
ping 172.16.1.29


sysctl net.ipv4.conf.eth0.accept_redirects=0
sysctl net.ipv4.conf.all.accept_redirects=0

route del -net 172.16.20.0/24 gw 172.16.21.253
ping 172.16.20.1

traceroute 172.16.20.1
route add -net 172.16.20.0/24 gw 172.16.21.253
traceroute 172.16.20.1

sysctl net.ipv4.conf.eth0.accept_redirects=1
sysctl net.ipv4.conf.all.accept_redirects=1

in tux 4:
/ip firewall nat disable 0
