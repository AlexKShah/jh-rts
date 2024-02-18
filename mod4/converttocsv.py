import csv

# Function to parse the input data
def parse_data(input_file, output_file):
    with open(input_file, 'r') as f_input, open(output_file, 'w', newline='') as f_output:
        csv_writer = csv.writer(f_output)
        csv_writer.writerow(['Time', 'Data'])  # Writing header
        
        for line in f_input:
            time_data = line.strip().split(' -> ')
            if len(time_data) == 2:  # Ensure the line is in the expected format
                time, data = time_data
                csv_writer.writerow([time, data])

# Example usage:
input_file = 'temp.csv'  # Change this to the name of your input file
output_file = 'output.csv'  # Change this to the desired name of the output CSV file

parse_data(input_file, output_file)
print(f"Data has been written to {output_file}.")