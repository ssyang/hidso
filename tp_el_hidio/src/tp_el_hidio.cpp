#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

#include "el_hidapi.h"
#include <dlfcn.h>//for dynamic loading

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


/////////////////////////////////////////////////////////
// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

int main(int argc, char* argv[])
{
	int res;

	#define MAX_STR 255
	wchar_t wstr[MAX_STR];
	hid_device *h_dev;
	int i;
	int n_wait = 0;
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
#ifdef WIN32
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#endif


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
		printf( " = PINPAD TEST = \n" );
		w_pid = 0x0316;
		n_interface = 0;
		n_tx = 1+319;
		n_rx = 2112;
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
			continue;
		}
		//
		struct hid_device_info *devs, *cur_dev;

		devs = gel_hid_enumerate(0x0, 0x0);
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
		}
		gel_hid_free_enumeration(devs);

		// Open the device using the VID, PID,
		// and optionally the Serial number.
		////handle = hid_open(0x4d8, 0x3f, L"12345");
		if( argc==2 ){
			h_dev = gel_hid_open(w_vid, w_pid, NULL);
		}
		else{
			h_dev = gel_hid_open_ex(w_vid, w_pid, n_interface, NULL);
		}
		if (!h_dev) {
			printf("unable to open device\n");
	 		continue;
		}

		for( n_test = 0; n_test <10; n_test ++ ){
			//send data
			s_tx[0] = c_tx_report_id;
			s_tx[1] = c_req;
			res = gel_hid_write( h_dev, s_tx, n_tx );
			if( res != n_tx ){
				printf("[%d] - error write %d bytes %02X, %02X -_-\n",n_test,n_tx,s_tx[0],s_tx[1] );
			}
			else{
				printf("[%d] - write %d bytes OK  %02X, %02X ^_^\n",n_test,n_tx,s_tx[0],s_tx[1]);
			}

			// Set the hid_read() function to be non-blocking.
			gel_hid_set_nonblocking(h_dev, 1);

			memset(s_rx,0,n_rx);

			// Read requested state. hid_read() has been set to be
			// non-blocking by the call to hid_set_nonblocking() above.
			// This loop demonstrates the non-blocking nature of hid_read().
			n_wait = 0;
			res = 0;
			while (res == 0) {
				res = gel_hid_read(h_dev, s_rx, n_rx );
				if (res == 0)
					printf("waiting...\n");
				if (res < 0)
					printf("Unable to read()\n");
				#ifdef WIN32
				Sleep(100);
				#else
				usleep(100*1000);
				#endif

				n_wait++;
				if( n_wait > 10 ){
					gel_hid_close(h_dev);
					gel_hid_exit();
					_free_library( h_dll );
					return 1;
				}
			}

			printf("Data read - %d:\n   ",res);
			// Print out the returned buffer.
			for (i = 0; i < res; i++)
				printf("%02hhx ", s_rx[i]);
			printf("\n");
		}//end for

		//
		gel_hid_close(h_dev);

		/* Free static HIDAPI objects. */
		gel_hid_exit();

	}while(0);

	_free_library( h_dll );

#ifdef WIN32
	system("pause");
#endif

	return 0;
}
