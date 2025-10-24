#include "VRAM.h"

enum RenderMode{
    Direct = 0,
    Buffered = 1
};

class GPU
{
    public:
        GPU(RenderMode mode);
        void Render();

    protected:
        void Draw2DObject(GraphicsObject2D* shape);
    private:
        VRAM* _graphics;
        RenderMode _renderMode;
        uint8_t _activeBank = 0;
        Graphics2D _graphics2D;
};