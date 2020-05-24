#include "AmptekUsbConnectionHandler.h"

#include <iostream>

AmptekUsbConnectionHandler::AmptekUsbConnectionHandler(int serialNb){

    input_buffer = new byte[ MAX_USB_IN_PACKET_SIZE ];

    libusb_device **devs;
    libusb_device_handle* tmp_handle;
	int r;
	ssize_t cnt;
    r = libusb_init(NULL);
	if (r < 0)
		throw AmptekException("Cannot init libusb");

	cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0){
		libusb_exit(NULL);
		throw AmptekException("Cannot get libusb device list");
	}

    int j = 0;
    for (int i = 0; i < cnt; ++i){
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(devs[i], &desc);
		if (r < 0) {
			std::cerr << "failed to get device descriptor: " << libusb_errorcode(r) << std::endl;
			continue;
		}
        std::cout << "Test dev " << std::hex  << desc.idVendor << "-" << desc.idProduct << std::dec  << std::endl;
        if (isAmptekPX5(&desc)){
            tmp_handle = nullptr;
            r = libusb_open(devs[i], &tmp_handle);
            if (r < 0) {
                std::cerr << "failed opening usb connection: " << libusb_errorcode(r) << std::endl;
                continue;
            }
            if (tmp_handle != nullptr){
                r = libusb_claim_interface(tmp_handle, PX5_USB_INTERFACE_NUMBER);
                if (r < 0) {
                    std::cerr << "failed claiming interface: " << libusb_errorcode(r) << std::endl;
                    libusb_close(tmp_handle);
                    continue;
                }
                try{
                    if ( hasSerialNb( tmp_handle, serialNb ) ){
                        std::cout << "YES" << std::endl;
                        usb_handle = tmp_handle;
                        break;
                    }else{
                        std::cout << "NO" << std::endl;
                    }
                }
                catch (AmptekException& e){
                     std::cerr << "failed checking Serial Nb: " << e.what() << std::endl;
                }
                //if this point is reached, either hasSerialNb was false, or has thrown an exception
                try{
                    CloseUsbDevice(tmp_handle);
                }
                catch (AmptekException& e){
                     std::cerr << "failed closing device: " << e.what() << std::endl;
                }
            }
        }
    }
    libusb_free_device_list(devs, 1);
    
    
    if (usb_handle == nullptr){
        throw AmptekException("Cannot connect to the Amptek PX5 device with the given serial number");
    }
    

}


AmptekUsbConnectionHandler::~AmptekUsbConnectionHandler(){
        std::cout << "in AmptekUsbConnectionHandler::~AmptekUsbConnectionHandler()" << std::endl;

    if (usb_handle != nullptr){
        try{
            CloseUsbDevice(usb_handle);
        }
        catch (AmptekException& e){
            std::cerr << "Error in AmptekUsbConnectionHandler::~AmptekUsbConnectionHandler: " << e.what() << std::endl;
        }
    }
    libusb_exit(NULL);

    delete[] input_buffer;
}

Packet AmptekUsbConnectionHandler::sendAndReceive( const Packet& request){
    
    if ( request.size() > MAX_USB_OUT_PACKET_SIZE ){
        throw AmptekException( "Cannot send Packets larger than "+ std::to_string(MAX_USB_OUT_PACKET_SIZE) + " bytes" );
    }
    std::lock_guard<std::mutex> comm_lock(comm_mutex);
    int bytes_transferred;
    int result = libusb_bulk_transfer(usb_handle, PX5_USB_BULK_OUT_ENDPOINT, (byte*) &(request.at(0)), request.size(), &bytes_transferred, 1000);
    std::string error_msg;
    if (result ==0){
        result = libusb_bulk_transfer(usb_handle, PX5_USB_BULK_IN_ENDPOINT, input_buffer, MAX_USB_IN_PACKET_SIZE, &bytes_transferred, 1000);
        if (result == 0){
            Packet p;
            p.resize( bytes_transferred);
            p.fromByteArray(input_buffer);
            return p;
        }else{
            error_msg = "Failed during status Response: " + libusb_errorcode(result);
        }
    }else{
        error_msg = "Failed during status Request: " + libusb_errorcode(result);
    }
    throw AmptekException(error_msg);
}

void AmptekUsbConnectionHandler::ClearCommunicationBuffer(){

}

bool AmptekUsbConnectionHandler::isAmptekPX5(libusb_device_descriptor* desc){
    return (desc->idVendor == PX5_USB_VENDOR_ID) && (desc->idProduct == PX5_USB_PRODUCT_ID);
}
bool AmptekUsbConnectionHandler::hasSerialNb(libusb_device_handle* handle, int target_serial_nb){
    std::cout << "Testing connected Amptek Device for Serial Nb "<< target_serial_nb << "..." << std::endl;
    if (target_serial_nb < 0){
        return true;
    }
    //request a status and check the serial nb 
    Packet statusReq = Packet::DP5_PKT_REQUEST_STATUS;
    int bytes_transferred;
    byte* status_data = input_buffer + DATA;
    int result = libusb_bulk_transfer(handle, PX5_USB_BULK_OUT_ENDPOINT, &(statusReq.at(0)), statusReq.size(), &bytes_transferred, 1000);
    std::string error_msg;
    if (result ==0){
        result = libusb_bulk_transfer(handle, PX5_USB_BULK_IN_ENDPOINT, input_buffer, 100, &bytes_transferred, 1000);
        if (result == 0){
            int serialNb = mergeBytes(status_data[29],status_data[28],status_data[27],status_data[26]);
            std::cout << "Device has Serial Nb " << serialNb << std::endl;
            return serialNb == target_serial_nb;
        }else{
            error_msg = "Failed during status Response: " + libusb_errorcode(result);
        }
    }else{
        error_msg = "Failed during status Request: " + libusb_errorcode(result);
    }
    throw AmptekException(error_msg);
}

void AmptekUsbConnectionHandler::CloseUsbDevice(libusb_device_handle * devh){

    int r;
	if (devh != NULL) {
		r = libusb_release_interface(devh, PX5_USB_INTERFACE_NUMBER);
		if (r < 0) {
			throw AmptekException("libusb_release_interface error: "+ libusb_errorcode(r));
		}
		libusb_close(devh);
	}
}


std::string AmptekUsbConnectionHandler::libusb_errorcode(int error_code){
    switch (error_code) {
	case LIBUSB_SUCCESS:
		return "Success";
	case LIBUSB_ERROR_IO:
		return "Input/output error";
	case LIBUSB_ERROR_INVALID_PARAM:
		return "Invalid parameter";
	case LIBUSB_ERROR_ACCESS:
		return "Access denied (insufficient permissions)";
	case LIBUSB_ERROR_NO_DEVICE:
		return "No such device (it may have been disconnected)";
	case LIBUSB_ERROR_NOT_FOUND:
		return "Entity not found";
	case LIBUSB_ERROR_BUSY:
		return "Resource busy";
	case LIBUSB_ERROR_TIMEOUT:
		return "Operation timed out";
	case LIBUSB_ERROR_OVERFLOW:
		return "Overflow";
	case LIBUSB_ERROR_PIPE:
		return "Pipe error";
	case LIBUSB_ERROR_INTERRUPTED:
		return "System call interrupted (perhaps due to signal)";
	case LIBUSB_ERROR_NO_MEM:
		return "Insufficient memory";
	case LIBUSB_ERROR_NOT_SUPPORTED:
		return "Operation not supported or unimplemented on this platform";
	case LIBUSB_ERROR_OTHER:
		return "Other error";
	}
	return "Unknown error";
}


