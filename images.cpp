#include <Magick++.h> 
#include <iostream> 
#include <SFML/Graphics.hpp>
#include <sstream>
#include "images.h"

using namespace std; 
using namespace Magick; 

struct Action
{
  int mode;
  int subMode;
  vector<double> _value;
};

struct ValueRange
{
  double minValue;
  double maxValue;
  double value;
  double valueRate;
};

int mode = 0;
int subMode = 0;
std::vector<Magick::Image> images;
Image originalImage;

char* outputFileName;

vector<Action> actions;

vector<ValueRange> _value(5);
int subModesNumber;

string ModeHelp()
{
  std::stringstream os;
  os << "Mode:" << endl;
  switch(mode)
  {
    case 0:
      os << "View" << endl;
      break;
    case 1:
      os << "Gamma" << endl << endl << " Submode:" << endl;
      switch(subMode)
      {
        case 0: 
          os << " Uniform" << endl;
          break;
        case 1:
          os << " Red" << endl;
          break;
        case 2:
          os << " Green" << endl;
          break;
        case 3:
          os << " Blue" << endl;
          break;
      }
      break;
    case 2://To add here
      os << "Modulate" << endl << endl << " Submode:" << endl;
      switch(subMode)
      {
        case 0:
          os << " Brightness" << endl;
          break;
        case 1:
          os << " Saturation" << endl;
          break;
        case 2:
          os << " Hue" << endl;
          break;
      }
      break;
    
    case 3:
      os << "Shade" << endl << endl << " Submode:" << endl;
      switch(subMode)
      {
        case 0:
          os << " Azimut" << endl;
          break;
        case 1:
          os << " Elevation" << endl;
          break;
      }
      break;
    case 4:
      os << "Blur" << endl << endl << " Submode:" << endl;
      switch(subMode)
      {
        case 0:
          os << " Radius" << endl;
          break;
        case 1:
          os << " Sigma" << endl;
          break;
      }
      break;
    case 5:
      os << "Motion Blur" << endl << endl << " Submode:" << endl;
      switch(subMode)
      {
        case 0:
          os << " Radius" << endl;
          break;
        case 1:
          os << " Sigma" << endl;
          break;
        case 2:
          os << " Angle" << endl;
          break;
      }
      break;
    case 6:
      os << "Sharpen" << endl << endl << " Submode:" << endl;
      switch(subMode)
      {
        case 0:
          os << " Radius" << endl;
          break;
        case 1:
          os << " Sigma" << endl;
          break;
      }
      break;
    case 7:
      os << "Emboss" << endl << endl << " Submode:" << endl;
      switch(subMode)
      {
        case 0: 
          os << " Radius" << endl;
          break;
        case 1:
          os << " Sigma" << endl;
          break;
      }
      break;
    case 8:
      os << "Edge" << endl;
      break;
    default:
      os << "<INVALID>" << endl;
  }
  os << endl << "Value: " << _value[subMode].value << endl;
  return os.str();
}

bool _changed = true;
bool dropChange()
{
  if(_changed)
  {
    _changed = false;
    return true;
  }
  else
    return false;
}

void ApplyFiltering(Image &image)
{
  _changed = true;
  switch(mode)
  {
    case 0:
      break;
    case 1:
      if(subMode == 0)
        image.gamma(_value[0].value);
      else
        image.gamma(_value[1].value,_value[2].value,_value[3].value);
      break;
    case 2:
      image.modulate(_value[0].value,_value[1].value,(_value[2].value*100/180)+100);
      break;
    case 3:
      image.shade(_value[0].value,_value[1].value,false);
      break;
    case 4:
      image.blur(_value[0].value,_value[1].value);
      break;
    case 5:
      image.motionBlur(_value[0].value,_value[1].value,_value[2].value);
      break;
    case 6:
      image.sharpen(_value[0].value,_value[1].value);
      break;
    case 7:
      image.emboss(_value[0].value,_value[1].value);
      break;
    case 8:
      // Magick::ChannelType ch;
      image.fx("0.5",ChannelType::AllChannels);
      // image.edge(round(_value[0].value));
      break;
    
    
  }
}

void ApplyFiltering()
{
  images.pop_back();
  images.push_back(images.back());
  ApplyFiltering(images.back());
}

void ChangeValue(float deltaTime)
{
  _value[subMode].value += deltaTime * _value[subMode].valueRate;
  if(_value[subMode].value > _value[subMode].maxValue)
    _value[subMode].value = _value[subMode].minValue;
  else if(_value[subMode].value < _value[subMode].minValue)
    _value[subMode].value = _value[subMode].maxValue;
  ApplyFiltering();

}

