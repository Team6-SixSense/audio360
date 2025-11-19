# WAV Converter

To see usage, run

```bash
python wav_converter.py -h
```

## Input file format

Binary audio file where each audio sample is either 2 or 4 bytes across the entire file.

# Audio Visualizer

Make sure dependencies are installed

```bash
python -m pip install -r requirements.txt
```

Install the VCOM Port driver from: https://www.st.com/en/development-tools/stsw-stm32102.html#get-software

Near the top of the file, update this to the appropriate COM
port for the STM USB connection:

SERIAL_PORT = 'COM5'

Run the script:

```bash
python audio_viz.py
```