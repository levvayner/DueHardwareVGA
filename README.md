## Due Hardware VGA

### Background
I wanted to create a standalone video card to use with VGA monitors. To do this, I needed a few components:
1. Sync Signal Control - generate HSYNC,VSYNC signals for VGA monitor
2. Video Memory - store data that will be displayed on screen
3. Control Hardware - control when video is drawn to monitor and when data can be modified
4. Video Signal Generation - something to provide structured data to the "video card" to be displayed

I decided to use an Arduino Due to meet the requirement of #4.

### Functionality

1. Text Rendering
2. 2D Graphics Rendering
3. 3D Graphics Rendering (planned)

### Latest Updates (11/05/2025)
Current iteration of the video card has a dual buffer configuration to elimiate timing constraints of a single shared memory bank.
Hardware implementaion can be found at [VGA Hardware Project on GitHub](https://github.com/levvayner/VGA_PLD)

Font generation is now supported. Using [this utility](https://github.com/levvayner/DueVGATTFFontConverter) we can convert TTF fonts to VGA Due format.

### Test firmware is available
You can use [SRAM-Console](https://github.com/levvayner/SRAM-Console) to test the functionality.
That firmware includes a console, editor, some drawing samples, SD card support, and launching apps from the SD card.