void SetValues()
{
  _value.clear();
  switch(mode)
  {
    case 0:
      subModesNumber = 1;
      // _value[0].value = 0;
      _value[0].minValue = _value[0].value = _value[0].maxValue = _value[0].valueRate = 0;
      break;
    case 1://Gamma
      subModesNumber = 4;
      for (int i = 0; i <= 3; i++)
      {
        _value[i].value = 1;
        _value[i].minValue = 0;
        _value[i].maxValue = 3;
        _value[i].valueRate = 1;
      }
      break;
    case 2://Modulate
      subModesNumber = 3;
      for (int i = 0; i <= 1; i++)
      {
        _value[i].value = 100;
        _value[i].minValue = 0;
        _value[i].maxValue = 300;
        _value[i].valueRate = 200;
      }
      _value[2].value = 0;
      _value[2].minValue = -180;
      _value[2].maxValue = 180;
      _value[2].valueRate = 180;
      break;
    case 3://Shade
      subModesNumber = 2;
      _value[0].value = 30;
      _value[1].value = 120;
      for (int i = 0; i <= 1; i++)
      {
        _value[i].minValue = 0;
        _value[i].maxValue = 360;
        _value[i].valueRate = 100;
      }
      _value[1].maxValue = 180;
      break;
    case 4://Blur
      subModesNumber = 2;
      _value[0].value = 1;
      _value[1].value = 0.5;
      for (int i = 0; i <= 1; i++)
      {
        _value[i].minValue = 0;
        _value[i].maxValue = 3;
        _value[i].valueRate = 1;
      }
      _value[0].maxValue = 15;
      break;
    case 5://Motion Blur
      subModesNumber = 3;
      _value[0].value = 2.5;
      _value[1].value = 5;
      _value[2].value = 30;
      for (int i = 0; i <= 1; i++)
      {
        _value[i].minValue = 0;
        _value[i].maxValue = 10;
        _value[i].valueRate = 1;
      }
      _value[0].maxValue = 5;
      _value[2].maxValue = 360;
      _value[2].valueRate = 100;
      break;
    case 6://Sharpen
      subModesNumber = 2;
      _value[0].value = 1;
      _value[1].value = 0.5;
      for (int i = 0; i <= 1; i++)
      {
        _value[i].minValue = 0;
        _value[i].maxValue = 5;
        _value[i].valueRate = 0.5;
      }
      break;
    case 7://Emboss
      subModesNumber = 2;

      _value[0].value = 1;
      _value[1].value = 0.5;
      for (int i = 0; i <= 1; i++)
      {
        _value[i].minValue = 0;
        _value[i].maxValue = 3;
        _value[i].valueRate = 2;
      }
      break;
    case 8://Edge
      subModesNumber = 1;
      _value[0].minValue = 0;
      _value[0].value = 3;
      _value[0].maxValue = 5;
      _value[0].valueRate = 2;
      break;
  }
}

void ImageInit()
{
  subMode = 0;
  SetValues();
  ApplyFiltering();
}

void ImageLoad(char* inputFileName, char* _outputFileName, int width, int height)
{
  outputFileName = _outputFileName;

  //Initialize input image

  Image image;
  // Read a file into image object 
  image.read(inputFileName);

  Color _transparentColor;
  _transparentColor.alpha(1);

  image.backgroundColor(_transparentColor);

  // Copy original
  originalImage = image;

  std::stringstream os;
  os << width << "x" << height << ">";

  image.resize(os.str());

  //Add original to the vector
  images.push_back(image);

  //Create first history copy
  images.push_back(images.back());

  ImageInit();
}

void NextMode()
{
  mode = (mode + 1 >= MODES_NUMBER ? 0 : mode + 1);
  ImageInit();
}

void PreviousMode()
{
  mode = (mode - 1 < 0 ? MODES_NUMBER - 1 : mode - 1);
  ImageInit();
}

void NextSubMode()
{
  subMode = (subMode + 1 >= subModesNumber ? 0 : subMode + 1);
  ApplyFiltering();
}

void PreviousSubMode()
{
  subMode = (subMode - 1 < 0 ? subModesNumber - 1 : subMode - 1);
  ApplyFiltering();
}

void Undo()
{
  if(images.size() > 2)
  {
    images.pop_back();
    images.pop_back();
    images.push_back(images.back());
    // cout << "size: " << images.size() << endl;
    _changed = true;
  }
}

void SaveLayer()
{
  //TODO: Remove tail if required
  images.push_back(images.back());

  vector<double> v(5);

  for(int i = 0; i < 5; i++)
  {
    v[i] = _value[i].value;
  }

  Action a = {mode, subMode, v};
  actions.push_back(a);

  mode = 0;
  ImageInit();
}

void SaveImage()
{
  try
  {
    Image image = originalImage;
    for(int i = 0; i < actions.size(); i++)
    {
      mode = actions[i].mode;
      subMode = actions[i].subMode;
      for (int j = 0; j < 5; j++)
        _value[j].value = actions[i]._value[j];
      ApplyFiltering(image);
    }
    image.write(outputFileName);  
  }
  catch( Exception &error_ ) 
  { 
    cout << "Caught exception: " << error_.what() << endl; 
  } 
  mode = 0;
  ImageInit();
}

sf::Uint8* GetPixelsFromImage(Image &image)
{

  int w = image.columns();
  int h = image.rows();

  PixelPacket *pixels = image.getPixels(0,0,w,h);

  sf::Uint8* output = new sf::Uint8[w*h*4];

  double ratio = (double)(pow(2,sizeof(output[0])*8))/(pow(2,sizeof(pixels[0].red)*8));

  for (int row = 0; row < h; row++)
  {
    for (int column = 0; column < w; column++)
    {

      output[(w * row + column)*4+0] = floor(pixels[w * row + column].red*ratio);
      output[(w * row + column)*4+1] = floor(pixels[w * row + column].green*ratio);
      output[(w * row + column)*4+2] = floor(pixels[w * row + column].blue*ratio);
      output[(w * row + column)*4+3] = 255;
    }
  }

  return output;
}