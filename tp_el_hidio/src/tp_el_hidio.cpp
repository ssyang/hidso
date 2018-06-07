#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

#include "el_hidapi.h"
#include <dlfcn.h>//for dynamic loading

/////////////////////////////////////////////////////////
// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

/////////////////////////////////////////////////////////
// dlopen()등을 사용하고 싶다면, libdl을 링크, 즉. -ldl 옵션을 붙여서 컴파일 해야 한다.
// 또한 dl 라이브러리를 링크한다.
// 관리자 권한으로 실행해야 함.
// 일반 유저권한으로 실행하려면 /etc/udev/rules.d/elpusk-usb.rules 파일을 만들고
// 그 파일 내용은
// SUBSYSTEMS=="usb", ATTRS{idVendor}=="134b", ATTRS{idProduct}=="0206", GROUP="users", MODE="0666"
// 로 한다.

///////////////////////////////
// local variables
type_hid_init gel_hid_init = NULL;
type_hid_exit gel_hid_exit = NULL;
type_hid_enumerate gel_hid_enumerate = NULL;
type_hid_free_enumeration gel_hid_free_enumeration = NULL;
type_hid_open_ex gel_hid_open_ex = NULL;
type_hid_open gel_hid_open = NULL;
type_hid_open_path gel_hid_open_path = NULL;
type_hid_write gel_hid_write = NULL;
type_hid_read_timeout gel_hid_read_timeout = NULL;
type_hid_read gel_hid_read = NULL;
type_hid_set_nonblocking gel_hid_set_nonblocking = NULL;
type_hid_send_feature_report gel_hid_send_feature_report = NULL;
type_hid_get_feature_report gel_hid_get_feature_report = NULL;
type_hid_close gel_hid_close = NULL;
type_hid_get_manufacturer_string gel_hid_get_manufacturer_string = NULL;
type_hid_get_product_string gel_hid_get_product_string = NULL;
type_hid_get_serial_number_string gel_hid_get_serial_number_string = NULL;
type_hid_get_indexed_string gel_hid_get_indexed_string = NULL;
type_hid_error gel_hid_error = NULL;

/////////////////////////////////////////////////////////
// local function prototype
static void *_load_library( const char *s_so_name );
static void _free_library( void *h_dll );

static void _display_usb_devices( unsigned char w_vid );
static hid_device *_hid_open( unsigned short w_vid, unsigned short w_pid, int n_interface );

// return value : 보내거나 받은 데이터 수(바이트 단위), 음수면 에러.
static int _hid_tx( hid_device *h_dev, const unsigned char *s_tx, int n_tx );

