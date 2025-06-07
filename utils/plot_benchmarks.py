import os # os can be imported safely
import sys # For sys.executable
# Dependencies are assumed to be pre-installed by a previous step.

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import seaborn as sns
import re

# Define the input CSV file and output directory
CSV_FILE_PATH = 'doc/data/performance_results.csv'
FIGURES_DIR = 'doc/figures'

def parse_benchmark_name(name):
    parts = name.split('/')
    sorter_type = parts[0].replace('BM_', '').replace('BitonicSort', '')

    data_size = None
    threads = None

    # Attempt to find data size (always present as range(0))
    # It's the first numeric part after BM_SorterName
    if len(parts) > 1 and parts[1].isdigit():
        data_size = int(parts[1])

    # Attempt to find thread count for StdThread
    if 'StdThread' in sorter_type and len(parts) > 2:
        if parts[2].isdigit(): # Handles cases like BM_StdThreadBitonicSort/size/threads
             threads = int(parts[2])
        elif "threads:" in parts[2] : # Handles cases like BM_StdThreadBitonicSort/size/threads:N (older benchmark format)
            try:
                threads = int(parts[2].split(':')[1])
            except (IndexError, ValueError):
                threads = None # Could not parse

    # For OpenMP, threads are not explicitly in the name from the C++ benchmark setup
    # We can assume it uses max threads or a default. Label it for clarity.
    if 'OpenMP' in sorter_type:
        # If a specific thread count was passed and encoded, parse it.
        # Otherwise, use a label like 'Max' or 'Default'.
        # For now, we don't have this in the name, so it will be None.
        pass # threads remains None

    return sorter_type, data_size, threads

def plot_performance(df):
    # Set a nice style
    sns.set_style("whitegrid")
    plt.rcParams['figure.dpi'] = 300  # High resolution

    # Ensure FIGURES_DIR exists
    if not os.path.exists(FIGURES_DIR):
        os.makedirs(FIGURES_DIR)

    # --- Plot 1: Performance vs. Input Size for all sorters ---
    # For StdThread, we need to select its best performing thread count or a representative one.
    # Or, plot all its thread variations. For simplicity, let's find best or use hardware_concurrency if available.

    plt.figure(figsize=(14, 8))

    sorter_names = df['sorter_type'].unique()

    for sorter in sorter_names:
        subset = df[df['sorter_type'] == sorter]
        label = sorter

        if sorter == 'StdThread':
            # Find the thread count that gives min time for each size, or plot a few specific ones.
            # For simplicity, let's pick the one with most threads if 'threads' column is good
            # Or let's plot each thread configuration for StdThread separately
            if 'threads' in subset.columns and subset['threads'].notna().any():
                for thread_count in sorted(subset['threads'].dropna().unique()):
                    thread_subset = subset[subset['threads'] == thread_count]
                    if not thread_subset.empty:
                        plt.plot(thread_subset['data_size'], thread_subset['cpu_time'], marker='o', linestyle='-', label=f'StdThread ({int(thread_count)} thr)')
                continue # Skip generic 'StdThread' label if we plotted per-thread lines

        if not subset.empty:
             plt.plot(subset['data_size'], subset['cpu_time'], marker='o', linestyle='-', label=label)

    plt.title('Bitonic Sort Performance Comparison', fontsize=16)
    plt.xlabel('Input Size (N)', fontsize=14)
    plt.ylabel('CPU Time (nanoseconds)', fontsize=14)
    plt.xscale('log', base=2) # Input size is often in powers of 2
    plt.yscale('log')

    # Format x-axis to show integers for powers of 2
    plt.gca().xaxis.set_major_formatter(ticker.FuncFormatter(lambda x, _: f'{int(x)}'))

    plt.legend(fontsize=10)
    plt.grid(True, which="both", ls="-", alpha=0.7)
    plt.tight_layout()
    plot_path = os.path.join(FIGURES_DIR, 'performance_comparison_loglog.png')
    plt.savefig(plot_path)
    plt.close()
    print(f"Saved performance comparison plot to {plot_path}")

    # --- Plot 2: StdThread Scalability (Time vs Threads for a large N) ---
    if 'StdThread' in sorter_names:
        std_thread_df = df[df['sorter_type'] == 'StdThread'].copy()
        if not std_thread_df.empty and 'threads' in std_thread_df.columns and std_thread_df['threads'].notna().any():
            # Select a large data size, e.g., the largest one available
            largest_n = std_thread_df['data_size'].max()
            scalability_data = std_thread_df[std_thread_df['data_size'] == largest_n].copy()
            scalability_data.dropna(subset=['threads'], inplace=True) # Ensure threads data is not NaN
            scalability_data['threads'] = scalability_data['threads'].astype(int)
            scalability_data.sort_values('threads', inplace=True)

            if not scalability_data.empty: # Changed .empty() to .empty
                plt.figure(figsize=(10, 6))
                plt.plot(scalability_data['threads'], scalability_data['cpu_time'], marker='o', linestyle='-')
                plt.title(f'StdThread Scalability (N={largest_n})', fontsize=16)
                plt.xlabel('Number of Threads', fontsize=14)
                plt.ylabel('CPU Time (nanoseconds)', fontsize=14)
                # Ensure x-axis shows integer thread counts
                plt.gca().xaxis.set_major_locator(ticker.MaxNLocator(integer=True))
                plt.grid(True, which="both", ls="-", alpha=0.7)
                plt.tight_layout()
                scalability_plot_path = os.path.join(FIGURES_DIR, 'std_thread_scalability.png')
                plt.savefig(scalability_plot_path)
                plt.close()
                print(f"Saved StdThread scalability plot to {scalability_plot_path}")
            else:
                print("Not enough data for StdThread scalability plot (largest N or threads missing).")
        else:
            print("StdThread data or 'threads' column not suitable for scalability plot.")
    else:
        print("StdThread sorter not found in data, skipping scalability plot.")


