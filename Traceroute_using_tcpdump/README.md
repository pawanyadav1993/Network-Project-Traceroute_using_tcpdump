Network-Project-Traceroute_using_tcpdump

Simple Network based project in C/C++ standard library

The program needs to take the file name of the tcpdump trace as the first argument. 
For example, the program needs to run as:

$ ./traceroute_analysis traceroute_TCP_trace.txt

As example the tcpdump file will contain following packets:

1291654312.963267 IP (tos 0x0, ttl 3, id 9075, offset 0, flags [none], proto TCP (6), length 60) 128.192.76.177.56812 > 137.138.144.168.80: S, cksum 0x5834 (correct), 2778675862:2778675862(0) win 5840 <mss 1460,sackOK,timestamp 2152510109 0,nop,wscale 2>

1291654312.963655 IP (tos 0x0, ttl 62, id 47385, offset 0, flags [none], proto ICMP (1), length 56) 128.192.254.49 > 128.192.76.177: ICMP time exceeded in-transit, length 36 IP (tos 0x0, ttl 1, id 9075, offset 0, flags [none], proto TCP (6), length 60) 128.192.76.177.56812 > 137.138.144.168.80: tcp 40 [bad hdr length 0 - too short, < 20]

The output of above program should lookalike Traceroute program.

TTL 4 
129.192.252.24 
0.388 ms 0.401 ms 0.398 ms

Thanks!!
