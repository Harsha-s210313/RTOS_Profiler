import serial
import json
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Serial port config
PORT = "COM3"      # change to your actual port
BAUD = 9600

# Data storage
cpu_values = []
task_data = {}  # { "Task1": 23.5, "Task2": 8.9 }

# Open serial
ser = serial.Serial(PORT, BAUD)

def update(frame):
    global task_names, cpu_values

    # Read line from serial
    line = ser.readline().decode('utf-8').strip()
    
    try:
        data = json.loads(line)
        cpu_values.append(data['cpu'])
        task_data[data['task']] = data['cpu']
        # Keep only last 10 entries
        if len(task_names) > 10:
            task_names.pop(0)
            cpu_values.pop(0)
        
        # Clear and redraw
        plt.cla()
        plt.bar(task_data.keys(), task_data.values())
        plt.xlabel('Task')
        plt.ylabel('CPU Usage (%)')
        plt.title('Real-time CPU Usage per Task')
        plt.ylim(0, 100)
    except json.JSONDecodeError:
        print("Invalid JSON:", line)

fig, ax = plt.subplots()
ani = animation.FuncAnimation(fig, update, interval=1000)
plt.show()