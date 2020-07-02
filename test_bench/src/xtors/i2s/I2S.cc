#include "global.h"
#include "/home/shares/zebu_ip_2017.12/I2S/include/Audio.hh"
using namespace I2S;

//only enable 1 resolution
#define I2S0_RESOLUTION32_TEST
//#define I2S0_RESOLUTION24_TEST
//#define I2S0_RESOLUTION16_TEST

//only enable 1 resolution
#define I2S1_RESOLUTION32_TEST
//#define I2S1_RESOLUTION24_TEST
//#define I2S1_RESOLUTION16_TEST

//#define I2S0_GATING_TEST //zebu do not support clock gating

pthread_t i2s1treadID = 0;
pthread_t i2s0treadID = 0;
I2s  *i2s0    = NULL;
I2s  *i2s1    = NULL;

const unsigned int data_pattern[8] = {
0xFFFFFFFF, 0xAAAAAAAA, 0x55555555, 0x11111111,
0x12345678, 0x87654321, 0xAABBCCDD, 0xDDCCBBAA,
};
unsigned int i2sbuf[512];
int rxcnt = 0;

void rxbuf_dump()
{
	int i;

    printf("\ni2s rx buf dump(left):\n");
    for(i = 0; i < rxcnt; i+= 2)
        printf("0x%08x, ", i2sbuf[i]);

    printf("\ni2s rx buf dump(right):\n");
    for(i = 1; i < rxcnt; i+= 2)
        printf("0x%08x, ", i2sbuf[i]);
	printf("\n");
}

