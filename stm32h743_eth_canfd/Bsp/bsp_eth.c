#include "bsp_eth.h"
#include "main.h"
#include "lwip.h"
#include "udp.h"
#include "igmp.h"
#include <stdio.h>
#include <string.h>

struct udp_pcb *multicast_pcb;
ip4_addr_t group_addr;
uint8_t multicast_buf[MULTICAST_RX_BUF];

void multicast_init_ip_port(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port)
{
    err_t err;

    IP4_ADDR(&group_addr, ip1, ip2, ip3, ip4);
    err = igmp_joingroup(IP_ADDR_ANY, &group_addr);
    if (err != ERR_OK) {
        printf("igmp_joingroup error\r\n");
        return;
    }

    multicast_pcb = udp_new();
    if(multicast_pcb == NULL) {
        printf("error creating udp\r\n");
        return;
    }

    err = udp_bind(multicast_pcb, IP_ADDR_ANY, port);
    if (err != ERR_OK) {
        printf("bind udp fail\r\n");
        udp_remove(multicast_pcb);
        multicast_pcb = NULL;
        return;
    }

    udp_recv(multicast_pcb, (udp_recv_fn)multicast_recv_callback , NULL);
    //printf( "Udp Multicast connecting .....\r\n");
}

void multicast_init(void)
{
    multicast_init_ip_port(MIP1, MIP2, MIP3, MIP4, MPORT);
}

void multicast_send(uint8_t *data, uint16_t len)
{
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    memcpy(p->payload, data, len);
    p->len = len;
    multicast_pcb->local_port = MPORT;  //(MPORT + 1000) & 0xFFFF;
    udp_sendto(multicast_pcb, p, &group_addr, RPORT);
    pbuf_free(p);
}

__weak void multicast_rx_callback(void *arg, struct udp_pcb *upcb,struct pbuf *p, ip_addr_t *addr, u16_t port)
{
    UNUSED(arg);
    UNUSED(upcb);
    UNUSED(p);
    UNUSED(addr);
    UNUSED(port);
}

void multicast_recv_callback(void *arg, struct udp_pcb *upcb,struct pbuf *p, ip_addr_t *addr, u16_t port)
{

    multicast_rx_callback(arg,upcb,p,addr,port);
    pbuf_free(p);

    //int i,j;
    //uint16_t lwip_demo_buf_len = 0;

	// if(p!=NULL){
	// 	if((p->tot_len)>=MULTICAST_RX_BUF){
	// 		memcpy(multicast_buf,p->payload,MULTICAST_RX_BUF);
	// 		lwip_demo_buf_len = MULTICAST_RX_BUF;
	// 	}else{
	// 		memcpy(multicast_buf,p->payload,p->tot_len);
	// 		lwip_demo_buf_len = p->tot_len;
	// 	}

	//  for(i=0;i<p->tot_len;i++)
	// 	{
	// 		printf("%02x  ",multicast_buf[i]);
	// 	  multicast_buf[i]=multicast_buf[i]+2;
	// 	}

	// 	printf("\n");

    //  pbuf_free(p);

	// }
}

void eth_init(void)
{
    //phy reset
    HAL_GPIO_WritePin(ETH_RST_GPIO_Port, ETH_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(ETH_RST_GPIO_Port, ETH_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(10);

    MX_LWIP_Init();

    multicast_init();
}

void eth_process(void)
{
    MX_LWIP_Process();
}

extern struct netif gnetif;
uint32_t eth_get_ip_addr(void)
{
    return (uint32_t)gnetif.ip_addr.addr;
}
