#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define VERSION "1.2024.02.19"
/*
     ifr_flags 的各項旗標和說明：
         IFF_UP              裝置正在運作
         IFF_BROADCAST       有效的廣播位址
         IFF_DEBUG           Debug 標誌
         IFF_LOOPBACK        這是 Loopback 裝置
         IFF_POINTOPOINT     這是點到點的網路裝置介面
         IFF_RUNNING         資源已分配
         IFF_NOARP           無arp協議，沒有設置第二層目的地址
         IFF_PROMISC         介面為雜湊(promiscuous)模式
         IFF_NOTRAILERS      避免使用 trailer
         IFF_ALLMULTI        接收所有群組廣播(multicast)封包資料
         IFF_MASTER          主負載平衡群(bundle)
         IFF_SLAVE           從負載平衡群(bundle)
         IFF_MULTICAST       支援群組廣播(multicast)
         IFF_PORTSEL         可以通過 ifmap 選擇 media 類型
         IFF_AUTOMEDIA       自動選擇 media
         IFF_DYNAMIC         裝置介面關閉時丟棄地址
 */

int is_p2p(const char* ifname,  char* protocol){
    struct ifreq ifr;
    int sockfd;
    int rc = 0;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&ifr, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

    ioctl(sockfd, SIOCGIFFLAGS, &ifr);
     if (ifr.ifr_flags & IFF_POINTOPOINT) {
         fprintf(stderr, "%s is point to point deivce!\n", ifname);
         strncpy(protocol, "p2p", IFNAMSIZ);
        rc = 1;
     }
    close(sockfd);
    return rc;

}
int is_loopback(const char* ifname,  char* protocol){
    struct ifreq ifr;
    int sockfd;
    int rc = 0;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&ifr, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

    ioctl(sockfd, SIOCGIFFLAGS, &ifr);
     if (ifr.ifr_flags & IFF_LOOPBACK) {
        //  fprintf(stderr, "%s is loopback!\n", ifname);
         strncpy(protocol, "Loopback", IFNAMSIZ);
        rc = 1;
     }
    close(sockfd);
    return rc;

}
int getIPAddr(const char* ifname, char* ipaddr){
    int rc = 0;
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;
    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ-1);
    if (ioctl(fd, SIOCGIFADDR, &ifr) !=-1){
        close(fd);
        /* display result */
        //printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
        if (ipaddr) {
            strncpy(ipaddr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), IFNAMSIZ);
        }
        //return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    }else{
        perror(ifname);
        return -1;
    }
    return rc;
}
int getMask(const char* ifname, char* ipmask){
    int rc = 0;
    int fd;
    struct ifreq ifr;
    unsigned int  mask;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;
    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ-1);
    if (ioctl(fd, SIOCGIFNETMASK, &ifr) !=-1){
        close(fd);
        /* display result */
        //printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
         if (ipmask) {
            mask = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
            sprintf(ipmask, "%d.%d.%d.%d", mask & 0xff,
                                            ((mask >> 8) & 0xff),
                                            ((mask >> 16) & 0xff),
                                            ((mask >> 24) & 0xff));
         }
    }else{
        perror(ifname);
        return -1;
    }
    return rc;
}
int check_wireless(const char* ifname, char* protocol) {
  int sock = -1;
  struct iwreq pwrq;
  memset(&pwrq, 0, sizeof(pwrq));
  strncpy(pwrq.ifr_name, ifname, IFNAMSIZ);

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    return 0;
  }

  if (ioctl(sock, SIOCGIWNAME, &pwrq) != -1) {
    if (protocol) strncpy(protocol, pwrq.u.name, IFNAMSIZ);
    close(sock);
    return 1;
  }

  close(sock);
  return 0;
}

void usage( char *name) {
        printf("Usage: \n");
        printf("%s [-a] [-d]\n",name);
        printf(" -a: show all interface include loopbak, ppp\n");
        printf(" -d: show detail info of the interface\n");
        printf(" -w: show wifi interface only\n");
        printf(" -v: show version\n");
        printf(" -h: show this help\n");
        exit(1);
}
void showversion(){
    fprintf(stderr, "v%s\n", VERSION);
    exit(1);
}
int main(int argc, char  *argv[]) {
    bool showAll=false;
    bool showDetail=false;
    bool showWiFi=false;
    int c;
    extern char *optarg;
    while(( c = getopt(argc,argv, "adwvh" )) != EOF ) {    // Loop the arguments
        switch(c) {
        case 'w':
            showWiFi=true;
            break;
        case 'a':
            //printf("a option %s\n",optarg);
            showAll=true;
              break;
        case 'd':
            showDetail=true;
              break;
        case 'v':
            showversion();
              break;
        case 'h':
        default:  usage( argv[0]);
        }
    }

  struct ifaddrs *ifaddr, *ifa;

  if (getifaddrs(&ifaddr) == -1) {
    perror("getifaddrs");
    return -1;
  }

  /* Walk through linked list, maintaining head pointer so we
     can free list later */
  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    char protocol[IFNAMSIZ]  = {0};
    char detail[IFNAMSIZ]  = {0};
    char ipaddr[IFNAMSIZ]  = {0};
    char ipmask[20]  = {0};

    if (ifa->ifa_addr == NULL ||
        ifa->ifa_addr->sa_family != AF_PACKET) {
            // not addr,  not AF_PACKET (Ethernet II)
            continue;
    }

    if (check_wireless(ifa->ifa_name, protocol)) {
        if (showWiFi || showAll){
            //printf("%s, wireless: %s\n", ifa->ifa_name, protocol);
            if (showDetail) {
                if ((getIPAddr(ifa->ifa_name, ipaddr)==0) && (getMask(ifa->ifa_name, ipmask)==0)){
                    printf("%s, Wlan, %s, %s, %s\n", ifa->ifa_name, protocol, 
                        ipaddr, ipmask);
                }else{
                    printf("%s, Wlan, %s\n", ifa->ifa_name, protocol);
                }
            } else{
                printf("%s\n", ifa->ifa_name);
            }
        }
    } else {
        if (!showWiFi) {
            strncpy(detail, "Net", IFNAMSIZ);
            if (is_loopback(ifa->ifa_name, detail)){
                if (! showAll){   // ignore loopback
                    continue;
                }
            }
            if(is_p2p(ifa->ifa_name, detail)){
                if (! showAll){   // ignore p2p
                    continue;
                }
            }
            if (showDetail) {
                if ((getIPAddr(ifa->ifa_name, ipaddr)==0) && (getMask(ifa->ifa_name, ipmask)==0)){
                    printf("%s, %s, %s, %s\n", ifa->ifa_name, detail, 
                        ipaddr, ipmask);
                }else{
                    printf("%s, %s\n", ifa->ifa_name, detail);
                }
            } else {
                printf("%s\n", ifa->ifa_name);
            }
        }
    }
  }
  freeifaddrs(ifaddr);
  return 0;
}
