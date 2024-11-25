@echo off
for %%f in (*.mp3) do ffmpeg -i "%%f" "%%~nf.wav"
pause