from tkinter import *
import GATT_Client as client
import asyncio

#NOTE: CLIENT INTERFACE capture_current_data() doesn't work. I will look into this

def capture_current_data():
    client.start_receive()


root = Tk()
root.geometry("550x550")
root.title("GATT Client")



fan_control_rad = Checkbutton(root,text="Fan Control",font=('Helvetica', 12,'bold'))
fan_control_rad.grid(column=1,row=1,padx=10,pady=10)
light_control_rad = Checkbutton(root,text="Light Control",font=('Helvetica', 12,'bold'),state='normal')
light_control_rad.grid(column=0,row=1,padx=10,pady=10)

current_data_capture = Button(root,text="Print Data",font=('Helvetica', 12,'bold'),state='normal',command=capture_current_data,background="red")
current_data_capture.grid(column=0,row=2,padx=10,pady=10)

root.mainloop()
