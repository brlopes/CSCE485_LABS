Hello, I separated the input frames and programs to better organize this submission and reduce redundancy, the testing set of frames for samples can be found under \frames

In order to test each program, please copy the frames files (ppm) from the \frames folder, or copy each program into the frames folder (either way). For example, you can copy the 100 sample frames from /frames to the \#4 folder and the output result can be used on #5 by copying the results to the \5 folder. Each folder has 1 ppm/pgm sample to start.

The summary of this lab report can be found at .\Report_lab3.pdf

parameter used on ffmpeg to encode images:
ffmpeg -r 15 -i frame%d.ppm Aoutput.mp4

