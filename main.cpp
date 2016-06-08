// #include <boost/thread.hpp>
#include <Magick++.h> 
#include <iostream> 
#include <SFML/Graphics.hpp>
#include <sstream>
#include "images.h"

int sw, sh, pw;

using namespace std; 
using namespace Magick; 

extern std::vector<Magick::Image> images;

sf::Font regularFont;
int fontSize;
sf::Text statusText;


sf::Image spriteImage;
void RedrawPicture(sf::RenderWindow &window)
{
  Image &image = images.back();
  sf::Uint8* pixels = GetPixelsFromImage(image);

  spriteImage.create(image.columns(), image.rows(), pixels);
  delete [] pixels;

  sf::Texture texture;
  if (!texture.loadFromImage(spriteImage))
      return;
  sf::Sprite sprite(texture);

  float scaleX = (float)pw / image.columns();
  float scaleY = (float)sh / image.rows(); 
  float scale = (scaleY > scaleX ? scaleX : scaleY);
  sprite.setScale(scale,scale);

  statusText.setString(ModeHelp());

  window.clear();
  window.draw(sprite);
  window.draw(statusText);
  window.display();

}

#ifdef __APPLE__
  bool isControl()
  {
    return sf::Keyboard::isKeyPressed(sf::Keyboard::LSystem) || sf::Keyboard::isKeyPressed(sf::Keyboard::RSystem);
  }
#else
  bool isControl()
  {
    return sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
  }
#endif

float _time = 0.0;
int _frames = 0;
void _ShowFPS(float deltaTime)
{
  _frames++;
  _time += deltaTime;
  if (_time >= 1.0)
  {
    cout << "FPS: " << _frames << endl;
    _time -= 1.0;
    _frames = 0;
  }
}

float inputThreshold = 0.0f;
sf::Joystick::Axis turntablePosIndex = sf::Joystick::U, turntableNegIndex = sf::Joystick::V;
void CheckInput(float deltaTime)
{
  float turntableSpeed = (fabs(sf::Joystick::getAxisPosition(0, turntablePosIndex)) > 0.002f ? sf::Joystick::getAxisPosition(0, sf::Joystick::U) :
                          ((fabs(sf::Joystick::getAxisPosition(0, turntableNegIndex)) > 0.002f) ? sf::Joystick::getAxisPosition(0, sf::Joystick::V) : 0));

  float multiplier = (sf::Joystick::isButtonPressed(0, 0) ? 10 : 1) 
                   * (sf::Joystick::isButtonPressed(0, 1) ? 2 : 1)
                   * (sf::Joystick::isButtonPressed(0, 2) ? 0.5f : 1);   
  // _ShowFPS(deltaTime);
  if(!isControl())
  {
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))//Increase
    {
      ChangeValue(deltaTime);
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z))//Decrease
    {
      ChangeValue(-deltaTime);
    }
    if(turntableSpeed != 0)
    {
      ChangeValue(deltaTime*turntableSpeed*multiplier);
    }
    if(inputThreshold > 0)
    {
      inputThreshold -= deltaTime;
    }
    else
    {
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Joystick::isButtonPressed(0, 14))
      {
        NextMode();
        inputThreshold = 0.2f;
      }
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Joystick::isButtonPressed(0, 13))
      {
        PreviousMode();
        inputThreshold = 0.2f;
      }
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Joystick::isButtonPressed(0, 12))
      {
        NextSubMode();
        inputThreshold = 0.2f;
      }
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Joystick::isButtonPressed(0, 11))
      {
        PreviousSubMode();
        inputThreshold = 0.2f;
      }
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return) || sf::Joystick::isButtonPressed(0, 3))
      {
        SaveLayer();
        inputThreshold = 0.2f;
      }
      if(sf::Joystick::isButtonPressed(0,10))
      {
        SaveImage();
        inputThreshold = 0.2f;
      }
      if(sf::Joystick::isButtonPressed(0,9))
      {
        Undo();
        inputThreshold = 0.2f;
      }
    }
  }
  else
  {
    if(inputThreshold > 0)
    {
      inputThreshold -= deltaTime;
    }
    else
    {
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
      {
        SaveImage();
        inputThreshold = 0.2f;
      }
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
      {
        Undo();
        inputThreshold = 0.2f;
      }
    }
  }
  
}

void TraceJoystick()
{
  // cout << "X: " << sf::Joystick::getAxisPosition(0, sf::Joystick::X) << "\n";
  // cout << "Y: " << sf::Joystick::getAxisPosition(0, sf::Joystick::Y) << "\n";
  // cout << "Z: " << sf::Joystick::getAxisPosition(0, sf::Joystick::Z) << "\n";
  // cout << "R: " << sf::Joystick::getAxisPosition(0, sf::Joystick::R) << "\n";
  // // if(fabs(sf::Joystick::getAxisPosition(0, sf::Joystick::U)) > 0.005f)
  // cout << "U: " << sf::Joystick::getAxisPosition(0, sf::Joystick::U) << "\n";
  // cout << "V: " << sf::Joystick::getAxisPosition(0, sf::Joystick::V) << "\n";
  int count = sf::Joystick::getButtonCount(0);
  for(int i = 0; i<count; i++)
  {
    if(sf::Joystick::isButtonPressed(0, i))
      cout << i << "\n";
  }
}

int main(int argc,char **argv) 
{ 
  //Check arguments
  if(argc < 3)
  {
    cout << "Error: You must provide input and output images\n";
    return 1;
  }
  else if(argc >= 5)
  {
    try
    {
      sw = std::stoi(argv[3]);
      sh = std::stoi(argv[4]);
    }
    catch(Exception &error_)
    {
      cout << "Caught exception: " << error_.what() << endl;
      return 1;
    }
  }
  else
  {
    sw = 1920;
    sh = 1080;
  }

  pw = sw - sw * 0.2;

  fontSize = floor(sh/30);

  InitializeMagick(*argv);
  try
  {
    ImageLoad(argv[1],argv[2],pw/4,sh/4);
  }
  catch( Exception &error_ ) 
  { 
    cout << "Caught exception: " << error_.what() << endl; 
    return 1; 
  } 

  // Create the main window
  sf::RenderWindow window(sf::VideoMode(sw, sh), "Twitter-kun Darkroom");

  //Load fonts
  if(!regularFont.loadFromFile("fonts/PT_Regular.ttf"))
  {
    cout << "Error: On font loading\n";
    return 1;
  }

  //Setup text
  statusText.setFont(regularFont);
  statusText.setCharacterSize(fontSize);
  statusText.setColor(sf::Color::White);
  statusText.setPosition(pw, 0);
  statusText.setString("Multiline\nTest\n");


  sf::Clock deltaClock;
  while (window.isOpen())
  {
      // Process events
      sf::Event event;
      while (window.pollEvent(event))
      {
          // Close window: exit
          if (event.type == sf::Event::Closed)
            window.close();
      }

      CheckInput(deltaClock.restart().asSeconds());

      if(dropChange())
        RedrawPicture(window);
      // TraceJoystick();
  }
  return 0; 
}

