import time
from pySerialTransfer import pySerialTransfer as txfer
import base64
def chunks(lst, n):
    """Yield successive n-sized chunks from lst."""
    for i in range(0, len(lst), n):
        yield lst[i:i + n]
if __name__ == '__main__':
    try:
        link = txfer.SerialTransfer('/dev/cu.usbmodem13401') #Windows에서는 COM1 등에 해당함
        raw = base64.b64encode(open('/Users/leejunhyuk/Desktop/XHWnX.png', 'rb').read()).decode('ascii')
        print(len(raw))
        bytes = list(chunks(raw, 60)) #QR 코드 예시
        
        link.open()
        time.sleep(2) # 미리 초기화해 둠
        index = 0
        while True:
            send_size = 0
            list_ = bytes[index]
            list_size = link.tx_obj(bytes[index])
            send_size += len(bytes[index])
            link.send(send_size)
            
            # 대기 하면서 오류 점검
            while not link.available():
                if link.status < 0:
                    if link.status == txfer.CRC_ERROR:
                        print('ERROR: CRC_ERROR')
                    elif link.status == txfer.PAYLOAD_ERROR:
                        print('ERROR: PAYLOAD_ERROR')
                    elif link.status == txfer.STOP_BYTE_ERROR:
                        print('ERROR: STOP_BYTE_ERROR')
                    else:
                        print('ERROR: {}'.format(link.status))
            # 응답 해석
            rec_list_  = link.rx_obj(obj_type=type(list_),
                                     obj_byte_size=3,
                                     list_format='i')
            print('SENT: {} '.format(list_))
            print('RCVD: {}'.format(rec_list_))
            print(' ')
            index += 1
            """if list_ == rec_list_:
                if index == len(bytes) - 1:
                    ###################################################################
                    # Send a string
                    ###################################################################
                    str_ = 'ENDSEND'
                    str_size = link.tx_obj(str_)
                    send_size += str_size
                    
                    ###################################################################
                    # Transmit all the data to send in a single packet
                    ###################################################################
                    link.send(send_size)
                    
                    ###################################################################
                    # Wait for a response and report any errors while receiving packets
                    ###################################################################
                    while not link.available():
                        if link.status < 0:
                            if link.status == txfer.CRC_ERROR:
                                print('ERROR: CRC_ERROR')
                            elif link.status == txfer.PAYLOAD_ERROR:
                                print('ERROR: PAYLOAD_ERROR')
                            elif link.status == txfer.STOP_BYTE_ERROR:
                                print('ERROR: STOP_BYTE_ERROR')
                            else:
                                print('ERROR: {}'.format(link.status))
                    ###################################################################
                    # Parse response string
                    ###################################################################
                    rec_str_   = link.rx_obj(obj_type=type(str_),
                                            obj_byte_size=str_size)
                    
                    ###################################################################
                    # Display the received data
                    ###################################################################
                    print('SENT: {}'.format(str_))
                    print('RCVD: {}'.format(rec_str_))
                    print(link.rxBuff)
                    print(' ')
                    break
                else:
                    index += 1
            else:
                continue
              """
    
    except KeyboardInterrupt:
        try:
            link.close()
        except:
            pass
    
    except:
        import traceback
        traceback.print_exc()
        
        try:
            link.close()
        except:
            pass