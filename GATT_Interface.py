import asyncio
from bleak import BleakScanner
from bleak import BleakClient
from tkinter import *
import threading
import time


#Pico address:
addr = "2C:CF:67:E0:8A:CC"
UUID_Lights = "74d31fe2-71d3-41c5-aa94-f15f9f61f3c6"
UUID_Fan = "74d31fe2-71d3-41c5-aa94-f15f9f61f3c7"
UUID_Fan_CTRL = "74d31fe2-71d3-41c5-aa94-f15f9f61f3c5"
#74d31fe2-71d3-41c5-aa94-f15f9f61f3c4
UUID_Lights_CTRL = "74d31fe2-71d3-41c5-aa94-f15f9f61f3c4"
# UUID_Fan = 0x000d
UUID_Current_Data = "3727f058-69dd-4d78-9b6f-9c2ebbb67d44"



light_control_status = 0
fan_control_status = 0
data_logging = 0
reading_flag = 0
writing_flag = 0
enabled = True
bluetooth_mode=None

def fan_callback(sender,data):
    print(data)


async def bluetooth_read():
    # uuid for light 74d31fe2-71d3-41c5-aa94-f15f9f61f3c7
    # print(light_var)
    # global enabled
    global light_control_status
    global fan_control_status
    global data_logging
    global data_logging_light
    # global bluetooth_mode
    # global writing_flag
    async with BleakClient(addr) as client:
        # await client.start_notify(UUID_Fan,fan_callback)
        fan_control_status = await client.read_gatt_char(UUID_Fan_CTRL)
        light_control_status = await client.read_gatt_char(UUID_Lights_CTRL)
        data_logging = await client.read_gatt_char(UUID_Current_Data)
        data_logging_light = await client.read_gatt_char(UUID_Lights)
        # await client.stop_notify()

        # fan_control_status = await client.read_gatt_char(UUID_Lights)
        # print(f"Fan:{fan_control_status}")
        # print(f"Lights:{light_control_status}")
        # print(f"Current:{data_logging}")
        print("--------------STATUS OF SYSTEM---------------")
        print(f"Current from Fan: {data_logging.decode('utf-8')} mA")
        # print(f"Current from Light: {data_logging_light.decode('utf-8')} mA")

        print(f"Component Status: {fan_control_status.decode('utf-8')}")
        print("---------------------------------------------")

async def bluetooth_write(characteristic,value):
    if(characteristic=="FAN"):
        async with BleakClient(addr) as client:
            # await asyncio.sleep(10.0)
            # await client.start_notify(UUID_Fan,fan_callback)

            await client.write_gatt_char(UUID_Fan_CTRL,value.encode(),False)





async def main():
    done = False
    while(not done):
        user_input = input("Enter the command to the system >> ")
        if(user_input == "STATUS"):
            try:
                await bluetooth_read()
                # print(f"Current from Fan: {data_logging.decode('utf-8')}")
                # print(f"Fan Status: {fan_control_status.decode('utf-8')}")
                # print(f"Light Status: {light_control_status.decode('utf-8')}")

            except Exception as e:
                try:
                    await bluetooth_read()
                except Exception as e:
                    print(e)
                    print("ERROR: TIMEOUT")
                    print("TRY COMMAND AGAIN")


        elif(user_input == "WRITE"):
            write_input = input("Enter the characteristic to write to (FAN,LIGHT) >> ")
            if(write_input == "FAN"):
                value_input = input("Enter the value to write to (1 or 0) >> ")
                if(value_input=='1'):
                    await bluetooth_write(characteristic="FAN",value='1')
                    await asyncio.sleep(1.0)
                elif(value_input=='0'):
                    await bluetooth_write(characteristic="FAN",value='0')
                    await asyncio.sleep(1.0)
                else:
                    print("USER ERROR: Input a '0' or '1'")
            elif(write_input == "LIGHT"):
                value_input = input("Enter the value to write to (1 or 0) >> ")
                if(value_input=='1'):
                    await bluetooth_write(characteristic="LIGHT",value='1')
                    await asyncio.sleep(1.0)
                elif(value_input=='0'):
                    await bluetooth_write(characteristic="LIGHT",value='0')
                    await asyncio.sleep(1.0)
                else:
                    print("USER ERROR: Input a '0' or '1'")  
        elif(user_input=="QUIT"):
            print("EXITING PROGRAM")
            done=True
               
        else:
            print("USER ERROR: VALID COMMANDS")

            
    # await asyncio.sleep(2.0)
    # await bluetooth_write()
    # await asyncio.sleep(2.0)
    # await bluetooth_read()

if __name__ == "__main__":
    asyncio.run(main())