// return value : 보내거나 받은 데이터 수(바이트 단위), 음수면 에러.
static int _hid_rx( hid_device *h_dev, unsigned char *s_rx, int n_rx );
/////////////////////////////////////////////////////////
// local function bodies
void *_load_library( const char *s_so_name )
{
	void *h_dll = NULL;
	unsigned char b_error = 1;

	do{
		h_dll = dlopen( s_so_name, RTLD_NOW);//RTLD_LAZY
		if( h_dll == NULL ){
			continue;
		}
		//
		gel_hid_init =(type_hid_init)dlsym(h_dll, "hid_init");
		if( gel_hid_init == NULL ){
			continue;
		}
		gel_hid_exit =(type_hid_exit)dlsym(h_dll, "hid_exit");
		if( gel_hid_exit == NULL ){
			continue;
		}
		gel_hid_enumerate =(type_hid_enumerate)dlsym(h_dll, "hid_enumerate");
		if( gel_hid_enumerate == NULL ){
			continue;
		}
		gel_hid_free_enumeration =(type_hid_free_enumeration)dlsym(h_dll, "hid_free_enumeration");
		if( gel_hid_free_enumeration == NULL ){
			continue;
		}
		gel_hid_open_ex =(type_hid_open_ex)dlsym(h_dll, "hid_open_ex");
		if( gel_hid_open_ex == NULL ){
			continue;
		}
		gel_hid_open =(type_hid_open)dlsym(h_dll, "hid_open");
		if( gel_hid_open == NULL ){
			continue;
		}
		gel_hid_open_path =(type_hid_open_path)dlsym(h_dll, "hid_open_path");
		if( gel_hid_open_path == NULL ){
			continue;
		}
		gel_hid_write =(type_hid_write)dlsym(h_dll, "hid_write");
		if( gel_hid_write == NULL ){
			continue;
		}
		gel_hid_read_timeout =(type_hid_read_timeout)dlsym(h_dll, "hid_read_timeout");
		if( gel_hid_read_timeout == NULL ){
			continue;
		}
		gel_hid_read =(type_hid_read)dlsym(h_dll, "hid_read");
		if( gel_hid_read == NULL ){
			continue;
		}
		gel_hid_set_nonblocking =(type_hid_set_nonblocking)dlsym(h_dll, "hid_set_nonblocking");
		if( gel_hid_set_nonblocking == NULL ){
			continue;
		}
		gel_hid_send_feature_report =(type_hid_send_feature_report)dlsym(h_dll, "hid_send_feature_report");
		if( gel_hid_send_feature_report == NULL ){
			continue;
		}
		gel_hid_get_feature_report =(type_hid_get_feature_report)dlsym(h_dll, "hid_get_feature_report");
		if( gel_hid_get_feature_report == NULL ){
			continue;
		}
		gel_hid_close =(type_hid_close)dlsym(h_dll, "hid_close");
		if( gel_hid_close == NULL ){
			continue;
		}
		gel_hid_get_manufacturer_string =(type_hid_get_manufacturer_string)dlsym(h_dll, "hid_get_manufacturer_string");
		if( gel_hid_get_manufacturer_string == NULL ){
			continue;
		}
		gel_hid_get_product_string =(type_hid_get_product_string)dlsym(h_dll, "hid_get_product_string");
		if( gel_hid_get_product_string == NULL ){
			continue;
		}
		gel_hid_get_serial_number_string =(type_hid_get_serial_number_string)dlsym(h_dll, "hid_get_serial_number_string");
		if( gel_hid_get_serial_number_string == NULL ){
			continue;
		}
		gel_hid_get_indexed_string =(type_hid_get_indexed_string)dlsym(h_dll, "hid_get_indexed_string");
		if( gel_hid_get_indexed_string == NULL ){
			continue;
		}
		gel_hid_error =(type_hid_error)dlsym(h_dll, "hid_error");
		if( gel_hid_error == NULL ){
			continue;
		}

		b_error = 0;//no error

	}while(0);

	if( b_error ){
		dlclose( h_dll );
		h_dll = NULL;
	}

	return h_dll;
}

void _free_library( void *h_dll )
{
	if( h_dll )
		dlclose( h_dll );
}

void _display_usb_devices( unsigned char w_vid )
{
	struct hid_device_info *devs, *cur_dev;

	devs = gel_hid_enumerate(0x0, 0x0);
	do{
		if( devs == NULL ){
			printf( "NO device......." );
			continue;
		}

		cur_dev = devs;
		while (cur_dev) {
			if( cur_dev->vendor_id == w_vid ){
				printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
				printf("\n");
				printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
				printf("  Product:      %ls\n", cur_dev->product_string);
				printf("  Release:      %hx\n", cur_dev->release_number);
				printf("  Interface:    %d\n",  cur_dev->interface_number);
				printf("\n");
			}
			cur_dev = cur_dev->next;
		}//end while
	}while(0);
	gel_hid_free_enumeration(devs);

}

hid_device *_hid_open( unsigned short w_vid, unsigned short w_pid, int n_interface )
{
	hid_device *h_dev = NULL;

	if( n_interface < 0 ){
		//single interface device.
		h_dev = gel_hid_open(w_vid, w_pid, NULL);
	}
	else{
		h_dev = gel_hid_open_ex(w_vid, w_pid, n_interface, NULL);
	}

	return h_dev;
}

int _hid_tx( hid_device *h_dev, const unsigned char *s_tx, int n_tx )
{
	int n_txed = 0;
	int n_result = 0;
	unsigned char b_run = 0;

	do{
		if( h_dev == NULL || s_tx == NULL || n_tx < 0 ){
			n_txed = -1;
			continue;
		}
		if( n_tx == 0 ){
			continue;
		}
		//
		do{
			b_run = 0;
			n_result = gel_hid_write( h_dev, &s_tx[n_txed], n_tx );
			if( n_result < 0 ){
				printf( " error tx : %d.\n ",n_result );
				continue;
			}
			if( n_result > 0 ){
				n_txed += n_result;
				n_tx -= n_result;
			}

			printf( " tx : %d.\n ",n_result );

			if( n_tx <= 0)
				continue;//tx complete.
			//
			b_run = 1;
		}while( b_run );
		//
	}while(0);

	return n_txed;
}

