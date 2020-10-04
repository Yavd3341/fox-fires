//
// Ilya Yavdoschuk
// FGroup (c) 2019
//

#include "FoxFires.hpp"

#ifdef DEBUG
#define DEBUG_COLOR_BOUNDING_BOX Color::Red
#define DEBUG_COLOR_INITIAL_POSITION Color::Yellow
#define DEBUG_COLOR_CALCULATED_POSITION Color::Green
#define DEBUG_COLOR_STICK_TOP_PATH Color::Cyan
#define DEBUG_COLOR_STICK_BOTTOM_PATH Color:: Magenta
#define DEBUG_COLOR_STICK_LEFT_PATH Color::White
#define DEBUG_COLOR_STICK_RIGHT_PATH Color::Blue
#endif

using namespace FG;

Vector2f Pine::defaultSize = Vector2f(0.1999995, 1);

const double prop = 0.7;

float ease(float f){
  return pow(f, 2);
}

Color getColor(char i){
  return Color(i, i, i, 0xFF);
}

Pine::Pine(Controller * controller, Vector2f position, Vector2f size) : RenderLayer(controller){
  this->controller = controller;
  this->position = position;
  this->size = size;

  logColor = Color(0x2A2016FF);
  sticksColor = Color(0x01796FFF);
}

Pine::Pine(Controller * controller, Vector2f position, float sizeMult) : RenderLayer(controller){
  this->controller = controller;
  this->position = position;
  this->sizeMult = sizeMult;
  this->size = Pine::defaultSize;

  logColor = Color(0x2A2016FF);
  sticksColor = Color(0x01796FFF);
}

void Pine::drawPine(RenderTarget * renderTarget, Vector2f pos, Vector2f size, Color log, Color sticks, unsigned int stickCount, float targetAngle){
  Color shade = getColor(0x88);
  float step = size.y / stickCount;

  Vertex logVertex[] =
  {
    Vertex(Vector2f(pos.x + size.x / 2 - (stickCount - 1.0) / stickCount * (size.x / 15), pos.y + size.y - step * 2), log), // TL
    Vertex(Vector2f(pos.x + size.x / 2 + (stickCount - 1.0) / stickCount * (size.x / 15), pos.y + size.y - step * 2), log), // TR
    Vertex(Vector2f(pos.x + size.x / 2 + size.x / 15, pos.y + size.y), log), // BR
    Vertex(Vector2f(pos.x + size.x / 2 - size.x / 15, pos.y + size.y), log)  // BL
  };

  renderTarget->draw(logVertex, 4, Quads);

  int px = pos.x;

  if (px < 0)
    px = 0;
  else if (px > controller->w - 1)
    px = controller->w - 1;

  px = map(px, 0, controller->w, 0, controller->dataLength);

  Color sky = controller->skyAmbient[px] * Color(0x222222FF);
  std::vector<Vertex> v;
  #ifdef DEBUG
  std::vector<Vertex> vt;
  std::vector<Vertex> vl;
  std::vector<Vertex> vb;
  std::vector<Vertex> vr;
  #endif
  float angleStep = targetAngle / stickCount;
  for (int i = stickCount; i > 0; i--) {
    float tmp = (step * i);
    float tmpAngle = (angleStep * i);

    // Side reference vector
    Vector2f sideRef(-size.x / 2, size.y);

    // Top vertex position
    Vector2f base(pos.x + size.x / 2, pos.y + size.y - tmp);

    // Right vertex position
    Vector2f s1ln = Vector2f(pos.x, pos.y + size.y) - base;
    s1ln = setVectorLenght(s1ln, map(getVectorLenght(s1ln), 0, getVectorLenght(sideRef), tmp, step));

    if (s1ln.y * 0.8 < 5)
      continue;

    // Bottom vertex positions
    Vector2f bottom(0, s1ln.y * 0.8);

    // Left vertex position
    Vector2f s2ln(-s1ln.x, s1ln.y);

    // Rotate vector
    base += Vector2f(0, tmp) - rotateVector(Vector2f(0, tmp), tmpAngle);
    s1ln = rotateVector(s1ln, tmpAngle);
    bottom = rotateVector(bottom, tmpAngle);
    s2ln = rotateVector(s2ln, tmpAngle);

    Color mappedColor = (sticks * getColor(map(i, stickCount, 1, 0xFF, 0x88)) + sky) * (controller->backColor + Color(0x666666FF));
    Color layerShade = getColor(map(i, 1, stickCount, 0x88, 0xFF));
    mappedColor.a = 0xFF;

    v.push_back(Vertex(base, mappedColor * layerShade));
    v.push_back(Vertex(s1ln + base, mappedColor * shade * layerShade));
    v.push_back(Vertex(bottom + base, mappedColor * layerShade));

    v.push_back(Vertex(base, mappedColor * layerShade));
    v.push_back(Vertex(s2ln + base, mappedColor * shade * layerShade));
    v.push_back(Vertex(bottom + base, mappedColor * layerShade));

    #ifdef DEBUG
    vt.push_back(Vertex(base, Color::Cyan));
    vl.push_back(Vertex(s1ln + base, Color::Red));
    vb.push_back(Vertex(bottom + base, Color::Green));
    vr.push_back(Vertex(s2ln + base, Color::Magenta));
    #endif
  }

  renderTarget->draw(&v[0], v.size(), Triangles);
  #ifdef DEBUG
  renderTarget->draw(&vt[0], vt.size(), LineStrip);
  renderTarget->draw(&vl[0], vl.size(), LineStrip);
  renderTarget->draw(&vb[0], vb.size(), LineStrip);
  renderTarget->draw(&vr[0], vr.size(), LineStrip);
  #endif
}

void Pine::draw(RenderTarget * renderTarget){
  Vector2f localSize = size * sizeMult;

  Vector2f winSize(controller->w, controller->h);
  Vector2f tmpPos = position - Vector2f(localSize.x / 2, localSize.y);
  Vector2f tmpSize(localSize.x * winSize.x, localSize.y * winSize.y);

  tmpPos = Vector2f(tmpPos.x * winSize.x, tmpPos.y * winSize.y);

  Pine::drawPine(renderTarget, tmpPos, tmpSize, logColor, sticksColor, stickCount, 0);

#ifdef DEBUG
  RectangleShape rectangle;
  rectangle.setSize(tmpSize);
  rectangle.setOutlineColor(Color::Red);
  rectangle.setFillColor(Color::Transparent);
  rectangle.setOutlineThickness(1);
  rectangle.setPosition(tmpPos);
  renderTarget->draw(rectangle);

  CircleShape circle;
  circle.setRadius(2);
  circle.setFillColor(sf::Color::Yellow);
  circle.setPosition(tmpPos - Vector2f(2, 2));
  renderTarget->draw(circle);

  CircleShape circle2;
  circle2.setRadius(2);
  circle2.setFillColor(sf::Color::Green);
  circle2.setPosition(Vector2f(position.x * winSize.x, position.y * winSize.y) - Vector2f(2, 2));
  renderTarget->draw(circle2);
#endif
}

void Pine::update(){
}