void i2s0_thread(void* argv)
{
	I2s  *i2s = (I2s*)argv;
	unsigned int ldata, rdata;
	int ret, i, index;
	int start_flag = false, fail_cnt = 0;
	int gating_test_cnt;
	bool test_pass = true; //default pass


    printf("i2s0 thread run.\n");
#if 0
	if(i2s->isTransmitter()) {
		printf("i2s0 thread get a i2s Transmiterr.\n");
	} else if(i2s->isReceiver()) {
		printf("i2s0 thread get a i2s Receiver.\n");
		while(1) {
			if(1 == i2s->receive(&ldata, &rdata, 0)) {
				//printf("i2s slave rx data: ldata(0x%x), rdata(0x%x)\n", ldata, rdata);
				i2sbuf[cnt++] = ldata;
				i2sbuf[cnt++] = rdata;
				if(cnt%20 == 0) {
					printf("%x,%x,%x,%x\n", i2sbuf[cnt-20], i2sbuf[cnt-19], i2sbuf[cnt-18], i2sbuf[cnt-17]);
					printf("%x,%x,%x,%x\n", i2sbuf[cnt-16], i2sbuf[cnt-15], i2sbuf[cnt-14], i2sbuf[cnt-13]);
					printf("%x,%x,%x,%x\n", i2sbuf[cnt-12], i2sbuf[cnt-11], i2sbuf[cnt-10], i2sbuf[cnt-9]);
					printf("%x,%x,%x,%x\n", i2sbuf[cnt-8], i2sbuf[cnt-7], i2sbuf[cnt-6], i2sbuf[cnt-5]);
					printf("%x,%x,%x,%x\n", i2sbuf[cnt-4], i2sbuf[cnt-3], i2sbuf[cnt-2], i2sbuf[cnt-1]);
				}
				if(cnt >= 512) {
					cnt = 0;
				}
			}
			usleep(10);
		}
	}
#endif
	if(i2s->isTransmitter()) {

		printf("i2s0 thread get a i2s Transmiterr.\n");

	} else if(i2s->isReceiver()) {
		printf("i2s0 thread get a i2s Receiver.\n");
#ifdef I2S0_GATING_TEST
		for(gating_test_cnt=0; gating_test_cnt < 4; gating_test_cnt++) {
			printf("I2S0 gating test count:%d.\n", gating_test_cnt);
			rxcnt = 0;
			start_flag = false;
			fail_cnt = 0;
			while(1) {
				if(1 == i2s->receive(&ldata, &rdata, 0)) {
					start_flag = true; //the first rx success means rx start
					i2sbuf[rxcnt++] = ldata;
					i2sbuf[rxcnt++] = rdata;
					if(rxcnt >= 512) {
						rxcnt = 0;
					}
				} else {
					if(start_flag) {
						fail_cnt++; //if start rx, static fail rx count
					}
				}
				usleep(10);
				if(fail_cnt > 10000) { //fail count > 10000 assume rx end
					break;
				}
			}
			rxbuf_dump();
			/* check left channel data */
			printf("check left channel rx data:\n");
			for(i = 0; i < rxcnt && (i2sbuf[i] == 0); i+=2); //find the first non-zero data, only check left channel, so i+=2
			for(index = 0; index < sizeof(data_pattern)/sizeof(unsigned int); i+=2, index++) {
				switch(gating_test_cnt) {
					case 0:
						if(i2sbuf[i] == (data_pattern[index]&0xFFF)) {
							printf("OK: rx data(0x%08x)\n", i2sbuf[i]);
						} else {
							test_pass = false;
							printf("ERROR: rx data(0x%08x), expect data(0x%08x)\n", i2sbuf[i], data_pattern[index]);
						}
						break;
					case 1:
						if(i2sbuf[i] == (data_pattern[index]&0xFFFF)) {
							printf("OK: rx data(0x%08x)\n", i2sbuf[i]);
						} else {
							test_pass = false;
							printf("ERROR: rx data(0x%08x), expect data(0x%08x)\n", i2sbuf[i], data_pattern[index]);
						}
						break;
					case 2:
						if(i2sbuf[i] == (data_pattern[index]&0xFFFFF)) {
							printf("OK: rx data(0x%08x)\n", i2sbuf[i]);
						} else {
							test_pass = false;
							printf("ERROR: rx data(0x%08x), expect data(0x%08x)\n", i2sbuf[i], data_pattern[index]);
						}
						break;
					case 3:
						if(i2sbuf[i] == (data_pattern[index]&0xFFFFFF)) {
							printf("OK: rx data(0x%08x)\n", i2sbuf[i]);
						} else {
							test_pass = false;
							printf("ERROR: rx data(0x%08x), expect data(0x%08x)\n", i2sbuf[i], data_pattern[index]);
						}
						break;
					default:
						printf("wrong gating number\n");
						break;
				}

			}
			/* check right channel data */
			printf("check right channel rx data:\n");
			for(i = 1; i < rxcnt && (i2sbuf[i] == 0); i+=2); //find the first non-zero data, only check left channel, so i+=2
			for(index = 0; index < sizeof(data_pattern)/sizeof(unsigned int); i+=2, index++) {
				switch(gating_test_cnt) {
					case 0:
						if(i2sbuf[i] == (data_pattern[index]&0xFFF)) {
							printf("OK: rx data(0x%08x)\n", i2sbuf[i]);
						} else {
							test_pass = false;
							printf("ERROR: rx data(0x%08x), expect data(0x%08x)\n", i2sbuf[i], data_pattern[index]);
						}
						break;
					case 1:
						if(i2sbuf[i] == (data_pattern[index]&0xFFFF)) {
							printf("OK: rx data(0x%08x)\n", i2sbuf[i]);
						} else {
							test_pass = false;
							printf("ERROR: rx data(0x%08x), expect data(0x%08x)\n", i2sbuf[i], data_pattern[index]);
						}
						break;
					case 2:
						if(i2sbuf[i] == (data_pattern[index]&0xFFFFF)) {
							printf("OK: rx data(0x%08x)\n", i2sbuf[i]);
						} else {
							test_pass = false;
							printf("ERROR: rx data(0x%08x), expect data(0x%08x)\n", i2sbuf[i], data_pattern[index]);
						}
						break;
					case 3:
						if(i2sbuf[i] == (data_pattern[index]&0xFFFFFF)) {
							printf("OK: rx data(0x%08x)\n", i2sbuf[i]);
						} else {
							test_pass = false;
							printf("ERROR: rx data(0x%08x), expect data(0x%08x)\n", i2sbuf[i], data_pattern[index]);
						}
						break;
					default:
						printf("wrong gating number\n");
						break;
				}
			}
		}
#endif
    unsigned int i2s_receive_count = 0;
    uint32_t is_first_receive = 0;
    uint32_t resolution = 0;
#ifdef I2S0_RESOLUTION32_TEST
    resolution = 32;
#endif
#ifdef I2S0_RESOLUTION24_TEST
    resolution = 24;
#endif
#ifdef I2S0_RESOLUTION16_TEST
    resolution = 16;
#endif
//#ifdef I2S0_RESOLUTION32_TEST
    while(1) {
        printf("\nI2S0 resolution %d test. receive count: %d\n", resolution, i2s_receive_count);
        is_first_receive = 0;
        i2s_receive_count ++;
		rxcnt = 0;
		start_flag = false;
        fail_cnt = 0;
		while(1) {
			if(1 == i2s->receive(&ldata, &rdata, 0)) {
                if(! is_first_receive) {
                    is_first_receive = 1;
                    if(i2s_receive_count > 1)
                        continue;
                }
				start_flag = true; //the first rx success means rx start
				i2sbuf[rxcnt++] = ldata;
				i2sbuf[rxcnt++] = rdata;
                fail_cnt = 0;
				if(rxcnt >= 512) {
					rxcnt = 0;
				}
			} else {
				if(start_flag) {
					fail_cnt++; //if start rx, static fail rx count
				}
			}
			usleep(10);
			if(fail_cnt > 10000) { //fail count > 10000 assume rx end
				break;
			}
        }
		rxbuf_dump();
		/* check left channel data */
        printf("unknow ws width, please check the data yourself\r\n");
//		printf("check left channel rx data:\n");
//		for(i = 0; i < rxcnt && (i2sbuf[i] == 0); i+=2); //find the first non-zero data, only check left channel, so i+=2
//		for(index = 0; index < sizeof(data_pattern)/sizeof(unsigned int); i+=2, index++) {
//			if(i2sbuf[i] == (data_pattern[index]&0xFFFFFFFF)) {
//				printf("OK: rx data(0x%08x)\n", i2sbuf[i]);
//			} else {
//				test_pass = false;
//				printf("ERROR: rx data(0x%08x), expect data(0x%08x)\n", i2sbuf[i], data_pattern[index]);
//			}
//		}
//		/* check right channel data */
//		printf("check right channel rx data:\n");
//		for(i = 1; i < rxcnt && (i2sbuf[i] == 0); i+=2); //find the first non-zero data, only check left channel, so i+=2
//		for(index = 0; index < sizeof(data_pattern)/sizeof(unsigned int); i+=2, index++) {
//			if(i2sbuf[i] == (data_pattern[index]&0xFFFFFFFF)) {
//				printf("OK: rx data(0x%08x)\n", i2sbuf[i]);
//			} else {
//				test_pass = false;
//				printf("ERROR: rx data(0x%08x), expect data(0x%08x)\n", i2sbuf[i], data_pattern[index]);
//			}
//		}
    }
//#endif
#ifdef I2S0_RESOLUTION24_TEST
		printf("I2S0 resolution 24 test.\n");
		rxcnt = 0;
		start_flag = false;
		fail_cnt = 0;
		while(1) {
			if(1 == i2s->receive(&ldata, &rdata, 0)) {
				start_flag = true; //the first rx success means rx start
				i2sbuf[rxcnt++] = ldata;
				i2sbuf[rxcnt++] = rdata;
				if(rxcnt >= 512) {
					rxcnt = 0;
				}
			} else {
				if(start_flag) {
					fail_cnt++; //if start rx, static fail rx count
				}
			}
			usleep(10);
			if(fail_cnt > 10000) { //fail count > 10000 assume rx end
				break;
			}
		}
		rxbuf_dump();
		/* check left channel data */
        printf("unknow ws width, please check the data yourself");
//		printf("check left channel rx data:\n");
//		for(i = 0; i < rxcnt && (i2sbuf[i] == 0); i+=2); //find the first non-zero data, only check left channel, so i+=2
//		for(index = 0; index < sizeof(data_pattern)/sizeof(unsigned int); i+=2, index++) {
//			if(i2sbuf[i] == (data_pattern[index]&0xFFFFFF)) {
//				printf("OK: rx data(0x%08x)\n", i2sbuf[i]);
//			} else {
//				test_pass = false;
//				printf("ERROR: rx data(0x%08x), expect data(0x%08x)\n", i2sbuf[i], ((data_pattern[index]&0xFFFFFF)));
//			}
//		}
//		/* check right channel data */
//		printf("check right channel rx data:\n");
//		for(i = 1; i < rxcnt && (i2sbuf[i] == 0); i+=2); //find the first non-zero data, only check left channel, so i+=2
//		for(index = 0; index < sizeof(data_pattern)/sizeof(unsigned int); i+=2, index++) {
//			if(i2sbuf[i] == (data_pattern[index]&0xFFFFFF)) {
//				printf("OK: rx data(0x%08x)\n", i2sbuf[i]);
//			} else {
//				test_pass = false;
//				printf("ERROR: rx data(0x%08x), expect data(0x%08x)\n", i2sbuf[i], ((data_pattern[index]&0xFFFFFF)));
//			}
//		}
			
#endif
#ifdef I2S0_RESOLUTION16_TEST
		printf("I2S0 resolution 16 test.\n");
		rxcnt = 0;
		start_flag = false;
		fail_cnt = 0;
		while(1) {
			if(1 == i2s->receive(&ldata, &rdata, 0)) {
				start_flag = true; //the first rx success means rx start
				i2sbuf[rxcnt++] = ldata;
				i2sbuf[rxcnt++] = rdata;
				if(rxcnt >= 512) {
					rxcnt = 0;
				}
			} else {
				if(start_flag) {
					fail_cnt++; //if start rx, static fail rx count
				}
			}
			usleep(10);
			if(fail_cnt > 10000) { //fail count > 10000 assume rx end
				break;
			}
		}
		rxbuf_dump();
		/* check left channel data */
        printf("unknow ws width, please check the data yourself");
//		printf("check left channel rx data:\n");
//		for(i = 0; i < rxcnt && (i2sbuf[i] == 0); i+=2); //find the first non-zero data, only check left channel, so i+=2
//		for(index = 0; index < sizeof(data_pattern)/sizeof(unsigned int); i+=2, index++) {
//			if(i2sbuf[i] == (data_pattern[index]&0xFFFF)) {
//				printf("OK: rx data(0x%08x)\n", i2sbuf[i]);
//			} else {
//				test_pass = false;
//				printf("ERROR: rx data(0x%08x), expect data(0x%08x)\n", i2sbuf[i], ((data_pattern[index]&0xFFFF)));
//			}
//		}
//		/* check left channel data */
//		printf("check right channel rx data:\n");
//		for(i = 1; i < rxcnt && (i2sbuf[i] == 0); i+=2); //find the first non-zero data, only check left channel, so i+=2
//		for(index = 0; index < sizeof(data_pattern)/sizeof(unsigned int); i+=2, index++) {
//			if(i2sbuf[i] == (data_pattern[index]&0xFFFF)) {
//				printf("OK: rx data(0x%08x)\n", i2sbuf[i]);
//			} else {
//				test_pass = false;
//				printf("ERROR: rx data(0x%08x), expect data(0x%08x)\n", i2sbuf[i], ((data_pattern[index]&0xFFFF)));
//			}
//		}
#endif
	}

	if(test_pass) {
		printf("\nI2S0 test pass\n");
	} else {
		printf("\nI2S0 test fail\n");
	}

	while(1) {
		sleep(1);
	}
out:	
	if (i2s != NULL) { 
		delete i2s; 
		i2s = NULL;
	}
}

