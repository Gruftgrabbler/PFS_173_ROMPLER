This folder contains the kickdrum sample data
samplerate = 11025, samples = 1440, duration ~= 131ms
Encoding Header: 8BitUnsigned, No twos_complement conversion applied, MONO
Encoding WavFile: 16BitPCM, MONO

To use it with the project we need to further decrease the samplerate:
python reduce_samplerate.py ../samples/kick/kick.raw.h 2 -plt -c 1 -p -t -wb ../samples/kick/writeback.wav -sr 11025
