"""
import pynput
import bluetooth connection library

1 Connect with ESP32 via BlueTooth

2 Handle Failure + Log

3 Loop for:
    .1 Right Arrow Key -> Send right move to ESP
    .2 Left Arrow Key -> Send Left move to ESP
    .3 Up Arrow Key -> Send Up move to ESP
    .4 Down Arrow Key -> Send Down move to ESP

4 In Loop. Handle failure + Log

5  Catch Blue tooth connection failure, and exit app.
    ELSE keep running forever unless Q pressed or CTRL+C (Handled by default terminal likely)
"""