def main():
    # Find the actual header row
    header_row_index = 0
    try:
        with open(CSV_FILE_PATH, 'r') as f:
            for i, line in enumerate(f):
                # Google Benchmark CSV header starts with 'name' or sometimes '\"name\"' if quoted
                if line.strip().startswith('"name",') or line.strip().startswith('name,'):
                    header_row_index = i
                    break
            else: # no break
                print(f"Error: Could not find the CSV header row starting with 'name,' in {CSV_FILE_PATH}")
                return
    except FileNotFoundError:
        print(f"Error: CSV file not found at {CSV_FILE_PATH}")
        return

    # Try to read the CSV, starting from the identified header row
    try:
        df = pd.read_csv(CSV_FILE_PATH, skiprows=header_row_index)
    except pd.errors.EmptyDataError:
        print(f"Error: CSV file is empty or header not found correctly at {CSV_FILE_PATH}")
        return
    except Exception as e:
        print(f"Error reading CSV file: {e}")
        return

    # Filter out benchmark aggregate rows if any (e.g., those with 'median', 'mean' if not desired)
    # Google benchmark CSV output for version specified usually has 'name', 'iterations', 'real_time', 'cpu_time', 'time_unit', etc.
    # We are interested in rows where 'time_unit' is present (i.e., actual measurements, not aggregates)
    df = df[df['time_unit'].notna()]

    # Parse benchmark names
    parsed_names = df['name'].apply(parse_benchmark_name)
    df['sorter_type'] = [item[0] for item in parsed_names]
    df['data_size'] = [item[1] for item in parsed_names]
    df['threads'] = [item[2] for item in parsed_names]

    # Convert time to a common unit, e.g., nanoseconds, if not already
    # Google benchmark CSV output has 'real_time' and 'cpu_time' columns, and 'time_unit' (e.g., 'ns', 'us', 'ms')
    # For simplicity, assuming times are already in nanoseconds or a consistent unit that can be plotted directly.
    # If conversion is needed:
    # def convert_to_ns(row):
    #     time = row['cpu_time'] # or 'real_time'
    #     unit = row['time_unit']
    #     if unit == 'ms':
    #         return time * 1e6
    #     elif unit == 'us':
    #         return time * 1e3
    #     return time # assume ns
    # df['cpu_time_ns'] = df.apply(convert_to_ns, axis=1)
    # Then use 'cpu_time_ns' for plotting. For now, direct 'cpu_time' is used.

    # Drop rows where data_size might be None (e.g. from parsing issues or header)
    df.dropna(subset=['data_size'], inplace=True)
    df['data_size'] = df['data_size'].astype(int)

    # Filter out any non-successful runs if error_occurred column exists and is true
    if 'error_occurred' in df.columns:
        df = df[df['error_occurred'].fillna(False) == False] # Keep rows where error_occurred is false or NaN

    if df.empty:
        print("No valid benchmark data found after parsing and filtering. Cannot generate plots.")
        return

    plot_performance(df)

if __name__ == '__main__':
    main()
