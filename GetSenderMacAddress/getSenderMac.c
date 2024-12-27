#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>  // Include this header for RTM_GETNEIGH and related types
#include <netinet/ether.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_PAYLOAD 1024

// Function to retrieve MAC address from Netlink ARP cache
int get_mac_address(const char *ip_addr)
{
    int                sockfd;
    struct sockaddr_nl sa;
    struct nlmsghdr   *nlh;
    struct rtmsg      *route_msg;
    struct iovec       iov;
    struct msghdr      msg;
    char               buffer[MAX_PAYLOAD];
    int                ret;

    sockfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        return -1;
    }

    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;

    nlh = (struct nlmsghdr *)buffer;
    nlh->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    nlh->nlmsg_type = RTM_GETNEIGH;  // Correct message type for neighbor info (ARP cache)
    nlh->nlmsg_flags = NLM_F_REQUEST;
    nlh->nlmsg_seq = 1;

    route_msg = (struct rtmsg *)NLMSG_DATA(nlh);
    route_msg->rtm_family = AF_INET;       // Use IPv4
    route_msg->rtm_table = RT_TABLE_MAIN;  // Default routing table

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&sa;
    msg.msg_namelen = sizeof(sa);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // Send the request to fetch ARP information
    ret = sendmsg(sockfd, &msg, 0);
    if (ret < 0)
    {
        perror("Send failed");
        close(sockfd);
        return -1;
    }

    // Receive the reply
    ret = recv(sockfd, buffer, sizeof(buffer), 0);
    if (ret < 0)
    {
        perror("Receive failed");
        close(sockfd);
        return -1;
    }

    // Print the raw data (For debugging)
    printf("Received data:\n");
    for (int i = 0; i < ret; i++)
    {
        printf("%02x ", buffer[i]);
    }
    printf("\n");

    close(sockfd);
    return 0;
}

int main()
{
    const char *ip_addr = "192.168.27.48";  // Replace with the IP you're interested in

    if (get_mac_address(ip_addr) < 0)
    {
        printf("Failed to retrieve MAC address.\n");
    }
    else
    {
        printf("MAC address found for %s\n", ip_addr);
    }

    return 0;
}
