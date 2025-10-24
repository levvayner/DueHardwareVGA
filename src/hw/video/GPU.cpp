#include "GPU.h"

GPU::GPU(RenderMode mode)
{
    _graphics = new VRAM();
    _renderMode = mode;
    _graphics->begin();
}

void GPU::Render()
{
    //2D graphics
    for(int idx=0;idx < _graphics2D.objectCount; idx++){
    //for( GraphicsObject2D obj : _graphics2D.objects){
        
        if(_activeBank == 0 && !(_graphics2D.objects+ idx)->drawnOnMem1){
            Draw2DObject((_graphics2D.objects+ idx));
            (_graphics2D.objects+ idx)->drawnOnMem1 = true;
        }
        else if(_activeBank == 1 && !(_graphics2D.objects+ idx)->drawnOnMem2){
            (_graphics2D.objects+ idx)->drawnOnMem2 = true;
        }
    }
}

void GPU::Draw2DObject(GraphicsObject2D* obj)
{
    if(obj->shape->shape == Line){
        _graphics->drawLine(obj->shape->vertecies[0], obj->shape->vertecies[1],obj->texture->colors[0]);
    }
    else if(obj->shape->shape == Triangle){
        if(obj->shape->style == FillStyle::Outline)
            _graphics->drawTriangle(
                obj->shape->vertecies[0].x, obj->shape->vertecies[0].y ,
                obj->shape->vertecies[1].x, obj->shape->vertecies[1].y,
                obj->shape->vertecies[2].x,obj->shape->vertecies[2].y,
                obj->texture->colors[0]
            );
            
        else if(obj->shape->style == FillStyle::Fill)
            _graphics->fillTriangle(
                obj->shape->vertecies[0].x, obj->shape->vertecies[0].y ,
                obj->shape->vertecies[1].x, obj->shape->vertecies[1].y,
                obj->shape->vertecies[2].x,obj->shape->vertecies[2].y,
                obj->texture->colors[0]
            );
    }
    else if(obj->shape->shape == Rectangle){
        if(obj->shape->style == FillStyle::Outline)
            _graphics->drawRectangle(
                obj->shape->vertecies[0],
                obj->shape->vertecies[1],
                obj->texture->colors[0]
            );
            
        else if(obj->shape->style == FillStyle::Fill)
            _graphics->fillRectangle(
                obj->shape->vertecies[0],
                obj->shape->vertecies[1],
                obj->texture->colors[0]
            );
    }

    else if(obj->shape->shape == Circle){
        Circle2D* circle = static_cast<Circle2D*>(obj->shape);
        if(obj->shape->style == FillStyle::Outline)
            _graphics->drawCircle(
                circle->vertecies[0].x,
                circle->vertecies[0].y,
                circle->radius,
                obj->texture->colors[0]
            );
            
        else if(obj->shape->style == FillStyle::Fill)
            _graphics->fillCircle(
                circle->vertecies[0].x,
                circle->vertecies[0].y,
                circle->radius,
                obj->texture->colors[0]
            );
    }
    else if(obj->shape->shape == Oval){
        Oval2D* oval = static_cast<Oval2D*>(obj->shape);
        if(obj->shape->style == FillStyle::Outline)
            _graphics->drawOval(
                oval->vertecies[0].x,
                oval->vertecies[0].y,
                oval->radiusX,
                oval->radiusY,
                obj->texture->colors[0]
            );
            
        else if(obj->shape->style == FillStyle::Fill)
            _graphics->fillOval(
                oval->vertecies[0].x,
                oval->vertecies[0].y,
                oval->radiusX,
                oval->radiusY,
                obj->texture->colors[0]
            );
    }
    else if(obj->shape->shape == Arc){
        Arc2D* arc = static_cast<Arc2D*>(obj->shape);
        if(obj->shape->style == FillStyle::Outline)
            _graphics->drawArc(
                arc->vertecies[0].x,
                arc->vertecies[0].y,
                arc->startDeg,
                arc->endDeg,
                arc->radius,
                obj->texture->colors[0]
            );
            
        else if(obj->shape->style == FillStyle::Fill){
        //    _graphics->fillArc(
        //         arc->vertecies[0].x,
        //         arc->vertecies[0].y,
        //         arc->startDeg,
        //         arc->endDeg,
        //         arc->radius,
        //         obj->texture->colors[0]
        //     );
        }
        
    }
    //TODO: add polygon support

    // if(Line2D* lPtr = dynamic_cast<Line2D*>(obj->shape)){
    //     _graphics->drawLine(lPtr->p1(), obj->shape->vertecies[1],obj->texture->colors[0]);
    // }
    // else if(Line2D* lPtr = dynamic_cast<Line2D*>(obj->shape)){
    // }
    // else if(Triangle2D* lPtr = dynamic_cast<Triangle2D*>(obj->shape)){
    //     if(obj->shape->style == FillStyle::Outline)
    //         _graphics->drawTriangle(lPtr->p1().x, lPtr->p1().y , obj->shape->vertecies[1].x, obj->shape->vertecies[1].y, obj->shape->vertecies[2].x,obj->shape->vertecies[2].y,obj->texture->colors[0]);
    //     else if(obj->shape->style == FillStyle::Fill){
    //         _graphics->fillTriangle(lPtr->p1().x, lPtr->p1().y , obj->shape->vertecies[1].x, obj->shape->vertecies[1].y, obj->shape->vertecies[2].x,obj->shape->vertecies[2].y,obj->texture->colors[0]);
    //     }

    // }
    // else if(Rectangle2D* lPtr = dynamic_cast<Rectangle2D*>(obj->shape)){
    // }
    // else if(Circle2D* lPtr = dynamic_cast<Circle2D*>(obj->shape)){
    // }
    // else if(Oval2D* lPtr = dynamic_cast<Oval2D*>(obj->shape)){
    // }
    // else if(Arc2D* lPtr = dynamic_cast<Arc2D*>(obj->shape)){
    // }
    // else if(Line2D* lPtr = dynamic_cast<Line2D*>(obj->shape)){
    // }
}