void i2s1_thread(void* argv)
{
	I2s  *i2s = (I2s*)argv;
	unsigned int ldata, rdata;
	int ret, i;
	bool test_pass = true;

	printf("i2s1 thread run.\n");
	if(i2s->isTransmitter()) {
		printf("i2s1 thread get a i2s Transmiterr.\n");
#if 0
		while(1) {
			i2s->send(0xaa, 0x55, true);
			usleep(50);
		}
#endif
//#ifdef I2S1_RESOLUTION32_TEST
        uint32_t resolution = 0;
#ifdef I2S0_RESOLUTION32_TEST
        resolution = 32;
#endif
#ifdef I2S0_RESOLUTION24_TEST
        resolution = 24;
#endif
#ifdef I2S0_RESOLUTION16_TEST
        resolution = 16;
#endif
        printf("I2S1 resolution %d test. send data every 2 seconds\n", resolution);
        //xtor_dut_sync_read("i2s1 ready");
        while(1) {
            // printf("sleep flag\r\n");
            i2s->send(0, 0, true); //the first left channel data will lost, so send dummy 0 at the first time
            for(i = 0; i < 8; i++) {
                ret = i2s->send(data_pattern[i], data_pattern[i], true);
                if(ret != 1) {
                    test_pass = false;
                    // printf("i2s1 blocking send result:%d.\n", ret);
                }
            }
            sleep(2);
        }
//#endif
#ifdef I2S1_RESOLUTION24_TEST
		printf("I2S1 resolution 24 test.\n");
		i2s->send(0, 0, true); //the first left channel data will lost, so send dummy 0 at the first time
		//xtor_dut_sync_read("i2s1 ready");
		for(i = 0; i < 8; i++) {
			ret = i2s->send(data_pattern[i], data_pattern[i], true);
			if(ret != 1) {
				test_pass = false;
				printf("i2s1 blocking send result:%d.\n", ret);
			}
		}
			
#endif
#ifdef I2S1_RESOLUTION16_TEST
		printf("I2S1 resolution 16 test.\n");
		i2s->send(0, 0, true); //the first left channel data will lost, so send dummy 0 at the first time
		//xtor_dut_sync_read("i2s1 ready");
		for(i = 0; i < 8; i++) {
			ret = i2s->send(data_pattern[i], data_pattern[i], true);
			if(ret != 1) {
				test_pass = false;
				printf("i2s1 blocking send result:%d.\n", ret);
			}
		}
			
#endif
	} else if(i2s->isReceiver()) {
		printf("i2s1 thread get a i2s Receiver.\n");
	}

	if(test_pass) {
		printf("\nI2S0 test pass\n");
	} else {
		printf("\nI2S0 test fail\n");
	}

	while(1) {
		sleep(1);
	}
out:	
	if (i2s != NULL) { 
		delete i2s; 
		i2s = NULL;
	}
}

