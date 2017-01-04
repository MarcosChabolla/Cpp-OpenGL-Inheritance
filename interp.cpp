// $Id: interp.cpp,v 1.3 2016/07/30 22:27:52 akhatri Exp $

#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"moveby"   , &interpreter::do_moveby},
   {"border"   , &interpreter::do_border},
};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
   {"triangle" , &interpreter::make_triangle },
   {"equilateral" , &interpreter::make_equilateral },
   {"diamond" , &interpreter::make_diamond },
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
    //cout<<"SECOND IS: "<<itor->second<<endl;
   func (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.emplace (name, make_shape (++begin, end));
}


void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
  
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   rgbcolor color {begin[0]};
   itor->second->draw (where, color);
   //rgbcolor color {begin[0]};
   object new_obj(itor->second, where, color);
   window::push_back(new_obj);
}

void interpreter::do_moveby (param begin, param end) {
   DEBUGF ('f', range (begin, end));

   object & curr = window::get_back();
   float x = atof(begin->c_str());
   curr.set_move(x);
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   
   factoryfn funct = itor->second;
   return funct (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
  DEBUGF ('f', range (begin, end));

   string font = *begin;
   ++begin;
   string str = "";
   for(auto it = begin; it != end; ++it) {
      str += *it;
      str += " ";
   }
   if(str.length())  {
      return make_shared<text> (font, 
             str.substr(0, str.length()-1));
   }
   else {
      return make_shared<text> (font, str);
   }
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   float first = atof(begin->c_str());
   ++begin;
   float second = atof(begin->c_str());
   return make_shared<ellipse> (GLfloat(first), GLfloat(second));
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   GLfloat diameter = stof(begin[0]);
   return make_shared<circle> (diameter);
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   
   vertex_list vlist;
   auto itor = begin;
   while(itor != end) {
      float first = atof(itor->c_str());
      ++itor;
      float second = atof(itor->c_str());
      ++itor;
      vlist.push_back(vertex(first, second));
   }
   return make_shared<polygon> (vlist);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   float first = atof(begin->c_str());
   ++begin;
   float second = atof(begin->c_str());
   return make_shared<rectangle> (GLfloat(first), GLfloat(second));
}

shape_ptr interpreter::make_diamond (param begin, param end) {
    DEBUGF ('f', range (begin, end));
    GLfloat width = stof(begin[0]);
    GLfloat height = stof(begin[1]);
    return make_shared<diamond> (width, height);
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   float first = atof(begin->c_str());
   return make_shared<square> (GLfloat(first));
}

shape_ptr interpreter::make_triangle (param begin, param end) {
   DEBUGF('f', range(begin, end));
   vertex_list vlist;
   auto itor = begin;
   
   while(itor != end) {
      float first = atof(itor->c_str());
      ++itor;
      float second = atof(itor->c_str());
      ++itor;
      vlist.push_back(vertex(first, second));
   }
   
   return make_shared<triangle> (vlist[0], vlist[1], vlist[2]);
} 

shape_ptr interpreter::make_equilateral (param begin, param end) {
   DEBUGF('f', range(begin, end));
   float first = atof(begin->c_str());
   return make_shared<equilateral> (GLfloat(first));
}

void interpreter::do_border (param begin, param end) {
   DEBUGF ('f', range (begin, end));

   object & curr = window::get_back();
   rgbcolor color {begin[0]};
   float a = atof(begin[1].c_str());
   curr.set_border(a, color);
}
