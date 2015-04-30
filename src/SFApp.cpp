#include "SFApp.h"

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2(canvas_w/2, canvas_h/2);
  player->SetPosition(player_pos);
  
  back.x = 0;
  back.y = 0;
  back.w = 640;
  back.h = 800;
  back2.x = 0;
  back2.y = 0 - 800;
  back2.w = 640;
  back2.h = 800;
  
  healthpos.x =  canvas_w - 40;
  healthpos.y =  15; 
  healthpos.w = 32;
  healthpos.h = 450;
  
  healthgreenpos.h = 444;
  healthgreenpos.w = 26;
  healthgreenpos.x = healthpos.x + 3;
  healthgreenpos.y = healthpos.y + 3;
  
  gameoverpos.h = 400;
  gameoverpos.w = 560;
  gameoverpos.x = canvas_w / 2 - gameoverpos.w / 2;
  gameoverpos.y = canvas_h / 2 - gameoverpos.h / 2;


  d1 = IMG_LoadTexture(sf_window->getRenderer(), "assets/1.png");
  d2 = IMG_LoadTexture(sf_window->getRenderer(), "assets/2.png");
  d3 = IMG_LoadTexture(sf_window->getRenderer(), "assets/3.png");
  d4 = IMG_LoadTexture(sf_window->getRenderer(), "assets/4.png");  
  d5 = IMG_LoadTexture(sf_window->getRenderer(), "assets/5.png");
  d6 = IMG_LoadTexture(sf_window->getRenderer(), "assets/6.png");
  d7 = IMG_LoadTexture(sf_window->getRenderer(), "assets/7.png");
  d8 = IMG_LoadTexture(sf_window->getRenderer(), "assets/8.png");
  d9 = IMG_LoadTexture(sf_window->getRenderer(), "assets/9.png");
  d0 = IMG_LoadTexture(sf_window->getRenderer(), "assets/0.png");
  scorepos.w = 16;
  scorepos.h = 16;
  scorepos.x = 10;
  scorepos.y = 10;

  gameoverimage = IMG_LoadTexture(sf_window->getRenderer(), "assets/gameover.png");  
  background = IMG_LoadTexture(sf_window->getRenderer(), "assets/background.png");
  healthbar = IMG_LoadTexture(sf_window->getRenderer(), "assets/emptyhealth.png");
  healthgreen = IMG_LoadTexture(sf_window->getRenderer(), "assets/healthbar.png");
}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    left = true;
    break;
  case SFEVENT_PLAYER_RIGHT:  
    right = true;
    break;
  case SFEVENT_PLAYER_UP:
    up = true;
    break;
  case SFEVENT_PLAYER_DOWN:
    down = true;
    break;
  case SFEVENT_PLAYER_LEFT_RELEASED:
    left = false;
    break;
  case SFEVENT_PLAYER_RIGHT_RELEASED:
    right = false;
    break;
  case SFEVENT_PLAYER_UP_RELEASED:
    up = false;
    break;
  case SFEVENT_PLAYER_DOWN_RELEASED:
    down = false;
    break;
  case SFEVENT_FIRE:
    fired = true;
    break;
  case SFEVENT_FIRE_RELEASED:
    fired = false;
    fire = 0;
    break;
  case SFEVENT_LWALL:
    addWall(true);
    break;
  case SFEVENT_RWALL:
    addWall(false);
    break;
  case SFEVENT_RESET:
    Reset();
    break;
  }
}


