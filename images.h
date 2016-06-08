#ifndef IMAGES
#define IMAGES
#include <Magick++.h> 
#include <iostream> 
#include <SFML/Graphics.hpp>

const int MODES_NUMBER = 9;
// std::vector<Magick::Image> images;

std::string ModeHelp();
void ChangeValue(float deltaTime);

void ImageLoad(char* inputFileName, char* outputFileName, int width, int height);
bool dropChange();
void NextMode();
void PreviousMode();
void NextSubMode();
void PreviousSubMode();
void SaveLayer();
void SaveImage();
void Undo();
sf::Uint8* GetPixelsFromImage(Magick::Image &image);
#endif