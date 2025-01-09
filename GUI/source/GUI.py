import tkinter as tk
import threading
import queue
import concurrent.futures
from enum import Enum
import time
from pythonnet import load
load("coreclr")
import clr

## Main thread: GUI
## Sub thread: Data exchange through TCP

# Import C# Library
clr.AddReference(r"C:\Users\Victor Yeung\Desktop\Coding\Mech541_Project\Autonomous_RC_Truck\Comms\gui\TcpServerLibrary\bin\Debug\net8.0\TcpServerLibrary.dll")
from TcpServerLibrary import TcpServer

# Use to exchange data between main and sub thread
data_queue = queue.Queue()
DataToSend = [[10,20,30],[40,50,60],[70,80,90]]

##################################
# If program stucks at listening, it mostly due to wrong IP address
##################################

class ActionFlag(Enum):
    Send = 1
    Receive = 2
    Idle = 3
    Disconnect = 4
actionflag = ActionFlag.Idle

actionlock = threading.Lock()

# Create Server Connection
def ServerSetup():
    print("Initializing TCP Server...")
    tcp_server = TcpServer()
    print("Starting server...")
    stream = tcp_server.StartServer()
    print("Server started successfully.")
    return stream

# 2 way handshake mechanism 
def ReceivePermission(stream):
    global actionflag
    try:
        Permission = TcpServer.HandShakeSYN(stream)
        if Permission == True:
            actionflag = ActionFlag.Receive
            print("ESP signal received, setting actionflag to Receive.")
            return True
        else:
            # actionflag = ActionFlag.Idle
            print("No valid signal received.")
            return False
        
    except Exception as e:
        print(f"Error receiving permission: {e}")
        return False

# Receive Data
def ReceiveAllData(stream):
    try:
        # Receive the number of data
        NumOfDataSet = int(TcpServer.ReceiveData(stream))
        print(f"{NumOfDataSet} set of data will be received")
        for x in range (int(NumOfDataSet)):
            data = TcpServer.ReceiveDataSet(stream)    
            if data != 0:
                # Place the data in the queue for GUI update
                python_list = convert_to_python_list(data)
                data_queue.put(python_list)
            elif x == int(NumOfDataSet):
                break
            else:
                break
        return 0 
        
    except Exception as e:
        print(f"Error receiving data: {e}")
        return 1

# Send Data
def SendAllData(stream,DataToSend):
    try:
        if TcpServer.SendSYN(stream) == True:
            NumOfDataSet = len(DataToSend)
            print(f"{NumOfDataSet} set of data will be sent")
            for x in range(NumOfDataSet):
                TcpServer.SendDataSet(stream,DataToSend[x])
                print(f"Sending set {x + 1}: {DataToSend[x]}")
            return 0
        
    except Exception as e:
        print(f"Error sending data: {e}")
        return 1

# Thread main()
def ThreadMain(stream, DataToSend):
    global actionflag, actionlock
    while True:
        try:    
            with actionlock:
                print(f"Current status is :{actionflag}")
                if actionflag == ActionFlag.Receive:
                    print("ActionFlag set to Receive, receiving data...")
                    ReceiveAllData(stream)
                    TcpServer.RespondClient(stream)
                    actionflag = ActionFlag.Idle
                    print("Returning ActionFlag to Idle.")

                elif actionflag == ActionFlag.Send:
                    print("ActionFlag set to Send, sending data...")
                    SendAllData(stream, DataToSend)
                    actionflag = ActionFlag.Idle
                    print("Returning ActionFlag to Idle.")

            # Check Change of Send/Receive Flag
            # Outside the lock to avoid blocking
            if actionflag == ActionFlag.Idle:
                if ReceivePermission(stream) == True:
                    with actionlock:
                        actionflag = ActionFlag.Receive
                        print("Receive Permission granted, setting actionflag to Receive.")
                # elif SendPermission(stream) == True:
                #     with actionlock:
                #         actionflag = ActionFlag.Send
                #         print("Send Permission granted, setting actionflag to Send.")
                else:
                    # print("No permission received, staying in Idle.")
                    print("No permission received")

        except Exception as e:
            print(f"ThreadMain: Exception encountered: {e}")
            break  # Exit the loop on exception

# Thread Initiate
def ClientHandler(stream):
    if stream:
        thread1 = threading.Thread(target=ThreadMain, args=(stream,DataToSend))
        thread1.daemon = True
        thread1.start()

# Convert C# double[] to Python List
def convert_to_python_list(csharp_array):
    return [float(i) for i in csharp_array]  # Convert each element to float

# GUI SETUP
class App(tk.Tk):
    def __init__(self):
      super().__init__()
      self.title("TCP Server GUI")
      self.label = tk.Label(self, text="Waiting for data...", font=("Helvetica", 16))
      self.label.pack(padx=20, pady=20)

      # Button to trigger an action
      self.button = tk.Button(self, text="Send Data", command=self.on_button_click)
      self.button.pack(padx=20, pady=10)
    
    def update_gui(self):
        # This method should handle GUI updates, calling it periodically
        if not data_queue.empty():
            data = data_queue.get()
            # print(f"Data received: {data}")  # For now, just print received data
            self.label.config(text=f"Data received: {data}")
        self.after(100, self.update_gui)  # Update every second
    
    def on_button_click(self):
        global DataToSend, actionflag
        # arr = [[10,20,30],[40,50,60],[70,80,90]]
        print("Button Clicked")
        with actionlock:
            actionflag = ActionFlag.Send
            # DataToSend = arr
            print(f"actionflag set to {actionflag}")

       
# main()
if __name__ == "__main__":
    try: 
        stream = ServerSetup()
        # Communication Thread
        ClientHandler(stream)

        # GUI Thread
        app = App()
        app.update_gui()  # Start the GUI update loop
        app.mainloop()  # Keep the main GUI loop running

    except KeyboardInterrupt:
        print("Shutting Down.")





