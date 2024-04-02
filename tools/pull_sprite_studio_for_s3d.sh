#!/bin/bash

cd $(dirname $0)

git pull

cd ../sample_project/project/SpriteStudioForSiv3D

git pull

read -p "Hit any key"
