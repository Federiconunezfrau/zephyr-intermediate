import sys

def count_thread_executions(filepath):
    # Initialize our counters
    counts = {
        "T_HIGH": 0,
        "T_MED": 0,
        "T_LOW": 0
    }

    try:
        with open(filepath, 'r', encoding='utf-8') as file:
            for line in file:
                if "T_HIGH running" in line:
                    counts["T_HIGH"] += 1
                elif "T_MED running" in line:
                    counts["T_MED"] += 1
                elif "T_LOW running" in line:
                    counts["T_LOW"] += 1

        print("--- Zephyr Thread Execution Counts ---")
        print(f"T_HIGH running: {counts['T_HIGH']}")
        print(f"T_MED running:  {counts['T_MED']}")
        print(f"T_LOW running:  {counts['T_LOW']}")

    except FileNotFoundError:
        print(f"Error: The file '{filepath}' was not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    # Ensure a file path was passed via the command line
    if len(sys.argv) > 1:
        log_file = sys.argv[1]
        count_thread_executions(log_file)
    else:
        print("Usage: python count_threads.py <path_to_log_file.txt>")