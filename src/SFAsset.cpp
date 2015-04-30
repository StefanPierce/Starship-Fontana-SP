#include "SFAsset.h"

int SFAsset::SFASSETID=0;

SFAsset::SFAsset(SFASSETTYPE type, std::shared_ptr<SFWindow> window): type(type), sf_window(window) {
  this->id   = ++SFASSETID;
  switch (type) {
  case SFASSET_EXPLOSION:
    sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/explosion.png");
    break;
  case SFASSET_PLAYER:
    sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/player.png");
    break;
  case SFASSET_PROJECTILE:
    sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/projectile.png");
    break;
  case SFASSET_ALIEN:
    sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/alien.png");
    break;
  case SFASSET_COIN:
    int gencoin;
    gencoin = rand() % 7;
    switch(gencoin){
    case 0:
    sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/fastshoot.png");
    code = FASTSHOOT;
    break;
    case 1:
    code = HEALTH;
    sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/health.png");    
    break;
    case 2:
    code = SLOWALIENS;
    sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/slowaliens.png");
    break;
    case 3:
    code = CLEARALIENS;
    sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/killaliens.png");
    break;
    case 4:
    code = FASTSHOOT;
    sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/fastshoot.png");
    break;
    case 5:
    code = HEALTH;
    sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/health.png");
    break;
    case 6:
    code = SLOWALIENS;
    sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/slowaliens.png");
    break;
    }
    break;
  case SFASSET_WALL:
    sprite = IMG_LoadTexture(sf_window->getRenderer(), "assets/wall.png");
    break;
  }

  if(!sprite) {
    cerr << "Could not load asset of type " << type << endl;
    throw SF_ERROR_LOAD_ASSET;
  }

  // Get texture width & height
  int w, h;
  SDL_QueryTexture(sprite, NULL, NULL, &w, &h);

  // Initialise bounding box
  bbox = make_shared<SFBoundingBox>(SFBoundingBox(Vector2(0.0f, 0.0f), w, h));
}

SFAsset::SFAsset(const SFAsset& a) {
  sprite = a.sprite;
  sf_window = a.sf_window;
  bbox   = a.bbox;
  type   = a.type;
}

SFAsset::~SFAsset() {
  bbox.reset();
  if(sprite) {
    SDL_DestroyTexture(sprite);
    sprite = nullptr;
  }
}

/**
 * The logical coordinates in the game assume that the screen
 * is indexed from 0,0 in the bottom left corner.  The blittable
 * coordinates of the screen map 0,0 to the top left corner. We
 * need to convert between the two coordinate spaces.  We assume
 * that there is a 1-to-1 quantisation.
 */
Vector2 GameSpaceToScreenSpace(SDL_Renderer* renderer, Vector2 &r) {
  int w, h;
  SDL_GetRendererOutputSize(renderer, &w, &h);

  return Vector2 (
                  r.getX(),
                  (h - r.getY())
                  );
}

void SFAsset::SetPosition(Point2 & point) {
  Vector2 v(point.getX(), point.getY());
  bbox->SetCentre(v);
}

Point2 SFAsset::GetPosition() {
  return Point2(bbox->centre->getX(), bbox->centre->getY());
}

SFAssetId SFAsset::GetId() {
  return id;
}

void SFAsset::OnRender() {
  // 1. Get the SDL_Rect from SFBoundingBox
  SDL_Rect rect;

  Vector2 gs = (*(bbox->centre) + (*(bbox->extent_x) * -1)) + (*(bbox->extent_y) * -1);
  Vector2 ss = GameSpaceToScreenSpace(sf_window->getRenderer(), gs);
  rect.w = bbox->extent_x->getX() * 2;
  rect.h = bbox->extent_y->getY() * 2;
  rect.x = ss.getX();
  rect.y = ss.getY() - rect.h;
  // 2. Blit the sprite onto the level
  SDL_RenderCopy(sf_window->getRenderer(), sprite, NULL, &rect);
}

void SFAsset::GoWest(float &speed) {
  Vector2 c = *(bbox->centre) + Vector2(0 - speed, 0.0f);
  if(!(c.getX() < 0)) {
    bbox->centre.reset();
    bbox->centre = make_shared<Vector2>(c);
  }else{speed = 0;}
}

void SFAsset::GoEast(float &speed) {
  int w, h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &w, &h);

  Vector2 c = *(bbox->centre) + Vector2(speed, 0.0f);
  if(!(c.getX() > w)) {
    bbox->centre.reset();
    bbox->centre = make_shared<Vector2>(c);
  }else{speed = 0;}
}

void SFAsset::GoNorth(float &speed) {
  Vector2 c = *(bbox->centre) + Vector2(0.0f, speed);
  int w, h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &w, &h);

  if((!(c.getY() > h))||SFASSET_PLAYER != type) {
  bbox->centre.reset();
  bbox->centre = make_shared<Vector2>(c);
  }else{speed = 0;}
}

void SFAsset::GoSouth(float &speed) {
  Vector2 c = *(bbox->centre) + Vector2(0.0f, 0 - speed);

  if((!(c.getY() < 0))||SFASSET_PLAYER != type) {
  bbox->centre.reset();
  bbox->centre = make_shared<Vector2>(c);
  }else{speed = 0;}
}

bool SFAsset::CollidesWith(shared_ptr<SFAsset> other) {
  return bbox->CollidesWith(other->bbox);
}

shared_ptr<SFBoundingBox> SFAsset::GetBoundingBox() {
  return bbox;
}

void SFAsset::SetNotAlive() {
  type = SFASSET_DEAD;
}

bool SFAsset::IsAlive() {
  return (SFASSET_DEAD != type);
}

void SFAsset::HandleCollision() {
  if(SFASSET_PROJECTILE == type || SFASSET_ALIEN == type || SFASSET_COIN == type || SFASSET_EXPLOSION) {
    SetNotAlive();
  }
}

COINTYPE SFAsset::getCode(){
  return code;
}

void SFAsset::addtoCounter(){
explosioncounter++;
}

int SFAsset::getCounter(){
return explosioncounter;
}
