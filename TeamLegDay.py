import matplotlib.pyplot as plt
import numpy as np
import serial
from matplotlib.animation import FuncAnimation
from matplotlib.colors import LinearSegmentedColormap
import seaborn as sns; sns.set()
import threading
import tkinter as tk
import pandas as pd 
import csv


dimension = (19, 16)#setting matrix size (final will be 19 by 14)
heatmap_matrix = np.zeros(dimension) #sets heatmap_matrix to numpy array of 0's
aRandomArray = np.random.random((16, 16)) #Using this test writing to .csv - not functional
timer = 0 #using to test plt.clf() - not functional

#function below creates custom color palette, colors are in HEX. E.g. 'RED' = #FF0000
def get_custom_color_palette(): 
    return LinearSegmentedColormap.from_list("", [
        '#06467F', '#2566A0', '#508EC6',
        '#75C2DF',
        '#75D7DF', '#75DFDD', '#75DFC6', '#FF0000'
    ])
    
cmap = get_custom_color_palette() #setting cmap for seaborn to custom color palette

def setup():
    sns.heatmap(np.zeros(dimension), cbar=False) #seaborn heatmap initial - np.zeros(dimensions) fill numpy array with 0's


def readSerial():
    arduinoData = serial.Serial('COM4', 115200) #reading data from Arduino serial, change according to your PC/Mac ports
    while(True):
        dataPacket = arduinoData.readline().decode().strip().split('\t') #reading arduino serial data line by line and stripping of unused characters
        try:
            rowIndex = int(dataPacket[0]) #
            for j in range(dimension[1]-1, -1, -1): 
                heatmap_matrix[rowIndex][j] = int(dataPacket[j+1])
        except: #pass if "[]" is present. Without this, visualization will crash after some time.
            pass


#Heatmap visualization
def visualize(index):
    sns.heatmap(heatmap_matrix, cmap=cmap, cbar=False, vmin=0.0, vmax= 3700.0) #vmin and vmax here are tied to arduino serial data: z axis
   

#Refresh heatmap button - Goal is to have this button automated every 10 seconds
def refresh_func():
    plt.clf()
    print("cache cleared")
    sns.heatmap(heatmap_matrix, cmap=cmap, cbar=True, vmin=0.0, vmax=75.0) #vmin and vmax here are tied to color bar range
    # palette choices: 
        # Perceptually uniform palettes
             # "GnBu"
             # "rocket"
             # 'mako' - this one looks really cool...
             # other palettes can be found here: https://seaborn.pydata.org/tutorial/color_palettes.html
        # 'cmap = cmap' selects custom color palette from 'def get_custom_color_palette():' function (line 20)
    plt.title("Ortho8 Pressure Mat 0-75mmHg")
    plt.xlabel("Rows")
    plt.ylabel("Columns")
    print("Refresh Button Clicked!")

#Export CSV button
def export_csv():
    #currently working on writing to csv
    #?Main issue: export_csv function is denied access to serial port while heatmap is visualized
    #?possibly use multithreading to write to CSV and seaborn.
    '''
    def writeCSV():
    arduinoData = serial.Serial('COM4', 115200)
    dataPacket = arduinoData.readline().decode().strip().split('\t')
    #np.savetxt('DosDarray.csv', aRandomArray, delimiter=',', fmt='%d')
    #pd.DataFrame(aRandomArray).to_csv("/SDII_software_P_A/newfile.csv")
    #change aRandomArray to actual live data from arduino
    pd.DataFrame(dataPacket).to_csv("/SDII_software_P_A/ArduinoRawfile.csv")
    '''
    print("Export Button Clicked!")

#Terminate sketch button
def stop_func():
    #?Current issue: GUI quiting prior to plt.close(), renaming 'command = root.destroy' to 'command = stop_func()' crashes both gui and heatmap
    plt.close()
    print("Program Terminated")
    exit()

#Main function
if __name__ == "__main__":
    
    root = tk.Tk() #GUI
    canvas = tk.Canvas(root, height=100, width=100) #Sets GUI size
    canvas.pack()

    button = tk.Button(root, text="Refresh", bg='gray',
                       fg='blue', command=refresh_func)
    button.pack(side='left')

    button = tk.Button(root, text="Export", bg='gray',
                       fg='red', command=export_csv)
    button.pack(side='left')

    button = tk.Button(root, text="Stop", bg='gray',
                       fg='green', command=root.destroy)
    button.pack(side='left')

    fig = plt.figure() 
    sns.heatmap(heatmap_matrix, cmap=cmap, cbar=True, vmin=0.0, vmax=75.0)  #vmin and vmax here are tied to color bar range
    plt.title("Ortho8 Pressure Mat 0-75mmHg")
    plt.xlabel("Rows")
    plt.ylabel("Columns")

    t1 = threading.Thread(target=readSerial)
    #t2 = threading.Thread(target=writeCSV)

    t1.start()
    #t2.start()
    anim = FuncAnimation(
        fig, visualize, init_func=setup, interval=112)  
    plt.show()
    t1.join()

    #t2.join
    root.mainloop()