int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  
  while (SDL_WaitEvent(&event) && is_running) {
    // if this is an update event, then handle it in SFApp,
    // otherwise punt it to the SFEventDispacher.
    SFEvent sfevent((const SDL_Event) event);
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {
  
  
  if(gameover){
  
  }else{
  // Handles Player movement
  if(up){
  if(ups < 5.0f){
   ups += 0.05;
  }else{ups = 5.0f;}
  }else if(ups > 0){
   ups -= 0.05;
  }else{ups = 0;}

  if(down){
   if(downs < 5.0f){
   downs += 0.05;
  }else{downs = 5.0f;}
  }else if(downs > 0){
   downs -= 0.05;
  }else{downs = 0;}

  if(left){
   if(lefts < 5.0f){
   lefts += 0.05;
  }else{lefts = 5.0f;}
  }else if(lefts > 0){
   lefts -= 0.05;
  }else{lefts = 0;}

  if(right){
   if(rights < 5.0f){
   rights += 0.05;
  }else{rights = 5.0f;}
  }else if(rights > 0){
   rights -= 0.05;
  }else{rights = 0;}

  player->GoNorth(ups);
  for(auto w: walls){
    if(w->CollidesWith(player)){
      player->GoSouth(ups);
      ups = 0;
    }
  }

  player->GoSouth(downs);
  for(auto w: walls){
    if(w->CollidesWith(player)){
      player->GoNorth(downs);
      downs = 0;
    }
  }

  player->GoWest(lefts);
  for(auto w: walls){
    if(w->CollidesWith(player)){
      player->GoEast(lefts);
      lefts = 0;
    }
  }

  player->GoEast(rights);
  for(auto w: walls){
    if(w->CollidesWith(player)){
      player->GoWest(rights);
      rights = 0;
    }
  }


  //Handles adding projectiles and moving them

  if(fired){
   if(fire % firespeed == 0){
   FireProjectile();
   }
  fire ++;
  }

  for(auto p: projectiles) {
    p->GoNorth(projectilespeed);
  }

  //collect coin pick ups
  for(auto c: coins) {
    if(c->CollidesWith(player)){
      c->HandleCollision();
      switch(c->getCode()){
      case FASTSHOOT:
       cout << "FAST SHOOT" << endl;
     // firespeed = 5;
     fastshoot = 300;
      break;
      case HEALTH:
       cout << "HEALTH" << endl;
        health += 10;
        if(health > 100){
        health = 100;
        }
        healthgreenpos.h = (444/100)*health;
        healthgreenpos.y = healthpos.y + 3 + (444-healthgreenpos.h);
      break;
      case SLOWALIENS:
      cout << "SLOW ALIENS" << endl;
      slowaliens = 300;
     // alienspeed /= 2;
      break;
      case CLEARALIENS:
      cout << "CLEAR ALIENS" << endl;
      for(auto a: aliens){
      a->HandleCollision();
      score ++;
        if(score % 100 == 0 && score > 0){
           maxAliens++;
           alienspeed += 0.01f;
         }
        if(score % 5 == 0 && score > 0){
        auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
  	auto pos  = a->GetPosition();
  	coin->SetPosition(pos);
  	coins.push_back(coin);
  	}
      }
      break;
      }
      
    }
  }


  if(fastshoot > 0){
  firespeed = 5;
  fastshoot --;
  }else{firespeed = 20;}
  
  if(slowaliens > 0){

  alienspeed = alientempspeed / 2;
  slowaliens --;
  }else{alienspeed = alientempspeed; alientempspeed = alienspeed;}
  
  //remove picked up coins
  list<shared_ptr<SFAsset>> tmp1;
  for(auto c : coins) {
    if(c->IsAlive()) {
      tmp1.push_back(c);
    }
  }
  coins.clear();
  coins = list<shared_ptr<SFAsset>>(tmp1);

  // Update enemy positions
  for(auto a : aliens) {
    a->GoSouth(alienspeed);
  }

  // Detect collisions
  
  for(auto a : aliens) {
    
    for(auto p : projectiles) {
      if(p->CollidesWith(a)) {
        p->HandleCollision();
        a->HandleCollision();
	score ++;
        if(score % 100 == 0 && score > 0){
           maxAliens++;
           alienspeed += 0.01f;
         }
        if(score % 5 == 0 && score > 0){
        auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
  	auto pos  = a->GetPosition();
  	coin->SetPosition(pos);
  	coins.push_back(coin);
  	}
      }
    }
    for(auto w: walls){
       if(w->CollidesWith(a)){
         a->HandleCollision();
         alienspeed += 0.01f;
	 score ++;
         if(score % 100 == 0 && score > 0){
           alienspeed += 0.01f;
           maxAliens++;
         }
       }
    }
    if(a->GetPosition().getY() < 0){
        health -= 10;
        healthgreenpos.h = (444/100)*health;
        healthgreenpos.y = healthpos.y + 3 + (444-healthgreenpos.h);
        a->HandleCollision();
      }
  }

  

  if(currentAliens < maxAliens){
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2(rand()% 640, 480 + (rand()%600));
    alien->SetPosition(pos);
    aliens.push_back(alien);
    currentAliens ++;
  }

  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }else{
    auto explosion = make_shared<SFAsset>(SFASSET_EXPLOSION, sf_window);
    auto pos = a->GetPosition();
    explosion->SetPosition(pos);
    explosions.push_back(explosion);
    currentAliens--;}
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);

  //remove projectiles
   list<shared_ptr<SFAsset>> tmp2;
  for(auto p : projectiles) {
  if(p->GetPosition().getY() > 480){
    p->HandleCollision();
  }
    if(p->IsAlive()) {
      tmp2.push_back(p);
    }
  }
  projectiles.clear();
  projectiles = list<shared_ptr<SFAsset>>(tmp2);

  back.y +=8;
  back2.y +=8;

  if(back.y > 800){
   back.y = back2.y - 800;
  }
  
  if(back2.y > 800){
   back2.y = back.y - 800;
  }
  
  
  list<shared_ptr<SFAsset>> tmp3;
  for(auto e: explosions){
  e->addtoCounter();
  if(e->getCounter() > 20){
  e->HandleCollision();
  }
  if(e->IsAlive()){
  tmp3.push_back(e);
  }
  }
  explosions.clear();
  explosions = list<shared_ptr<SFAsset>>(tmp3);
  
  if(health <= 0){
  gameover = true;
  }
  
  }
}

