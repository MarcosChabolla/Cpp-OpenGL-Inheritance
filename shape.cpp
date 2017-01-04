// $Id: shape.cpp,v 1.2 2016/07/30 22:27:52 akhatri Exp $

#include <typeinfo>
#include <unordered_map>
using namespace std;

#include "shape.h"
#include "util.h"

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font, const string& textdata):
      glut_bitmap_font(glut_bitmap_font), textdata(textdata) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle(GLfloat diameter): ellipse(diameter, diameter) {
    DEBUGF('c', this << "(" << diameter << ")");
}

void circle::draw(const vertex& center, const rgbcolor& color) const {
    DEBUGF('d', this << "(" << center << "," << color << ")");
    return ellipse::draw(center, color);
}

polygon::polygon (const vertex_list& vertices): vertices(vertices) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
            polygon({{-width/2.0, -height/2.0}, {-width/2.0, height/2.0},
            {width/2.0, height/2.0}, {width/2.0, -height/2.0}}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

diamond::diamond(const GLfloat width, const GLfloat height):
    polygon(([] (GLfloat w, GLfloat h) -> vertex_list {
        vertex_list vs;

        //top of diamond
        GLfloat x0 = 0;
        GLfloat y0 = (h / 2);
        vs.push_back(vertex{x0, y0});

        //left of diamond
        GLfloat x2 = - (w / 2);
        GLfloat y2 = 0;
        vs.push_back(vertex{x2, y2});

        //bottom of diamond
        GLfloat x1 = 0;
        GLfloat y1 = - (h / 2);
        vs.push_back(vertex{x1, y1});

        //right of diamond
        GLfloat x3 = (w / 2);
        GLfloat y3 = 0;
        vs.push_back(vertex{x3, y3});

        return vs;
    })(width, height)) {
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

triangle::triangle (vertex v1, vertex v2, vertex v3) : 
                                 polygon({v1, v2, v3}) {
   DEBUGF ('c', this << "(" << v1 << "," << v2 << "," << v3 << ")");
}

equilateral::equilateral (GLfloat side) : triangle(
   {0.0, 0.8660*side}, {-side/2.0, 0.0}, {side/2.0, 0.0} ) {
   DEBUGF ('c', this << "(" << side <<  ")");
}

text::text(const string & font, const string &textdata) {
   auto itor = fontcode.find(font);
   
   if(itor == fontcode.end()) {
      throw runtime_error("Invalid font code : " + font);
   }
   
   glut_bitmap_font = itor->second;
   this->textdata = textdata;
}
void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   rgbcolor newColor(color);
   glColor3ubv (newColor.ubvec);
   glRasterPos2i (center.xpos, center.ypos);
   glutBitmapString (glut_bitmap_font, (GLubyte*) textdata.c_str());
}
void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   rgbcolor newColor(color);
   glBegin (GL_POLYGON);
   glEnable (GL_LINE_SMOOTH);
   glColor3ubv (newColor.ubvec3());
   float x,y;
   float t;
   for(t = 0.0; t <= 360; t +=0.02){
      x = center.xpos + dimension.xpos*sin(t);
      y = center.ypos + dimension.ypos*cos(t);
      glVertex2f(x,y);
   }
    glEnd();
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   
   glBegin (GL_POLYGON);
   rgbcolor newColor(color);
   glColor3ubv (newColor.ubvec3());

   for(auto vert : vertices) {
      glVertex2f (vert.xpos+center.xpos,
            vert.ypos+center.ypos);
   }
   glEnd();
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

void text::border(vertex center, float width, rgbcolor color) const {
   DEBUGF ('d', this << "(" << width << "," << color << ")");
  

}

void ellipse::border(vertex center, float width, rgbcolor color) const {
   DEBUGF ('d', this << "(" << width << "," << color << ")");
   rgbcolor newColor(color);
   glLineWidth(width); 
   glEnable (GL_LINE_SMOOTH);
   glColor3ubv (newColor.ubvec3());
   glBegin (GL_LINES);
   float x,y;
   float t;
   for(t = 0.0; t <= 360; t +=0.02)
   {
      x = center.xpos + dimension.xpos*sin(t);
      y = center.ypos + dimension.ypos*cos(t);
      glVertex2f(x,y);
    }
    glEnd();
}

void polygon::border(vertex center, float width, rgbcolor color) const {
   glLineWidth(width); 
   rgbcolor newColor(color);
   glColor3ubv(newColor.ubvec3());
   glBegin(GL_LINES);
   for(int i = 0; i < vertices.size()-1; ++i) {
      glVertex2f (vertices[i].xpos+center.xpos,
            vertices[i].ypos+center.ypos);
      glVertex2f (vertices[i+1].xpos+center.xpos,
            vertices[i+1].ypos+center.ypos);
   }
   glVertex2f(vertices.back().xpos+center.xpos, 
      vertices.back().ypos+center.ypos);
   glVertex2f(vertices[0].xpos+center.xpos, 
      vertices[0].ypos+center.ypos);
   glEnd();
}