int I2S0Init(Board * board)
{
	if(i2s0){
		printf("I2S0 has be inited. please recheck it.");
		return -1;
	}
	i2s0 = new I2s;
	//i2s1->setBfmCfg(1,0,32,2) ; //if enalbe will segament error
	i2s0->setDebugLevel(0);
#ifdef I2S0_RESOLUTION32_TEST
	i2s0->init(board,I2S0_TRAS_DEV,0, 0, 32); //para3: 0(slave), 1(master)
#endif
#ifdef I2S0_RESOLUTION24_TEST
	i2s0->init(board,I2S0_TRAS_DEV,0, 0, 24); //para3: 0(slave), 1(master)
#endif
#ifdef I2S0_RESOLUTION16_TEST
	i2s0->init(board,I2S0_TRAS_DEV,0, 0, 16); //para3: 0(slave), 1(master)
#endif
	//i2s0->setOutFile("testout.au",8000,au_16bl);
	if(i2s0->isTransmitter()) {
		printf("i2s0 is i2s Transmiterr.\n");
	} else if(i2s0->isReceiver()) {
		printf("i2s0 is i2s Receiver.\n");
    }
     return 0;
}

int I2S1Init(Board * board)
{
		printf("*************************************\n");
		printf("********   init i2s master   ********\n");
		printf("*************************************\n");
		i2s1 = new I2s;
		//i2s1->setAudioFormat(I2S_Audio);
#ifdef I2S1_RESOLUTION32_TEST
		i2s1->init(board, I2S1_TRAS_DEV, 1, 0, 32);
#endif
#ifdef I2S1_RESOLUTION24_TEST
		i2s1->init(board, I2S1_TRAS_DEV, 1, 0, 24);
#endif
#ifdef I2S1_RESOLUTION16_TEST
		i2s1->init(board, I2S1_TRAS_DEV, 1, 0, 16);
#endif
		if(i2s1->isTransmitter()) {
			printf("i2s1 is i2s Transmiterr.\n");
		} else if(i2s1->isReceiver()) {
			printf("i2s1 is i2s Receiver.\n");
		}

}

int I2S0Run()
{
	printf("I2S0 run.\n");
	if(pthread_create(&i2s0treadID,NULL,(void* (*)(void*))i2s0_thread, (void*)i2s0)){
		throw ("Could not creat i2s0 pthread\n"); 
	}
     return 0;
}
  


int I2S1Run()
{
	printf("I2S1 run.\n");
	if(pthread_create(&i2s1treadID,NULL,(void* (*)(void*))i2s1_thread, (void*)i2s1)){
		throw ("Could not creat i2s1 pthread\n"); 
	}
     return 0;
}

