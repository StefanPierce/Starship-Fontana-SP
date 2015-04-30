#ifndef SFAPP_H
#define SFAPP_H

#include <memory>   // Pull in std::shared_ptr
#include <iostream> // Pull in std::cerr, std::endl
#include <list>     // Pull in list
#include <sstream>
#include <SDL2/SDL_ttf.h>
using namespace std;

#include "SFCommon.h"
#include "SFEvent.h"
#include "SFAsset.h"

/**
 * Represents the StarshipFontana application.  It has responsibilities for
 * * Creating and destroying the app window
 * * Processing game events
 */
class SFApp {
public:
  SFApp(std::shared_ptr<SFWindow>);
  virtual ~SFApp();
  void    OnEvent(SFEvent &);
  int     OnExecute();
  void    OnUpdateWorld();
  void    OnRender();
  void    addWall(bool Left);
  void    FireProjectile();
private:
  SDL_Surface           * surface;
  bool                    is_running;

  shared_ptr<SFWindow>       sf_window;

  shared_ptr<SFAsset>        player;
  shared_ptr<SFBoundingBox>  app_box;
  list<shared_ptr<SFAsset> > projectiles;
  list<shared_ptr<SFAsset> > aliens;
  list<shared_ptr<SFAsset> > coins;
  list<shared_ptr<SFAsset> > walls;
  list<shared_ptr<SFAsset> > explosions;
  bool up = false;
  bool down = false;
  bool left = false;
  bool right = false;
  bool fired = false;
  int score = 0;
  float lefts, rights, ups, downs;  
  float alienspeed = 1.5f;
  float alientempspeed = alienspeed;
  float projectilespeed = 6.0f;
  int fire;
  int firespeed = 20;
  int maxAliens = 10;
  int currentAliens = 0;
  int maxWalls = 4;
  int noWalls = 0;
  int fastshoot = 0, slowaliens = 0;
  int health = 100;
  bool gameover = false;
  SDL_Texture               * background;
  SDL_Texture               * healthbar;
  SDL_Texture               * healthgreen;
  SDL_Rect back, back2, healthpos, healthgreenpos;
  SFError OnInit();
};
#endif
