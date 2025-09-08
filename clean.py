import sys

def parse_volume(value: str) -> str:
    """Convert '3.2K', '1.5M', '2B' into integers as strings."""
    value = value.strip().strip('"').strip("'")  # Remove quotes
    if not value:
        return "0"

    multiplier = 1
    if value.endswith("K"):
        multiplier = 1_000
        value = value[:-1]
    elif value.endswith("M"):
        multiplier = 1_000_000
        value = value[:-1]
    elif value.endswith("B"):
        multiplier = 1_000_000_000
        value = value[:-1]

    return str(int(float(value) * multiplier))

if len(sys.argv) < 3:
    print(f"Usage: {sys.argv[0]} input.csv output.csv [reverse]")
    sys.exit(1)

input_file, output_file = sys.argv[1], sys.argv[2]
reverse_rows = len(sys.argv) == 4 and sys.argv[3].lower() == "reverse"

# Read original file in text mode (CSV text)
with open(input_file, "r", newline="") as f_in:
    lines = f_in.readlines()

processed_lines = []
for line in lines:
    line = line.strip()
    if not line:
        continue
    cols = [col.strip().strip('"').strip("'") for col in line.split(",")]  # remove quotes
    # Assume last column is volume
    cols[-1] = parse_volume(cols[-1])
    processed_lines.append(",".join(cols) + "\n")

if reverse_rows:
    processed_lines = list(reversed(processed_lines))

with open(output_file, "w", newline="") as f_out:
    f_out.writelines(processed_lines)
