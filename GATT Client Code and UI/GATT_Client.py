import sys
sys.coinit_flags = 0 # 0 means MT


import asyncio
from bleak import BleakScanner
from bleak import BleakClient
from tkinter import *
import threading
import time

#Pico address:
addr = "2C:CF:67:E0:8A:CC"
Characteristic = "3727f058-69dd-4d78-9b6f-9c2ebbb67d44"

light_control_status = 0
fan_control_status = 0
data_logging_status = 0
reading_flag = 0
writing_flag = 0
enabled = True
bluetooth_mode=None


    # print(light_var.get())
def bluetooth_async():
    try:
        asyncio.run(bluetooth_read_loop())
        return 1
    except Exception as e:
        print(e)
        return 0

def user_interface_async():
    asyncio.run(user_interface_loop())



user_int_thread  = threading.Thread(target= user_interface_async)
bluetooth_thread  = threading.Thread(target= bluetooth_async)

def refresh_callback(lights_label,fan_label):
    global light_control_status
    global fan_control_status
    global data_logging_status

    print(f"Lights: {light_control_status}")
    print(f"Fan: {fan_control_status}")
    print(f"data: {data_logging_status}")
    if(int(light_control_status)==1):
        lights_label.config(text = "ON",fg="green")
    else:
        lights_label.config(text = "OFF",fg="red")
    if(int(fan_control_status)==1):
        fan_label.config(text = "ON",fg="green")
    else:
            fan_label.config(text = "OFF",fg="red")    # bluetooth_async(lights_label,fan_label)

def toggle_bluetooth_func():
    global enabled
    enabled = not enabled
    print(f"Enabled: {enabled}")


async def bluetooth_read_loop():
    # uuid for light 74d31fe2-71d3-41c5-aa94-f15f9f61f3c7
    # print(light_var)
    global enabled
    global light_control_status
    global fan_control_status
    global data_logging_status
    global bluetooth_mode
    async with BleakClient(addr) as client:
        while(enabled):
            reading_flag = 1
            # start = time.perf_counter()
            # for i in range (0,101):
            light_control_status        = await client.read_gatt_char("74d31fe2-71d3-41c5-aa94-f15f9f61f3c7")
            # end = time.perf_counter()
            # elapsed = end - start
            fan_control_status          = await client.read_gatt_char("74d31fe2-71d3-41c5-aa94-f15f9f61f3c6")
            data_logging_status         = await client.read_gatt_char("989dd69c-1be4-4b29-8bb4-ece912ddb61d")
            light_control_status = light_control_status.decode('utf-8')
            fan_control_status = fan_control_status.decode('utf-8')
            data_logging_status = data_logging_status.decode('utf-8')
            # print(f"Lights: {light_control_status}")
            # print(f"Fan: {fan_control_status}")
            # print(f"data: {data_logging_status}")
            # print(f"Performance {elapsed:.6f}")
            reading_flag = 0



def user_interface_loop():
    # uuid for light 74d31fe2-71d3-41c5-aa94-f15f9f61f3c7
    # print(light_var)
    root = Tk()
    root.geometry("550x550")
    root.title("Classify Card")

    light_control = IntVar()
    fan_control = IntVar()
    data_control = IntVar()

    #notify_lbl = Label(root,text="sometimes pc need time to open camera. Please wait after trigger it",font=('Helvetica', 12),foreground="black")
    #notify_lbl.grid(column = 1,row=2,padx=10,pady=10)

    fan_status_label = Label(root,text="Fan Status",font=('Arial', 16,'bold'),border=2)
    fan_status_label.grid(column=0,row=0,padx=10,pady=10)
    light_status_label = Label(root,text="Light Status",font=('Arial', 16,'bold'),border=2)
    light_status_label.grid(column=0,row=1,padx=10,pady=10)

    # current_data_capture = Button(root,text="Print Data",font=('Helvetica', 12,'bold'),state='normal',command=capture_current_data: await capture_current_data(),background="red"))
    # current_data_capture.grid(column=0,row=2,padx=10,pady=10)


    # Creating a Checkbutton
                                

    # Setting options for the Checkbutton

    #status lights
    light_status = Label(root,text="Status")
    light_status.grid(column=1,row=1,padx=10,pady=10)

    fan_status = Label(root,text="Status")
    fan_status.grid(column=1,row=0,padx=10,pady=10)


    refresh = Button(root, text="Refresh", command=lambda: refresh_callback(light_status,fan_status)) 
    refresh.config(bg="lightgrey", font=("Arial", 16), 
                    relief="raised")

    refresh.grid(column=0,row=2,padx=10,pady=10)
    
    toggle_bluetooth = Button(root, text="STOP BLUETOOTH", command=toggle_bluetooth_func) 
    toggle_bluetooth.config(bg="red", font=("Arial", 16), 
                    relief="raised")
    toggle_bluetooth.grid(column=0,row=3,padx=10,pady=10)
    
    root.mainloop()



'''
async def toggle_fan_control(fan_var):
    # uuid for light 74d31fe2-71d3-41c5-aa94-f15f9f61f3c6
    print(fan_var)
    async with BleakClient(addr) as client:
        value = await client.read_gatt_char("74d31fe2-71d3-41c5-aa94-f15f9f61f3c6")
        print(f"Value: {value}")

    
async def toggle_data_logging(data_var):
    # uuid for light 989dd69c-1be4-4b29-8bb4-ece912ddb61d
    print(data_var)
    async with BleakClient(addr) as client:
        value = await client.read_gatt_char("989dd69c-1be4-4b29-8bb4-ece912ddb61d")
        print(f"Value: {value}")


def tkinter_loop():
            
    root = Tk()
    root.geometry("550x550")
    root.title("Classify Card")

    light_control = IntVar()
    fan_control = IntVar()
    data_control = IntVar()

    #notify_lbl = Label(root,text="sometimes pc need time to open camera. Please wait after trigger it",font=('Helvetica', 12),foreground="black")
    #notify_lbl.grid(column = 1,row=2,padx=10,pady=10)

    fan_control_rad = Checkbutton(root,text="Light Control",font=('Arial', 16,'bold'),relief="raised",bg='lightgrey',variable=light_control,command=lambda: toggle_light_control(light_control.get()))
    fan_control_rad.grid(column=0,row=0,padx=10,pady=10)
    light_control_rad = Checkbutton(root,text="Fan Control",font=('Arial', 16,'bold'),relief="raised",bg='lightgrey',variable=fan_control,command=lambda: toggle_fan_control(fan_control.get()))
    light_control_rad.grid(column=0,row=1,padx=10,pady=10)

    # current_data_capture = Button(root,text="Print Data",font=('Helvetica', 12,'bold'),state='normal',command=capture_current_data: await capture_current_data(),background="red"))
    # current_data_capture.grid(column=0,row=2,padx=10,pady=10)


    # Creating a Checkbutton
    enable_data_logging = Checkbutton(root, text="Enable Data Logging", variable=data_control, 
                                onvalue=1, offvalue=0,command=lambda toggle_data_logging: toggle_data_logging(fan_control.get()))

    # Setting options for the Checkbutton
    enable_data_logging.config(bg="lightgrey", font=("Arial", 16), 
                    relief="raised")

    enable_data_logging.grid(column=0,row=2,padx=10,pady=10)

    root.mainloop()
'''

if __name__ == '__main__':
    bluetooth_thread.start()
    user_interface_loop()
    