#ifndef BSP_ETH
#define BSP_ETH

#include "main.h"
#include "lwip.h"
#include "udp.h"

//default value
#define MIP1 239
#define MIP2 255
#define MIP3 255
#define MIP4 252
#define MPORT 10000
#define RPORT 20000

#define MULTICAST_RX_BUF 1500

extern void eth_init(void);
extern void eth_process(void);
extern uint32_t eth_get_ip_addr(void);

extern void multicast_init(void);
extern void multicast_init_ip_port(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port);
extern void multicast_send(uint8_t *data, uint16_t len);
extern void multicast_recv_callback(void *arg, struct udp_pcb *upcb,struct pbuf *p, ip_addr_t *addr, u16_t port);
extern void multicast_rx_callback(void *arg, struct udp_pcb *upcb,struct pbuf *p, ip_addr_t *addr, u16_t port);

#endif
