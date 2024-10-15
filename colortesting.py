import tkinter as tk
import serial
import time
import threading

# Define a dictionary of RGB color codes and their corresponding color names
color_database = {
    "#FF0000": "Red",
    "#00FF00": "Green",
    "#0000FF": "Blue",
    "#FFFF00": "Yellow",
    "#FF00FF": "Magenta",
    "#00FFFF": "Cyan / Aqua",
    "#FFFFFF": "White",
    "#000000": "Black",
}

# Initialize serial communication with Arduino (use the correct COM port)
ser = serial.Serial('COM10', baudrate=9600)  # Replace 'COM10' with the correct port

# Function to look up and display color name and color
def lookup_color():
    try:
        # Read RGB values from Arduino via serial connection
        rgb_values = ser.readline().decode().strip().split(',')
        r, g, b = [int(val) for val in rgb_values]

        # Initialize color_code
        color_code = f"#{r:02X}{g:02X}{b:02X}"

        # Look up color name in the database
        color_name = color_database.get(color_code, "Unknown")

        # Update the UI elements
        color_label.config(text=f"Color Name: {color_name}")
        rgb_label.config(text=f"RGB Value: R={r}, G={g}, B={b}")
        canvas.config(bg=color_code)
    except Exception as e:
        print(f"Error: {e}")

# Make window draggable
def start_move(event):
    window.x = event.x
    window.y = event.y

def do_move(event):
    x = event.x - window.x + window.winfo_x()
    y = event.y - window.y + window.winfo_y()
    window.geometry(f"+{x}+{y}")

# Create a tkinter window
window = tk.Tk()
window.overrideredirect(True)  # Remove the window title bar
window.geometry('300x200')
window.title("Color Detection and Lookup")

# Bind mouse events to make the window draggable
window.bind('<Button-1>', start_move)
window.bind('<B1-Motion>', do_move)

# Create and configure UI elements
color_label = tk.Label(window, text="Color Name: ")
color_label.pack(pady=10)

rgb_label = tk.Label(window, text="RGB Value: R=0, G=0, B=0")
rgb_label.pack(pady=10)

canvas = tk.Canvas(window, width=100, height=100)
canvas.pack()

# Set the scanning interval in seconds (e.g., 0.02 seconds)
scanning_interval = 0.02

# Function to continuously scan for colors
def continuous_scan():
    while True:
        lookup_color()  # Call the lookup_color function
        window.update()  # Update the tkinter window
        time.sleep(scanning_interval)  # Wait for the specified interval

# Create a separate thread for continuous scanning
scan_thread = threading.Thread(target=continuous_scan)
scan_thread.daemon = True  # Allow the thread to exit when the main program exits
scan_thread.start()  # Start the continuous scanning thread

# Start the tkinter main loop
window.mainloop()
