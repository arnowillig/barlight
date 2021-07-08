#!/bin/bash

# DBNBeatTracker online -j1 file_example_MP3_1MG.mp3


#DBNBeatTracker online -j1 file_example_MP3_1MG.mp3 2>/dev/null | while IFS='' read line; do echo "BEAT"; curl -s http://localhost:8080/api/beat >/dev/null; done
DBNBeatTracker online -j1 2>/dev/null | while IFS='' read line; do echo "BEAT"; curl -s http://localhost:8080/api/beat >/dev/null; done
 