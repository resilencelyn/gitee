#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/ip.h>
#include <linux/tcp.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct ethhdr *hdr_ether;
struct iphdr  *hdr_ip;
struct tcphdr *hdr_tcp;
struct arphdr *hdr_arp;

void dumpData(unsigned char *data, int size) {
	char a , line[17] , c;
	int i, j;

	printf("\t\t");

	//loop over each character and print
	for(i=0 ; i < size ; i++) {
		c = data[i];

		//Print the hex value for every character , with a space. Important to make unsigned
		printf("%.2x ", (unsigned char) c);

		//Add the character to data line. Important to make unsigned
		a = ( c >= 32 && c <= 128) ? (unsigned char) c : '.';

		line[i % 16] = a;

		//if last character of a line , then print the line - 16 characters in 1 line
		if ( (i != 0 && (i + 1) % 16 == 0) || i == size - 1) {
			line[i % 16 + 1] = '\0';

			//print a big gap of 10 characters between hex and characters
			printf("\t");

			//Print additional spaces for last lines which might be less than 16 characters in length
			for( j = strlen(line) ; j < 16; j++)
			{
				printf("   ");
			}

			printf("%s\n\t\t" , line);
		}
	}

	printf("\n");
}

void decodeTCPPacket(unsigned char *buffer, int size) {
	hdr_tcp = (struct tcphdr*)buffer;

	printf("\t\trcvlen: %4d\t", size);

	int src = ((int)buffer[0] << 8) + buffer[1];
	int des = ((int)buffer[2] << 8) + buffer[3];

	// Port number
	printf("%d -> %d\t", src, des);

	// Data offset (the size of the TCP header in 32-bit words)
	int hdr_len = hdr_tcp->doff * 4;
	printf("header length: %d\n", hdr_len);

	// if (src == 80 || des == 80)
		dumpData(&buffer[hdr_len], size - hdr_len);
}

void decodeIPPacket(unsigned char *buffer, int size) {
	hdr_ip = (struct iphdr*)buffer;

	printf("\trcvlen:%4d\t", size);

	// version
	int version = hdr_ip->version;
	printf("version: %d\t", version);
	switch (version) {
		case 4:
			// header length
			printf("header length: %d\t", hdr_ip->ihl);

			// total length
			printf("total length: %d\n\t", hdr_ip->tot_len);

			// IP address
			struct in_addr ip_addr;
			ip_addr.s_addr = hdr_ip->saddr;
			printf("%-15s -> ", inet_ntoa(ip_addr));
			ip_addr.s_addr = hdr_ip->daddr;
			printf("%-15s\t", inet_ntoa(ip_addr));

			// TTL
			printf("TTL: %3d\t", hdr_ip->ttl);

			// Protocol
			printf("Protocol: ");
			switch (hdr_ip->protocol) {
				// ICMP
				case 0x01:
					printf("ICMP\n");
					break;

				// IGMP
				case 0x02:
					printf("IGMP\n");
					break;

				// TCP
				case 0x06:
					printf("TCP\n");
					decodeTCPPacket(&buffer[hdr_ip->ihl * 4], size - hdr_ip->ihl * 4);
					break;

				// UDP
				case 0x11:
					printf("UDP\n");
					break;

				default:
					printf("%2X\n", hdr_ip->protocol);
			};			
			break;
		case 6:
			break;
		default:
			printf("Exception");
	};
	printf("\n");
}
void decodeARPPacket(unsigned char *buffer, int size) {
	hdr_arp = (struct arphdr*)buffer;
	unsigned char *data;

	printf("\trcvlen:%4d\t", size);
	printf("ar_hrd: %x\t", ntohs(hdr_arp->ar_hrd));
	printf("ar_pro: %x\t", ntohs(hdr_arp->ar_pro));
	printf("ar_hln: %x\t", hdr_arp->ar_hln);
	printf("ar_pln: %x\t", hdr_arp->ar_pln);
	printf("ar_op: %x\t", ntohs(hdr_arp->ar_op));
	printf("\n");

	data = buffer + sizeof(struct arphdr);
	printf("\tsrc mac: %02X:%02X:%02X:%02X:%02X:%02X\t",
			data[0], data[1], data[2], data[3], data[4], data[5]);
	data += 6;
	printf("dst mac: %02X:%02X:%02X:%02X:%02X:%02X",
			data[0], data[1], data[2], data[3], data[4], data[5]);
	printf("\n");
}
void decodeEtherFrame(unsigned char *buffer, int size) {
	hdr_ether = (struct ethhdr*)buffer;

	printf("rcvlen:%4d\t", size);

	// source MAC address
	printf("%02X:%02X:%02X:%02X:%02X:%02X",
		hdr_ether->h_source[0], hdr_ether->h_source[1], hdr_ether->h_source[2], hdr_ether->h_source[3], hdr_ether->h_source[4], hdr_ether->h_source[5]);

	printf(" -> ");

	// destination MAC address
	printf("%02X:%02X:%02X:%02X:%02X:%02X\t",
		hdr_ether->h_dest[0], hdr_ether->h_dest[1], hdr_ether->h_dest[2], hdr_ether->h_dest[3], hdr_ether->h_dest[4], hdr_ether->h_dest[5]);

	// EtherType
	// Deal with endian
	int etherType = ((int)buffer[12] << 8) + buffer[13];
	switch (etherType) {
		case 0x0800:
			printf("IPv4\n");
			decodeIPPacket(&buffer[ETH_HLEN], size - ETH_HLEN);
			break;
		case 0x0806:
			printf("ARP\n");
			decodeARPPacket(&buffer[ETH_HLEN], size - ETH_HLEN);
			break;
		case 0x86DD:
			printf("IPv6\n");
			decodeIPPacket(&buffer[ETH_HLEN], size - ETH_HLEN);
			break;
		default:
			printf("EtherType 0x%04X", etherType);
	};

	printf("\n");
}