int _hid_rx( hid_device *h_dev, unsigned char *s_rx, int n_rx )
{
	int n_rxed = 0;
	int n_result = 0;
	unsigned char b_run = 0;
	int n_retry = 3;

	do{
		if( h_dev == NULL || s_rx == NULL || n_rx <= 0 ){
			n_rxed = -1;
			continue;
		}
		//
		// Set the hid_read() function to be non-blocking.
		gel_hid_set_nonblocking(h_dev, 1);

		do{
			b_run = 0;

			n_result = gel_hid_read(h_dev, &s_rx[n_rxed], n_rx );
			if( n_result < 0 ){
				printf( " error rx : %d.\n ",n_result );
				continue;
			}
			if( n_result > 0 ){
				n_rxed += n_result;
				n_retry = 3;
			}
			else{
				n_retry--;
				if( n_retry <= 0)
					continue;
			}

			printf( " rx : %d.\n ",n_result );

			if( n_rxed >= n_rx )
				continue;//complete received.
			//
			#ifdef WIN32
			Sleep(10);
			#else
			usleep(10*1000);
			#endif

			b_run = 1;
		}while(b_run);

	}while(0);

	return n_rxed;
}


int main(int argc, char* argv[])
{
	int res;

	#define MAX_STR 255
	hid_device *h_dev;
	int i;
	int n_test = 0;
	unsigned char s_rx[2560] = {0,};
	unsigned char s_tx[2560] = {0,};
	unsigned char c_req = 0;
	int n_rx = 220;
	int n_tx = 1+64;

	unsigned char c_tx_report_id = 0;
	// for usb device.
	unsigned short w_vid = 0x134b;
	unsigned short w_pid = 0x0206;
	int n_interface = 1;
	char s_so_name[256] = {0,};
	//

	//load library
	void* h_dll = NULL;

	//
	for( i=0; i<argc; i++ ){
		printf( "arg[%d] - %s\n", i, argv[i] );
	}//end for

	///////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////
	//strcpy( s_so_name, ".//libhidapi.so");
	strcpy( s_so_name, ".//libhidapi-libusb.so");

	if(argc==2 ){
		printf( "=================\n" );

		if( argv[1][0] == '2' ){
			printf( " = PINPAD TEST 0x0312= \n" );
			w_pid = 0x0312;
		}
		else{
			printf( " = PINPAD TEST 0x0316= \n" );
			w_pid = 0x0316;
		}
		n_interface = -1;//single interface device
		n_tx = 1+63;
		n_rx = 320;//2112;
		c_tx_report_id = 1;
		c_req = 0x50;
	}
	else{
		printf( "=================\n" );
		printf( " = MSR TEST = \n" );
	}
	///////////////////////////////////////////////////////////////////////////////////////////

	do{
		h_dll = _load_library( s_so_name );
		if( h_dll == NULL ){
			printf("unable load library %s\n",s_so_name );
			continue;
		}
		else{
			printf("Loaded library %s\n",s_so_name );
		}

		gel_hid_init();
		//
		_display_usb_devices( w_vid );

		//continue;

		// Open the device using the VID, PID,

		h_dev = _hid_open(w_vid, w_pid, n_interface );
		if (!h_dev) {
			printf("unable to open device\n");
			gel_hid_exit();
	 		continue;
		}
		printf("OK open device\n");

		for( n_test = 0; n_test <1; n_test ++ ){

			//send data
			s_tx[0] = c_tx_report_id;
			s_tx[1] = c_req;

			res = _hid_tx( h_dev, s_tx, n_tx );
			if( res != n_tx ){
				printf("[%d] - error write %d bytes %02X, %02X -_-\n",n_test,n_tx,s_tx[0],s_tx[1] );
			}
			else{
				printf("[%d] - write %d bytes OK  %02X, %02X ^_^\n",n_test,n_tx,s_tx[0],s_tx[1]);
			}


			memset(s_rx,0,n_rx);

			res = _hid_rx( h_dev, s_rx, n_rx );
			if (res < 0)
				printf("Unable to read()\n");
			else{
				printf("Data read - %d:\n   ",res);
				// Print out the returned buffer.
				for (i = 0; i < res; i++)
					printf("%02hhx ", s_rx[i]);
				printf("\n");
			}
		}//end for

		gel_hid_close(h_dev);
		gel_hid_exit();

	}while(0);


	_free_library( h_dll );

#ifdef WIN32
	system("pause");
#endif

	return 0;
}
