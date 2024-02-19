import csv
import matplotlib.pyplot as plt
from datetime import datetime

# Function to parse the input data and plot it
def parse_and_plot_data(input_file):
    times = []
    data = []
    
    with open(input_file, 'r') as f_input:
        csv_reader = csv.reader(f_input)
        next(csv_reader)  # Skip header

        for row in csv_reader:
            millis, temp_str = row
            time = datetime.fromtimestamp(int(millis) / 1000.0)  # Convert millis to seconds
            value = float(temp_str)
                
            times.append(time)
            data.append(value)
    
    plt.plot(times, data)
    plt.ylabel('Temp (F)')
    plt.title('Temperature Over Time')
    plt.xlabel('Time')
    plt.xticks(rotation=45)  # Rotate x-axis labels for better readability
    plt.show()

# Example usage:
input_file = 'output.csv'  # Change this to the name of your output CSV file

parse_and_plot_data(input_file)
