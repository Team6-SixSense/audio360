# Single Microphone Verification

This test confirms that Mic1 is capable of accurately capturing environmental audio data

## Test Summary

- **Objective:** Validate the functionality of a single microphone in capturing audio.
- **Result:** Successful. The converted file Mic1.wav clearly captured the spoken phrase:
*"Testing 1 2 3, I'm hoping we don't hit a system fault."*

## Technical Notes

Code adjustments are necessary to ensure microphone buffer data is written at the end of the collection process.
This prevents loss of audio samples.
As a result, the system can reliably collect up to 5 seconds of audio data before encountering memory faults with this method.

## Results

`Mic1.wav` is the recording from this test.

<audio controls>
    <source src="./Mic1.wav" type="audio/wav">
    Your browser does not support the audio element.
</audio>
