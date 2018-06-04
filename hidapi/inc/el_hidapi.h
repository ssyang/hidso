/*
 * el_hidapi.h
 *
 *  Created on: 2018. 6. 4.
 *      Author: u
 *
 *  for dynamic loading libhidapi.so
 *
 *  notice
 *  Don't use with hidapi.h
 */

#ifndef INC_EL_HIDAPI_H_
#define INC_EL_HIDAPI_H_


#include <wchar.h>

#ifdef _WIN32
      #define HID_API_EXPORT __declspec(dllexport)
      #define HID_API_CALL
#else
      #define HID_API_EXPORT /**< API export macro */
      #define HID_API_CALL /**< API call macro */
#endif

#define HID_API_EXPORT_CALL HID_API_EXPORT HID_API_CALL /**< API export and call macro*/

#ifdef __cplusplus
extern "C" {
#endif
		struct hid_device_;
		typedef struct hid_device_ hid_device; /**< opaque hidapi structure */

		/** hidapi info structure */
		struct hid_device_info {
			/** Platform-specific device path */
			char *path;
			/** Device Vendor ID */
			unsigned short vendor_id;
			/** Device Product ID */
			unsigned short product_id;
			/** Serial Number */
			wchar_t *serial_number;
			/** Device Release Number in binary-coded decimal,
			    also known as Device Version Number */
			unsigned short release_number;
			/** Manufacturer String */
			wchar_t *manufacturer_string;
			/** Product string */
			wchar_t *product_string;
			/** Usage Page for this Device/Interface
			    (Windows/Mac only). */
			unsigned short usage_page;
			/** Usage for this Device/Interface
			    (Windows/Mac only).*/
			unsigned short usage;
			/** The USB interface which this logical device
			    represents. Valid on both Linux implementations
			    in all cases, and valid on the Windows implementation
			    only if the device contains more than one interface. */
			int interface_number;

			/** Pointer to the next device */
			struct hid_device_info *next;
		};

		/////////////////////////////////////////////////////////
		// exported function prototype
		// dlopen()등을 사용하고 싶다면, libdl을 링크, 즉. -ldl 옵션을 붙여서 컴파일 해야 한다.
		// 또한 dl 라이브러리를 링크한다.
		// 관리자 권한으로 실행해야 함.

		typedef int (*type_hid_init)(void);
		typedef int (*type_hid_exit)(void);
		typedef struct hid_device_info *(*type_hid_enumerate)(unsigned short vendor_id, unsigned short product_id);
		typedef void (*type_hid_free_enumeration)(struct hid_device_info *devs);
		typedef hid_device *(*type_hid_open_ex)(unsigned short vendor_id, unsigned short product_id, int interface_number, wchar_t *serial_number);
		typedef hid_device *(*type_hid_open)(unsigned short vendor_id, unsigned short product_id, wchar_t *serial_number);
		typedef hid_device *(*type_hid_open_path)(const char *path);
		typedef int (*type_hid_write)(hid_device *device, const unsigned char *data, size_t length);
		typedef int (*type_hid_read_timeout)(hid_device *dev, unsigned char *data, size_t length, int milliseconds);
		typedef int (*type_hid_read)(hid_device *device, unsigned char *data, size_t length);
		typedef int (*type_hid_set_nonblocking)(hid_device *device, int nonblock);
		typedef int (*type_hid_send_feature_report)(hid_device *device, const unsigned char *data, size_t length);
		typedef int (*type_hid_get_feature_report)(hid_device *device, unsigned char *data, size_t length);
		typedef void (*type_hid_close)(hid_device *device);
		typedef int (*type_hid_get_manufacturer_string)(hid_device *device, wchar_t *string, size_t maxlen);
		typedef int (*type_hid_get_product_string)(hid_device *device, wchar_t *string, size_t maxlen);
		typedef int (*type_hid_get_serial_number_string)(hid_device *device, wchar_t *string, size_t maxlen);
		typedef int (*type_hid_get_indexed_string)(hid_device *device, int string_index, wchar_t *string, size_t maxlen);
		typedef const wchar_t* (*type_hid_error)(hid_device *device);


#ifdef __cplusplus
}
#endif


#endif /* INC_EL_HIDAPI_H_ */
