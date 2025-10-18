import matplotlib.pyplot as plt

def plot_waveform():
    """
    Reads waveform data from stdin and plots it.
    """
    log_file = open("COM3-115200.log", 'r')
    lines = log_file.readlines()

    samples = []
    in_data_block = False

    for line in lines:
        line = line.strip()
        if line == '---END_WAVEFORM_DATA---':
            break
        if in_data_block:
            try:
                samples.append(int(line))
            except ValueError:
                print(f"Warning: Could not parse '{line}', skipping.")
        if line == '---START_WAVEFORM_DATA---':
            in_data_block = True

    if not samples:
        print("No valid waveform data was read. Exiting.")
        return

    print(f"Read {len(samples)} samples. Plotting...")

    plt.figure(figsize=(12, 6))
    plt.plot(samples)
    plt.title("Captured Audio Waveform")
    plt.xlabel("Sample Number")
    plt.ylabel("Amplitude (32-bit Signed)")
    plt.grid(True)
    plt.axhline(0, color='red', linewidth=0.8)
    plt.show()

if __name__ == '__main__':
    plot_waveform()