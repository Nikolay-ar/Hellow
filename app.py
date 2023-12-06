import ctypes
import enum
import numpy as np
from skimage.io import imsave

# This Python file uses the following encoding: utf-8

if __name__ == "__main__":
    print("heoo")

class GigEVision_Status(enum.IntEnum):
    GIGEVISION_STATUS_UNKNOWN = -1,
    GIGEVISION_STATUS_FAILED  =  0,
    GIGEVISION_STATUS_OK      =  1


class GigEVision_Context_t(ctypes.Structure):  # Задаем структуру
    _fields_ = [("Device", ctypes.c_int32),                     # 4   байта
                ("Address_Source", (ctypes.c_uint8*16)),        # 16  байт
                ("Address_Destination", (ctypes.c_char*16)),    # 16  байт
                ("Empty", (ctypes.c_uint8*574))                 # 574 байта
                ]

# Подключение DLL
libGigEVision_dll = ctypes.cdll.LoadLibrary("./libGigEVision.so")
GigEVision_Open = libGigEVision_dll.GigEVision_Open     # Подключение камеры
GigEVision_Open.restype = ctypes.c_int32                # GigEVision_Status_t

GigEVision_Set_AcquisitionMode = libGigEVision_dll.GigEVision_Set_AcquisitionMode
GigEVision_Set_AcquisitionMode.restype = ctypes.c_int32

GigEVision_Get_SingleFrame = libGigEVision_dll.GigEVision_Get_SingleFrame
GigEVision_Get_SingleFrame.restype = ctypes.c_int32

GigEVision_Close = libGigEVision_dll.GigEVision_Close
GigEVision_Close.restype = ctypes.c_int32


Context = GigEVision_Context_t()
pContext = ctypes.pointer(Context)

Status = GigEVision_Open(  1,          #   Device
                        b"172.18.16.23",        # Address_Source
                        b"172.18.16.24",        # Address_Destination
                        0,          #   GIGEVISION_SOCKET_READ_TIMEOUT_CONTROL_SECONDS
                        250000,     #   GIGEVISION_SOCKET_READ_TIMEOUT_CONTROL_MICROSECONDS
                        0,          #   GIGEVISION_SOCKET_READ_TIMEOUT_STREAM_SECONDS
                        250000,     #   GIGEVISION_SOCKET_READ_TIMEOUT_STREAM_MICROSECONDS
                        1048576,    #   GIGEVISION_SOCKET_BUFFER_SIZE_CONTROL
                        1048576,    #   GIGEVISION_SOCKET_BUFFER_SIZE_STREAM
                        1,          #   GIGEVISION_SOCKET_POLL_EMPTY_CYCLES_MAXIMUM
                        pContext     # Context
)

if Status == GigEVision_Status.GIGEVISION_STATUS_OK:
    print("GigEVision_Open OK")
else:
    print("GigEVision_Open Error")
    exit()

Status = GigEVision_Set_AcquisitionMode(pContext, 1)
if Status == GigEVision_Status.GIGEVISION_STATUS_OK:
    print("GigEVision_Set_AcquisitionMode OK")
else:
    print("GigEVision_Set_AcquisitionMode Error")
    exit()
Payload_Size = ctypes.c_ulong(0)
Image_BayerRG8_2064x1544 = (ctypes.c_uint8*3186816)(0)

Status = GigEVision_Get_SingleFrame (pContext, 
                                     3186816, 
                                     Image_BayerRG8_2064x1544,
                                     ctypes.pointer(Payload_Size),
                                     0,
                                     0
                                       )

if Status == GigEVision_Status.GIGEVISION_STATUS_OK:
    print("GigEVision_Get_SingleFrame OK")
else:
    print("GigEVision_Get_SingleFrame Error")
    exit()


# Width and height of Bayer image, not original which is w/2 x h/2      Ширина и высота изображения Bayer, не оригинального, которое составляет w/2 x h/2
w, h = 2064, 1544
ow, oh = w//2, h//2

# Load in Bayer8 image, assumed raw 8-bit RGGB                          Загружается в изображение Bayer8, предполагается необработанный 8-битный RGGB
# bayer = np.fromfile('./Data/GigEVision_Test.rg8', dtype=np.uint8).reshape((h,w))

bayer = np.ctypeslib.as_array(Image_BayerRG8_2064x1544).reshape((h,w))     # Картинка из указателя

# Pick up raw uint8 samples                                             Возьмите необработанные образцы uint8
R  = bayer[0::2, 0::2]     # rows 0,2,4,6 columns 0,2,4,6
G0 = bayer[0::2, 1::2]     # rows 0,2,4,6 columns 1,3,5,7
G1 = bayer[1::2, 0::2]     # rows 1,3,5,7 columns 0,2,4,6
B  = bayer[1::2, 1::2]     # rows 1,3,5,7 columns 1,3,5,7

# Chop any left-over edges and average the 2 Green values               Обрежьте все оставшиеся края и усредните 2 зеленых значения
R = R[:oh,:ow]
G = G0[:oh,:ow]//2 + G1[:oh,:ow]//2
B = B[:oh,:ow]

# Formulate image by stacking R, G and B and save                       Сформулируйте изображение, сложив R, G и B, и сохраните
out = np.dstack((R,G,B))
imsave('./Data/GigEVision_Test.interpolated-py.bmp',out)


for i in range(10000):
    print(Image_BayerRG8_2064x1544[i])

Status = GigEVision_Close( ctypes.pointer(Context) )
if Status == GigEVision_Status.GIGEVISION_STATUS_OK:
    print("GigEVision_Close OK")
else:
    print("GigEVision_Close Error")
    exit()