void SFApp::OnRender() {
  SDL_RenderClear(sf_window->getRenderer());
 
  SDL_RenderCopy(sf_window->getRenderer(), background, NULL, &back);
  SDL_RenderCopy(sf_window->getRenderer(), background, NULL, &back2);

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender();}
  }

  for(auto w: walls) {
    if(w->IsAlive()) {w->OnRender();}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender();}
  }

  for(auto c: coins) {
    c->OnRender();
  }
  
  for(auto e: explosions){
    e->OnRender();
  }
  
  DrawScore(std::to_string(score));
  //  DrawScore("1723");
   // draw the player
  player->OnRender();
  
  SDL_RenderCopy(sf_window->getRenderer(), healthbar, NULL, &healthpos);
  SDL_RenderCopy(sf_window->getRenderer(), healthgreen, NULL, &healthgreenpos);
  
  if(gameover){
      SDL_RenderCopy(sf_window->getRenderer(), gameoverimage, NULL, &gameoverpos);
  }
  
  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
 
void SFApp::Reset(){
  if(gameover){
  aliens.clear();
  currentAliens = 0;
  score = 0;
  coins.clear();
  health = 100;
  healthgreenpos.h = (444.0f/100)*health;
  healthgreenpos.y = healthpos.y + 3 + (444-healthgreenpos.h);
  alienspeed = 1.5f;
  alientempspeed = alienspeed;
  fastshoot = 0;
  slowaliens = 0;
  walls.clear();
  noWalls = 0;
  gameover = false;
  }
}

void SFApp::DrawScore(string message){

int tmp = 0;

for(int i = 0; i < 7; i++){
  if(message.length() - 1  < 6 - i){
    SDL_RenderCopy(sf_window->getRenderer(), d0, NULL, &scorepos);
    tmp ++;
  }else if(message.length() > 0){
  switch(message.at(i - tmp)){
  case '0':
      SDL_RenderCopy(sf_window->getRenderer(), d0, NULL, &scorepos);
      break;
  case '1':
      SDL_RenderCopy(sf_window->getRenderer(), d1, NULL, &scorepos);
      break;
  case '2':
      SDL_RenderCopy(sf_window->getRenderer(), d2, NULL, &scorepos);
      break;
  case '3':
      SDL_RenderCopy(sf_window->getRenderer(), d3, NULL, &scorepos);
      break;
  case '4':
      SDL_RenderCopy(sf_window->getRenderer(), d4, NULL, &scorepos);
      break;
  case '5':
      SDL_RenderCopy(sf_window->getRenderer(), d5, NULL, &scorepos);
      break;
  case '6':
      SDL_RenderCopy(sf_window->getRenderer(), d6, NULL, &scorepos);
      break;
  case '7':
      SDL_RenderCopy(sf_window->getRenderer(), d7, NULL, &scorepos);
      break;
  case '8':
      SDL_RenderCopy(sf_window->getRenderer(), d8, NULL, &scorepos);
      break;
  case '9':
      SDL_RenderCopy(sf_window->getRenderer(), d9, NULL, &scorepos);
      break;
      }
   }
   
   scorepos.x += 16;
}

scorepos.x = 10;

}

void SFApp::addWall(bool Left){
    auto wall = make_shared<SFAsset>(SFASSET_WALL, sf_window);
    auto wall_pos   = player->GetPosition();

    if(noWalls < maxWalls){
    if(Left){
    wall_pos = Point2(wall_pos.getX() - 50.0f, wall_pos.getY());
    }else{
    wall_pos = Point2(wall_pos.getX() + 50.0f, wall_pos.getY());
    }
    wall->SetPosition(wall_pos);
    walls.push_back(wall);
    noWalls++;
    }
}
