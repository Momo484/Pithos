#pragma once

struct Square {
    int x;
    int y;

    bool operator==(const Square& other) const {
        return (this->x == other.x && this->y == other.y);
    }
    
    bool operator!=(const Square& other) const {
        return !(*this == other); 
    }
};