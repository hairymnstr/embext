#!/usr/bin/env python3
import sys
import os
from subprocess import check_call, PIPE
import hashlib
from PIL import Image, ImageDraw

force_create = False

if (not os.path.exists("testext.img")) or (force_create):
    
    # make an empty filesystem for test
    check_call(["mke2fs", "testext.img", "512M"])

if not os.path.isdir("temp"):
    os.mkdir("temp")

print("Mount image...")

check_call(["sudo", "mount", "-o", "loop", "testext.img", "temp"])

if not os.path.isdir("temp/static"):
    print("Creating directory on new volume")
    check_call(["sudo", "mkdir", "temp/static"])
    print("Making the directory writeable")
    check_call(["sudo", "chmod", "a+rwx", "temp/static"])

if not os.path.exists("temp/static/test_image.png"):
    print("Creating a test image")
    image = Image.new('RGBA', (640, 480))
    draw = ImageDraw.Draw(image)
    draw.ellipse((120, 40, 520, 440), fill = 'blue', outline = 'blue')
    print("Saving to test file system")
    image.save("temp/static/test_image.png")
    with open("temp/static/test_image.png", "rb") as fr:
        m = hashlib.md5()
        m.update(fr.read())
    with open("test_image.md5", "wb") as fw:
        fw.write(m.digest())
    
if not os.path.exists("temp/logs"):
    print("Creating log folder")
    check_call(["sudo", "mkdir", "temp/logs"])

    print("Making the directory writeable")
    check_call(["sudo", "chmod", "0777", "temp/logs"])

if not os.path.exists("temp/logs/test.txt"):
    print("Creating test text file")
    with open("temp/logs/test.txt", "w") as fw:
        fw.write("Hello world\n")
    
print("Unmount image...")
check_call(["sudo", "umount", "temp"])
