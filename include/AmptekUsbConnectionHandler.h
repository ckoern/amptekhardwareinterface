#ifndef AmptekUsbConnectionHandler_h
#define AmptekUsbConnectionHandler_h

#include "AmptekConnectionHandler.h"
#include "libusb-1.0/libusb.h"
#include <mutex>

#define MAX_USB_OUT_PACKET_SIZE 520
#define MAX_USB_IN_PACKET_SIZE 32768    //defined by the amptek standard
#define PX5_USB_BULK_IN_ENDPOINT 0x81
#define PX5_USB_BULK_OUT_ENDPOINT 0x02
#define PX5_USB_INTERFACE_NUMBER 0
#define PX5_USB_VENDOR_ID 0x10C4
#define PX5_USB_PRODUCT_ID 0x842A

class AmptekUsbConnectionHandler : public AmptekConnectionHandler{
public:
    AmptekUsbConnectionHandler(int serialNb);
    virtual ~AmptekUsbConnectionHandler();
    virtual Packet sendAndReceive( const Packet& request);
    virtual void ClearCommunicationBuffer();
private:
    libusb_device_handle* usb_handle = nullptr;

    bool isAmptekPX5(libusb_device_descriptor* desc);
    bool hasSerialNb(libusb_device_handle* handle, int target_serial_nb);
    std::string libusb_errorcode(int error);

    void CloseUsbDevice(libusb_device_handle * devh);

    byte* input_buffer;
    std::mutex comm_mutex;
};

#endif