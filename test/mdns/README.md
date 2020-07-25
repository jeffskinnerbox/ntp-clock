
DNS works great for normal sites on the Internet, but most local networks don't have their own DNS server.
This means that you can't reach local devices using a domain name, and you're stuck using IP addresses
Fortunately, there's another way: multicast DNS, or mDNS.
The multicast DNS (mDNS) protocol resolves hostnames to IP addresses within small networks that do not include a local name server.
mDNS uses domain names with the `.local` suffix.


* [ESP32: mDNS address resolution](https://www.dfrobot.com/blog-1525.html?tracking=5ee9f0a9c26a4)
* [ESP32: Advertise service with mDNS](https://techtutorialsx.com/2020/04/18/esp32-advertise-service-with-mdns/)
* [ESP32: Query mDNS service](https://www.dfrobot.com/blog-1528.html?tracking=5ee9f0a9c26a4)
* [Multicast Domain Name System](https://tttapa.github.io/ESP8266/Chap08%20-%20mDNS.html)
