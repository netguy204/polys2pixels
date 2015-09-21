#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <stdlib.h>
#include <stdint.h>

class Point {
 public:
  int x, y;

  inline Point(int x, int y)
      : x(x), y(y) {
  }

  inline Point operator-(const Point& o) const {
    return Point(x - o.x,  y - o.y);
  }

  inline Point operator+(const Point& o) const {
    return Point(x + o.x, y + o.y);
  }

  inline Point operator*(const int s) const {
    return Point(x * s, y * s);
  }

  inline Point min(const Point& o) const {
    return Point(std::min(x, o.x), std::min(y, o.y));
  }

  inline Point max(const Point& o) const {
    return Point(std::max(x, o.x), std::max(y, o.y));
  }
};

class Rect {
 public:
  Point ll, ur;

  inline Rect(const Point& p)
      : ll(p), ur(p) {
  }

  inline Rect(const Point& ll, const Point& ur)
      : ll(ll), ur(ur) {
  }

  inline Rect include(const Point& p) {
    return Rect(ll.min(p), ur.max(p));
  }

  inline bool contains(const Point& p) const {
    return p.x >= ll.x && p.x < ur.x && p.y >= ll.y && p.y < ur.y;
  }

  inline int width() const {
    return ur.x - ll.x;
  }

  inline int height() const {
    return ur.y - ll.y;
  }
};

class Triangle {
 public:
  Point a, b, c;

  inline Triangle(const Point& a, const Point& b, const Point& c)
      : a(a), b(b), c(c) {
  }

  inline Rect AABB() const {
    return Rect(a).include(b).include(c);
  }

  inline int orient(const Point& aa, const Point& bb, const Point& cc) const {
    return (bb.x - aa.x) * (cc.y - aa.y) - (bb.y - aa.y) * (cc.x - aa.x);
  }

  inline bool contains(const Point& p) const {
    int w0 = orient(b, c, p);
    int w1 = orient(c, a, p);
    int w2 = orient(a, b, p);
    return w0 >= 0 && w1 >= 0 && w2 >= 0;
  }
};


class Renderer {
 public:
  int substep;
  int submask;

  // in pixels
  Rect bounds;
  uint8_t* memory;

  Renderer(int substep, Rect bounds)
      : substep(substep), submask(substep - 1), bounds(bounds) {
    memory = (uint8_t*)malloc(sizeof(memory[0]) * bounds.width() * bounds.height());
  }

  ~Renderer() {
    free(memory);
  }

  void clear() {
    memset(memory, 0, sizeof(memory[0]) * bounds.width() * bounds.height());
  }

  uint8_t& get(int x, int y) {
    return memory[(y - bounds.ll.y) * bounds.width() + (x - bounds.ll.x)];
  }

  void render(const Triangle& tri) {
    for(int yy = bounds.ll.y; yy < bounds.ur.y; ++yy) {
      for(int xx = bounds.ll.x; xx < bounds.ur.x; ++xx) {
        if(tri.contains(Point(xx*substep, yy*substep))) {
          get(xx, yy) += 1;
        }
      }
    }
  }

  void dump() {
    for(int yy = bounds.ll.y; yy < bounds.ur.y; ++yy) {
      for(int xx = bounds.ll.x; xx < bounds.ur.x; ++xx) {
        if(get(xx, yy)) {
          printf("%c", '0' + get(xx, yy));
        } else {
          printf(" ");
        }
      }
      printf("\n");
    }
  }
};

int main(int argc, char** argv) {
  const int sx = 80;
  const int sy = 30;
  const int step = 256;
  Triangle tri(Point(0, 0) * step,
               Point(sx - 1, 0) * step,
               Point(sx / 2, sy - 1) * step);

  Renderer renderer(step, Rect(Point(0, 0), Point(sx, sy)));
  renderer.clear();
  renderer.render(tri);
  renderer.dump();
}
