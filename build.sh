#!/bin/bash
clang++ `Magick++-config --cxxflags --cppflags` -std=c++14 -O3 -o darkroom images.cpp main.cpp `Magick++-config --ldflags --libs` -lsfml-graphics -lsfml-window -lsfml-system