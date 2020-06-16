#!/bin/bash

DATE=$(date +"%Y-%m-%d_%H%M")

sudo fswebcam -r 1280x720 /var/www/images/camera.jpg
