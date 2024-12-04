#!/bin/bash
# This script can be used to iterate over all the inputs and create the output.

input_dir="inputs"
output_dir="output"

# Check if the output directory exists, if not, create it
if [ ! -d "$output_dir" ]; then
  mkdir -p "$output_dir"
fi

# Iterate through all input files in the inputs directory
for input_file in "$input_dir"/input*; do
  # Extract the number from the input file name (e.g., input1 -> 1)
  file_number=$(basename "$input_file" | sed 's/^input//')
  
  # Construct the corresponding output file name
  output_file="$output_dir/output$file_number"

  # Run the repcrep command
  ./repcrep "$input_file" "$output_file"
done