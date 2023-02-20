#include "FrameBuffer.h"
#include "glm/glm.hpp"

int             FrameBuffer::width     = 0;
int             FrameBuffer::height    = 0;
float* FrameBuffer::imageData = nullptr;
unsigned int * FrameBuffer::counterData = nullptr;

void FrameBuffer::Init(int w, int h)
{
    width     = w;
    height    = h;
    int size  = 3 * width * height;
    imageData = new float[size];
    //set to 0
    for (unsigned int i = 0; i < size; i++)
    {
        imageData[i] = 0.0f;
    }
    //TOFDO ALLOCATE
    size = width * height;
    counterData = new unsigned int[size];
    //set to 0
    for (unsigned int i = 0; i < size; i++)
    {
        counterData[i] = 0;
    }
}

void FrameBuffer::Free()
{
    delete[] imageData;
    //TODO
    delete[] counterData;
}

void FrameBuffer::Clear(unsigned char r, unsigned char g, unsigned char b)
{
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            imageData[(y * width + x) * 3 + 0] = r;
            imageData[(y * width + x) * 3 + 1] = g;
            imageData[(y * width + x) * 3 + 2] = b;
        }
    }
}

void FrameBuffer::SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
    if (x >= 0 && y >= 0 && x < width && y < height)
    {
        imageData[(y * width + x) * 3 + 0] = r;
        imageData[(y * width + x) * 3 + 1] = g;
        imageData[(y * width + x) * 3 + 2] = b;
    }
}

void FrameBuffer::GetPixel(int x, int y, unsigned char & r, unsigned char & g, unsigned char & b)
{
    if (x >= 0 && y >= 0 && x < width && y < height)
    {
        float r_f = imageData[(y * width + x) * 3 + 0] / counterData[(y * width + x)];
        float g_f = imageData[(y * width + x) * 3 + 1] / counterData[(y * width + x)];
        float b_f = imageData[(y * width + x) * 3 + 2] / counterData[(y * width + x)];

        //now we need to change from 0-1 format to 0-255 format
        //clamp values to 0 - 1
        r_f = glm::clamp(r_f, 0.0f, 1.0f);
        g_f = glm::clamp(g_f, 0.0f, 1.0f);
        b_f = glm::clamp(b_f, 0.0f, 1.0f);
        //convert to 255
        r_f = r_f * 255.0f;
        g_f = g_f * 255.0f;
        b_f = b_f * 255.0f;
        //convert to char and return
        r = static_cast<char>(r_f);
        g = static_cast<char>(g_f);
        b = static_cast<char>(b_f);

    }
}
void FrameBuffer::AddSample(int x, int y, float r, float g, float b)
{
    if (x >= 0 && y >= 0 && x < width && y < height)
    {
        imageData[(y * width + x) * 3 + 0] += r;
        imageData[(y * width + x) * 3 + 1] += g;
        imageData[(y * width + x) * 3 + 2] += b;

        counterData[(y * width + x)] += 1;
    }
}
// Convert the custom framebuffer to SFML format
void FrameBuffer::ConvertFrameBufferToSFMLImage(sf::Image & image)
{
    int w = FrameBuffer::GetWidth();
    int h = FrameBuffer::GetHeight();

    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            unsigned char r, b, g;
            FrameBuffer::GetPixel(x, y, r, g, b);
            image.setPixel(x, y, sf::Color(r, g, b));
        }
    }
}