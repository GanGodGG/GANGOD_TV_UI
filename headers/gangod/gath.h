#pragma once

#include <iostream>

#include <string>
#include <algorithm>
#include <math.h>
namespace gath{
    struct v2d{
        float x;
        float y;

        bool operator ==(v2d second){
            return x == second.x && y == second.y;
        }

        v2d operator /(float value){
            return {x/value, y/value};
        }
        v2d operator *(float value){
            return {x*value, y*value};
        }
    };
}