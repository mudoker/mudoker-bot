#pragma once

struct BitmapAsset {
    int width;
    int height;
    const unsigned char* data;
};

extern const BitmapAsset ALL_FACES[];
extern const int FACE_COUNT;
